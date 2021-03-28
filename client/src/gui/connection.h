#ifndef CONNECTION_H
#define CONNECTION_H

#include <map>
#include <QWidget>
#include <QtDebug>
#include <QMessageBox>
#include <QStyle>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QInputDialog>
#include "Socket.hpp"
#include "../tools/settingsmanager.hpp"

namespace Ui {
class connection;
}

class connection : public QWidget
{
    Q_OBJECT

struct ConnectInfo {
    QString ip;
    QString port;
};

public:
    explicit connection(ISocket *sock, SettingsManager *setting_manager, QWidget *parent = nullptr);
    ~connection();

    void connectToServer(QString ip, QString port);

signals:
    void connected();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void on_select_favorite_comboBox_currentTextChanged(const QString &arg1);
    void on_lineEdit_ip_textEdited(const QString &arg1);
    void on_lineEdit_port_textEdited(const QString &arg1);
    void on_add_favorite_Button_clicked();
    void on_remove_favorite_Button_clicked();

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    void tryConnect();
    void saveConnectionList();

private:
    Ui::connection *ui;
    ISocket *sock;
    SettingsManager *setting_manager;

    std::map<QString, ConnectInfo> favorite;
};

#endif // CONNECTION_H
