#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->screenGeometry(this)));
    ui->setupUi(this);

    this->main_sock = new Socket("mycert.pem", OpensslWrapper::CLIENT);

    this->mode_live = new modeLive(ui->live_tab);

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
    //this->pingServeur("Yoyoyo !!!");
    QObject::connect(this, &MainWindow::new_serv_msg,
                     this, &MainWindow::onMsgFromServer);

    //std::thread t2(&MainWindow::second_thread_test, this);
    //t2.detach();
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

