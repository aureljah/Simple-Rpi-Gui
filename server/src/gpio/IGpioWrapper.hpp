#pragma once

#include <string>
#include <stdint.h>
#include "OpensslWrapper.hpp"

class IGpioWrapper
{
public:
    virtual ~IGpioWrapper() {}

    virtual void setGpioModeInput(int pin) = 0; // set pin to mode input
    virtual void setGpioModeOutput(int pin) = 0; // set pin to mode output

    virtual void writeGpioOutput(int pin, int value) = 0; // write/set pin value (value between 0 and 100)
    virtual int readGpioInput(int pin) = 0; // read a pin (ret 0 or 1)
};
