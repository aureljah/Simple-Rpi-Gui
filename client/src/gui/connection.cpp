#include "connection.h"
#include "ui_connection.h"

connection::connection(ISocket *sock, SettingsManager *setting_manager, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::connection),
    sock(sock), setting_manager(setting_manager)
{
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->screenGeometry(this)));
    ui->setupUi(this);
    ui->lineEdit_ip->setInputMask("000.000.000.000;_");
    ui->lineEdit_port->setInputMask("00000");

    QJsonArray array = this->setting_manager->getArray(SettingsManager::CONECTION_ARRAY);
    for(QJsonArray::iterator it = array.begin(); it != array.end(); it++)
    {
        ConnectInfo info;
        QJsonObject obj = it->toObject();
        QJsonValueRef value = obj.find(this->setting_manager->getFieldStr(SettingsManager::CONECTION_ARR_IP)).value();
        info.ip = value.toString();
        value = obj.find(this->setting_manager->getFieldStr(SettingsManager::CONECTION_ARR_PORT)).value();
        info.port = value.toString();
        value = obj.find(this->setting_manager->getFieldStr(SettingsManager::CONECTION_ARR_NAME)).value();
        this->favorite[value.toString()] = info;
        ui->select_favorite_comboBox->addItem(value.toString());
    }

    //this->favorite["Local"] = {"127.0.0.1", "4242"};
    //ui->select_favorite_comboBox->addItem("Local");
    ui->select_favorite_comboBox->setCurrentIndex(-1);

    ui->lineEdit_ip->setText(this->setting_manager->getDataStr(SettingsManager::LAST_IP_USED));
    ui->lineEdit_port->setText(this->setting_manager->getDataStr(SettingsManager::LAST_PORT_USED));

    this->installEventFilter(this);
}

void connection::connectToServer(QString ip, QString port)
{
    try {
      sock->connect(ip.toStdString(), port.toInt());

      emit connected();
      this->close();
    }
    catch(char const *msg) {
        qInfo() << "Error: " << msg << "\n";
        QMessageBox msgBox;

        msgBox.setText("Error: " + QString(msg));
        //msgBox.show();

        msgBox.exec();
    }
}

connection::~connection()
{
    this->removeEventFilter(this);
    delete ui;
}

void connection::tryConnect()
{
    qInfo() << "Try connecting on " << ui->lineEdit_ip->text()
            << ":" << ui->lineEdit_port->text() << "...\n";

    this->setting_manager->setData(SettingsManager::LAST_IP_USED, ui->lineEdit_ip->text());
    this->setting_manager->setData(SettingsManager::LAST_PORT_USED, ui->lineEdit_port->text());
    this->connectToServer(ui->lineEdit_ip->text(), ui->lineEdit_port->text());
}

void connection::saveConnectionList()
{
    QJsonArray array;
    for(std::map<QString, ConnectInfo>::iterator it = this->favorite.begin(); it != this->favorite.end(); it++)
    {
        ConnectInfo info = it->second;
        QString name = it->first;
        QJsonObject obj;
        obj.insert(this->setting_manager->getFieldStr(SettingsManager::CONECTION_ARR_IP), info.ip);
        obj.insert(this->setting_manager->getFieldStr(SettingsManager::CONECTION_ARR_PORT), info.port);
        obj.insert(this->setting_manager->getFieldStr(SettingsManager::CONECTION_ARR_NAME), name);
        array.append(obj);
    }
    //qInfo() << "saveConnectionList: array(size: " << array.size() << "): " << array << "\n";
    this->setting_manager->setData(SettingsManager::CONECTION_ARRAY, array);
}

void connection::on_buttonBox_accepted()
{
    this->tryConnect();
}

void connection::on_buttonBox_rejected()
{
    this->close();
}

bool connection::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type()==QEvent::KeyPress)
    {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        if (key->key() == Qt::Key_Enter || key->key() == Qt::Key_Return)
           this->tryConnect();
        else
           return QObject::eventFilter(obj, event);

        return true;
    }
    else
        return QObject::eventFilter(obj, event);

  return false;
}

void connection::on_select_favorite_comboBox_currentTextChanged(const QString &arg1)
{
    for(std::map<QString, ConnectInfo>::iterator it = this->favorite.begin(); it != this->favorite.end(); it++)
    {
        if (it->first == arg1)
        {
            ConnectInfo info = it->second;
            ui->lineEdit_ip->setText(info.ip);
            ui->lineEdit_port->setText(info.port);
            break;
        }
    }
}

void connection::on_lineEdit_ip_textEdited(const QString &arg1)
{
    ui->select_favorite_comboBox->setCurrentIndex(-1);
}
void connection::on_lineEdit_port_textEdited(const QString &arg1)
{
    ui->select_favorite_comboBox->setCurrentIndex(-1);
}
void connection::on_add_favorite_Button_clicked()
{
    ConnectInfo info;
    info.ip = ui->lineEdit_ip->text();
    info.port = ui->lineEdit_port->text();

    QString text = QInputDialog::getText(this, "Enter name", "Enter name for " + info.ip + ":" + info.port);
    if (text == "" || this->favorite.count(text) > 0)
    {
        QMessageBox msgBox;
        msgBox.setText("Error: name invalid or already exist");
        msgBox.exec();
        return;
    }
    this->favorite[text] = info;
    ui->select_favorite_comboBox->addItem(text);
    ui->select_favorite_comboBox->setCurrentText(text);

    this->saveConnectionList();
}
void connection::on_remove_favorite_Button_clicked()
{
    QString current_item = ui->select_favorite_comboBox->currentText();
    this->favorite.erase(current_item);
    int cur_idx = ui->select_favorite_comboBox->currentIndex();
    ui->select_favorite_comboBox->setCurrentIndex(-1);
    ui->select_favorite_comboBox->removeItem(cur_idx);

    this->saveConnectionList();
}
