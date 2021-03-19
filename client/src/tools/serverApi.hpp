#pragma once

#include "Socket.hpp"
#include <iostream>
#include <QWidget>
#include <QtDebug>
#include <QMessageBox>
//#include <QString>
//#include <QStyle>
//#include <QDesktopWidget>
#include <thread>
#include <mutex>
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
    std::string getServerIp() { return this->server_socket->getIpStr(); };
    int getServerPort() { return (this->server_socket->getPort()); };
    int getInputReceiverPort() { return (this->getServerPort() + 1); };

    /* API */
    bool getServerStatus();
    bool getServerSetting();
    void setServerSetting(std::string setting, std::string value);

    void liveSetInputServer(int pin, std::string name);
    void liveSetOutputServer(int pin, int value, std::string name);
    void liveDelPinServer(int pin);
    void liveParseInputReceiver(std::string data);

signals:
    void sig_sendAndCheckBasicResp(std::string cmd, std::string fct_name);
    void new_live_input(int, std::string);
    void new_live_output(int, int, std::string);
    void input_value_changed(int, int);

    void stay_alive_setting(bool);

    void send_recv_server_msg(std::string, std::string);

private slots:
    void slot_sendAndCheckBasicResp(std::string cmd, std::string fct_name);
    void sendAndCheckBasicResp(std::string cmd, std::string fct_name);

private:
    std::string sendToServer(std::string cmd);
    std::vector<std::string> splitStrToArray(std::string cmd);
    bool checkBasicRespArray(std::vector<std::string> resp_array, std::string fct_name);

    void parseLiveStatus(std::vector<std::string> resp_array, size_t start_idx);

private:
    ISocket *server_socket;
    std::mutex socket_lock;

    //void (*interceptor)(std::string, std::string);
};
