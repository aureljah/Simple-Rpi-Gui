#pragma once

#include "../gpio/IGpioWrapper.hpp"
#include <iostream>
#include <exception>
#include <string>
#include <mutex>

namespace GPIO_TYPE {
    enum gpio_type {
        INPUT,
        OUTPUT
    };
}

class LivePin
{
public:
    LivePin(IGpioWrapper *rpi, int pin, GPIO_TYPE::gpio_type type, std::string name, int value = 0);
    ~LivePin();

public:
    int getPin();
    GPIO_TYPE::gpio_type getType();
    std::string getName();
    int getValue();

    bool setName(std::string name);
    bool setValue(int value);

private:
    IGpioWrapper *rpi;

    int pin;
    GPIO_TYPE::gpio_type type;
    std::string name;
    int value;

    std::mutex set_lock;
};