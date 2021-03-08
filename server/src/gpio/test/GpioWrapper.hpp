#pragma once

#include <iostream>
#include <exception>
#include <string>
#include <cstdio>
#include "../IGpioWrapper.hpp"

/* Dummy GpioWrapper for testing the server outside an RPI */

class GpioWrapper : public IGpioWrapper
{
public:
    GpioWrapper();
    ~GpioWrapper();

public:
    void setGpioModeInput(int pin); // set pin to mode input
    void setGpioModeOutput(int pin); // set pin to mode output

    void writeGpioOutput(int pin, int value); // write/set pin value (value between 0 and 100)
    int readGpioInput(int pin); // read a pin (ret 0 or 1)
    int readGpioInput(int pin, int old_value); // simulate read a pin
};