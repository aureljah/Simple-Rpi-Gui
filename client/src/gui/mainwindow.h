#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QVBoxLayout>
#include <QMainWindow>
#include <QDebug>
#include <QStyle>
#include <QDesktopWidget>
#include <thread>
#include <mutex>
#include <map>
#include "Socket.hpp" // OS specific before other !
#include "../tools/serverApi.hpp"
#include "connection.h"
#include "live/modelive.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void startConnectWin();
    void writeToDebugScreen(std::string msg, std::string type);
    void pingServeur(QString msg);

    void second_thread_test();

    void setServerStayAlive(bool value);

signals:
    void new_serv_msg(QString msg);

private slots:
    void onConnected();
    void on_pushButton_clicked();
    void onMsgFromServer(QString msg);

    void on_live_add_output_pushButton_clicked();
    void on_live_add_input_pushButton_clicked();

    void on_settingStayActive_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    ISocket *main_sock;
    serverApi *server_api;

    modeLive *mode_live;

    /* setting */
    bool server_stay_alive;

    //std::thread *t2;
    std::mutex lock_text_screen;

};

#endif // MAINWINDOW_H
