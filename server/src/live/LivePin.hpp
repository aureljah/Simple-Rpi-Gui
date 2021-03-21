#pragma once

#include "../gpio/IGpioWrapper.hpp"
#include <iostream>
#include <exception>
#include <string>
#include <thread>
#include <mutex>
#include <unistd.h>

#define FADE_RATE_MS 5 // 10ms => 0 to 100 in 1sec

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

    void setFade(bool fade_in, bool fade_out);

private:
    void doFading();

private:
    IGpioWrapper *rpi;

    int pin;
    GPIO_TYPE::gpio_type type;
    std::string name;
    int value; // or target value for fade feature

    // for fade in/out feature
    int real_value; // real value that we incr/decr to target value when fade active
    //std::thread fade_thread;
    bool fade_thread_active;

    bool use_fade_in;
    bool use_fade_out;

    std::mutex set_lock;
};