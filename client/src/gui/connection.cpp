#include "connection.h"
#include "ui_connection.h"

connection::connection(ISocket *sock, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::connection),
    sock(sock)
{
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), qApp->desktop()->screenGeometry(this)));
    ui->setupUi(this);
    ui->lineEdit_ip->setInputMask("000.000.000.000;_");
    ui->lineEdit_port->setInputMask("00000");
    ui->lineEdit_ip->setText("127.0.0.1");
    ui->lineEdit_port->setText("4242");

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

    this->connectToServer(ui->lineEdit_ip->text(), ui->lineEdit_port->text());
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
