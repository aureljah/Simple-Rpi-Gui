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
    std::vector<std::string> splitStrToArray(std::string cmd);
    std::string parseCommand(std::string cmd); // parse cmd => call related fct => give response

    bool useModeLive();

    /* API */
    std::string getStatusCommand();

    std::string liveSetInputCommand(std::string cmd_pin, std::string cmd_name);
    std::string liveSetOutputCommand(std::string cmd_pin, std::string cmd_value, std::string cmd_name);
    std::string liveDelPinCommand(std::string cmd_pin);

private:
    // general settings ?
    // stay active after client disconnect

    // Live class
    LiveMode *live_mode;
    // script class
    // static script transfert class ?

    //ISocket *sock; // server socket (accept)
    //ISocket *client; // client socket (connected)
};