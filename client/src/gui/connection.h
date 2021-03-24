#ifndef CONNECTION_H
#define CONNECTION_H

#include <QWidget>
#include <QtDebug>
#include <QMessageBox>
#include <QStyle>
#include <QDesktopWidget>
#include <QKeyEvent>
#include "Socket.hpp"

namespace Ui {
class connection;
}

class connection : public QWidget
{
    Q_OBJECT

public:
    explicit connection(ISocket *sock, QWidget *parent = nullptr);
    ~connection();

    void connectToServer(QString ip, QString port);

signals:
    void connected();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

protected:
    void tryConnect();
    bool eventFilter(QObject* obj, QEvent* event);

private:
    Ui::connection *ui;
    ISocket *sock;
};

#endif // CONNECTION_H
