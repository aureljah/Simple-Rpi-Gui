#pragma once

#include "Socket.hpp"
#include <iostream>
#include <QWidget>
#include <QtDebug>
#include <QMessageBox>
//#include <QString>
//#include <QStyle>
//#include <QDesktopWidget>
//#include <thread>
//#include <mutex>
#include <string>
#include <map>
#include <vector>

class serverApi : public QObject
{
    Q_OBJECT

public:
    serverApi(ISocket *server_socket);
    ~serverApi();

public:
    /* API */
    bool getServerStatus();
    bool getServerSetting();
    bool setServerSetting(std::string setting, std::string value);

    bool liveSetInputServer(int pin, std::string name);
    bool liveSetOutputServer(int pin, int value, std::string name);
    bool liveDelPinServer(int pin);

signals:
    void new_live_input(int, std::string);
    void new_live_output(int, int, std::string);

    void stay_alive_setting(bool);

    void send_recv_server_msg(std::string, std::string);

private:
    std::string sendToServer(std::string cmd);
    std::vector<std::string> splitStrToArray(std::string cmd);
    bool checkBasicRespArray(std::vector<std::string> resp_array, std::string fct_name);

    void parseLiveStatus(std::vector<std::string> resp_array);

private:
    ISocket *server_socket;

    //void (*interceptor)(std::string, std::string);
};
