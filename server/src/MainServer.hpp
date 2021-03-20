#pragma once

#include "Socket.hpp"
#include "OpensslWrapper.hpp"
#include "./live/LiveMode.hpp"
#include <iostream>
#include <exception>
#include <string>
#include <thread>
#include <vector>

class MainServer
{
public:
    MainServer();
    ~MainServer();

public:
    void run(int port, std::string cert_path); // inf loop on socket accept & socket read

private:
    void onDisconnected();
    std::vector<std::string> splitStrToArray(std::string cmd);
    std::string parseCommand(std::string cmd); // parse cmd => call related fct => give response

    bool useModeLive();
    bool deleteAllMode();

    /* API */
    std::string getStatusCommand();
    std::string getSettingCommand();
    std::string setSettingCommand(std::string setting, std::string value);

    std::string liveSetInputCommand(std::string cmd_pin, std::string cmd_name);
    std::string parseLiveSetOutputCommand(std::vector<std::string> cmd_array, size_t idx_start);
    std::string liveSetOutputCommand(std::string cmd_pin, std::string cmd_value, std::string cmd_name);
    std::string liveDelPinCommand(std::string cmd_pin);

    /* TOOLS */
    std::string boolToString(bool var);
    bool stringToBool(std::string str);

    bool isPinNumberValid(int pin);
    bool isValueValid(int value);
    bool isNameValid(std::string name);
    bool isBoolStringValid(std::string str_bool);

private:
    // Live class
    LiveMode *live_mode;
    // script class
    // static script transfert class ?

    // general settings
    bool setting_stay_active; // server will stay active and dont delete/stop live or script after client disconnect

    int base_port;
    //ISocket *sock; // server socket (accept)
    //ISocket *client; // client socket (connected)
};