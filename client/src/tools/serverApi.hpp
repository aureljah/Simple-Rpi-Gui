#pragma once

#include "Socket.hpp"
#include <iostream>
#include <QWidget>
#include <QtDebug>
#include <QMessageBox>
//#include <QString>
//#include <QStyle>
//#include <QDesktopWidget>
#include <QDateTime>
#include <thread>
#include <mutex>
#include <string>
#include <map>
#include <vector>

#define THREAD_LIMIT 10

#define ERROR_MSGBOX_TIMEOUT 5000 // 5 sec

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
    void liveSetSeveralOutputServer(std::vector<int> pins, std::vector<int> values, std::vector<std::string> names);
    void liveDelPinServer(int pin);
    void liveParseInputReceiver(std::string data);

signals:
    void sig_sendAndCheckBasicResp(std::string cmd, std::string fct_name, bool can_skip);
    void new_live_input(int, std::string);
    void new_live_output(int, int, std::string);
    void input_value_changed(int, int);

    void stay_alive_setting(bool);
    void use_fade_in_setting(bool);
    void use_fade_out_setting(bool);

    void error_500(std::string, std::string);
    void send_recv_server_msg(std::string, std::string);

private slots:
    void slot_sendAndCheckBasicResp(std::string cmd, std::string fct_name, bool can_skip);
    void sendAndCheckBasicResp(std::string cmd, std::string fct_name); //* ONLY FOR THREAD

private:
    std::string sendToServer(std::string cmd);
    std::vector<std::string> splitStrToArray(std::string cmd);
    bool checkBasicRespArray(std::vector<std::string> resp_array, std::string fct_name);

    void parseLiveStatus(std::vector<std::string> resp_array, size_t start_idx);

private:
    ISocket *server_socket;
    std::mutex socket_lock;
    std::mutex error_handler_lock;
    int thread_count;
    std::mutex thread_count_lock;

    qint64 last_err_time;

    //void (*interceptor)(std::string, std::string);
};
