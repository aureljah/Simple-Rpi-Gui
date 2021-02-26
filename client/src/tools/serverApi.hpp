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
    //void setInterceptor(void (*interceptor)(std::string, std::string) = nullptr);

    /* API */
    //std::string getServerStatus();
    //std::string getServerSetting();
    //bool setServerSetting(std::string setting, std::string value);

    bool liveSetInputServer(int pin, std::string name);
    bool liveSetOutputServer(int pin, int value, std::string name);
    bool liveDelPinServer(int pin);

signals:
    void send_recv_server_msg(std::string, std::string);

private:
    std::string sendToServer(std::string cmd);
    std::vector<std::string> splitStrToArray(std::string cmd);
    bool checkBasicRespArray(std::vector<std::string> resp_array, std::string fct_name);

private:
    ISocket *server_socket;

    //void (*interceptor)(std::string, std::string);
};
