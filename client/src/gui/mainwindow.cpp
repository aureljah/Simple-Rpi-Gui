#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), server_stay_alive(false)
{
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->screenGeometry(this)));
    ui->setupUi(this);

    this->main_sock = new Socket("mycert.pem", OpensslWrapper::CLIENT);
    this->server_api = new serverApi(this->main_sock);
    QObject::connect(this->server_api, &serverApi::send_recv_server_msg,
                     this, &MainWindow::writeToDebugScreen);
    QObject::connect(this->server_api, &serverApi::stay_alive_setting,
                     this, &MainWindow::setServerStayAlive);

    this->mode_live = new modeLive(ui->live_tab, this->server_api);
    QObject::connect(this->server_api, &serverApi::new_live_input,
                     this->mode_live, &modeLive::add_input);
    QObject::connect(this->server_api, &serverApi::new_live_output,
                     this->mode_live, &modeLive::add_output);
    QObject::connect(this->server_api, &serverApi::input_value_changed,
                     this->mode_live, &modeLive::update_input_value);


    this->startConnectWin();
}

MainWindow::~MainWindow()
{
    delete this->main_sock;
    delete this->ui;
}

void MainWindow::startConnectWin()
{
    auto win = new connection(this->main_sock);

    QObject::connect(win, &connection::connected,
                     this, &MainWindow::onConnected);

    win->setAttribute(Qt::WA_DeleteOnClose);
    win->show();
}

void MainWindow::writeToDebugScreen(std::string msg, std::string type)
{
    QString full_msg = "";
    if (type == "send")
    {
        full_msg = "[CLIENT]: " + QString::fromStdString(msg);
    }
    else if (type == "recv")
    {
        full_msg = "[SERVER]: " + QString::fromStdString(msg);
    }
    else
        return;

    lock_text_screen.lock();
    ui->plainTextEdit->appendPlainText(full_msg);
    lock_text_screen.unlock();
}

void MainWindow::pingServeur(QString msg)
{
    lock_text_screen.lock();
    this->main_sock->write(msg.toStdString());
    ui->plainTextEdit->appendPlainText("[CLIENT]: " + msg);
    std::string serv_msg = this->main_sock->read(4096);
    ui->plainTextEdit->appendPlainText("[SERVER]: " + QString::fromStdString((serv_msg)));
    lock_text_screen.unlock();
    //qInfo() << "[CLIENT] server: " << QString::fromStdString(msg) << "\n";
}

void MainWindow::second_thread_test()
{
    try {
    ISocket *sock = new Socket("mycert.pem", OpensslWrapper::SERVER);
    ISocket *server = nullptr;

    sock->bind(4243);
    sock->listen(1);
    if ((server = sock->accept()) != NULL)
    {
        while (true)
        {
            try {
            std::string msg;
            msg = server->read(4096);
            qInfo() << "INFO 2nd thread: msg_read: " << QString::fromStdString(msg);
            emit new_serv_msg(QString::fromStdString(msg));
            //lock_text_screen.lock();
            //ui->plainTextEdit->appendPlainText("[SERVER]: " + QString::fromStdString(msg));
            //lock_text_screen.unlock();
            }
            catch(char const *msg) {
              std::cerr << "Error: " << msg << "\n";
              break;
            }
        }
        delete server;
    }
    delete sock;
   }
    catch(char const *msg) {
      std::cerr << "Error: " << msg << "\n";
    }
}

void MainWindow::onMsgFromServer(QString msg)
{
    ui->plainTextEdit->appendPlainText("[SERVER]: " + msg);
}

void MainWindow::onConnected()
{
    this->show();
    QObject::connect(this, &MainWindow::new_serv_msg,
                     this, &MainWindow::onMsgFromServer);

    this->server_api->getServerStatus();
    this->server_api->getServerSetting();
}

/* send ping/msg to server boutton */
void MainWindow::on_pushButton_clicked()
{
    qInfo() << "[DEBUG]: Debug input: " << ui->lineEdit->text() << "\n";
    this->pingServeur(ui->lineEdit->text());
    ui->lineEdit->clear();
}

/* add output boutton */
void MainWindow::on_live_add_output_pushButton_clicked()
{
    this->mode_live->startGpioSettingDialog(gpioSettingDialog::OUTPUT);
}

/* add output boutton */
void MainWindow::on_live_add_input_pushButton_clicked()
{
    this->mode_live->startGpioSettingDialog(gpioSettingDialog::INPUT);
}

void MainWindow::setServerStayAlive(bool value)
{
    this->server_stay_alive = value;
    ui->settingStayActive->setChecked(value);
}

void MainWindow::on_settingStayActive_toggled(bool checked)
{
    this->server_stay_alive = checked;
    if (checked == true)
        this->server_api->setServerSetting("stay_active", "true");
    else
        this->server_api->setServerSetting("stay_active", "false");
}
