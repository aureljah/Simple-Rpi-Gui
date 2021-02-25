#pragma once

#include "Socket.hpp"
#include "OpensslWrapper.hpp"
#include "../gpio/IGpioWrapper.hpp"
#include "GpioWrapper.hpp"
#include "LivePin.hpp"
#include <iostream>
#include <exception>
#include <string>
#include <thread>
#include <map>

class LiveMode
{
public:
    LiveMode();
    ~LiveMode();

public:
    std::string getStatus();

    std::string setOutput(int pin, int value, std::string name);
    std::string setInput(int pin, std::string name);
    std::string delPin(int pin);

private:
    // thread/socket to read all output ?

private:
    IGpioWrapper *rpi;

    std::map<int, LivePin*> outputs;
    std::map<int, LivePin*> inputs;

};