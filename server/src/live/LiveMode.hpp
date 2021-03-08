#pragma once

#include "Socket.hpp"
#include "OpensslWrapper.hpp"
#include "../gpio/IGpioWrapper.hpp"
#include "GpioWrapper.hpp"
#include "LivePin.hpp"
#include <cmath>
#include <iostream>
#include <exception>
#include <string>
#include <thread>
#include <csignal>
#include <map>

#define DEFAULT_MONITOR_POLL_TIME 500

class LiveMode
{
public:
    LiveMode(int base_port);
    ~LiveMode();

public:
    std::string getStatus();

    std::string setOutput(int pin, int value, std::string name);
    std::string setInput(int pin, std::string name);
    std::string delPin(int pin);

private:
    void input_monitor(int base_port);

private:
    IGpioWrapper *rpi;

    std::thread *input_monitor_thread;
    bool input_monitor_running;
    int monitor_poll_time;

    std::map<int, LivePin*> outputs;
    std::map<int, LivePin*> inputs;
};