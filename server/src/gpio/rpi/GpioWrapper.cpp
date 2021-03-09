#include "GpioWrapper.hpp"

GpioWrapper::GpioWrapper() {
    if (gpioInitialise() < 0)
    {
        std::cerr << "[ERROR]: pigpio initialisation failed\n";
        return;
    }
    std::cout << "[GPIO]: Gpio initialised\n";
}

GpioWrapper::~GpioWrapper() {
    gpioTerminate();
    std::cout << "[GPIO]: Gpio terminated\n";
}

void GpioWrapper::setGpioModeInput(int pin) {
    gpioSetMode(pin, PI_INPUT);
    std::cout << "[GPIO]: Setting pin " << pin << " to INPUT mode\n";
}

void GpioWrapper::setGpioModeOutput(int pin) {
    gpioSetMode(pin, PI_OUTPUT);
    std::cout << "[GPIO]: Setting pin " << pin << " to OUTPUT mode\n";
}

void GpioWrapper::writeGpioOutput(int pin, int value) {
    // gpioWrite() or gpioPWM()
    int rpi_value = floor(value * 2.55);
    if (rpi_value > 255)
    {
        std::cout << "[GPIO WARNING]: rpi_value > 255: rpi_value: " << rpi_value << "\n";
        rpi_value = 255;
    }
    else if (rpi_value < 0)
    {
        std::cout << "[GPIO WARNING]: rpi_value < 0: rpi_value: " << rpi_value << "\n";
        rpi_value = 0;
    }

    gpioPWM(pin, rpi_value);
    std::cout << "[GPIO]: Writing pin " << pin << " to " << value << "%\n";
}

int GpioWrapper::readGpioInput(int pin) {
    int value = gpioRead(pin);
    value = value * 100;
    if (value > 100)
    {
        std::cout << "[GPIO WARNING]: gpioRead value > 100: rpi_value: " << value << "\n";
        value = 100;
    }
    //std::cout << "[GPIO]: Reading pin " << pin << " value = " << value << "\n";
    return value;
}

int GpioWrapper::readGpioInput(int pin, int old_value) {
    return this->readGpioInput(pin);
}