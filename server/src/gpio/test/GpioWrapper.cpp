#include "GpioWrapper.hpp"

GpioWrapper::GpioWrapper() {
    //gpioInitialise()
    std::cout << "[GPIO]: Gpio initialised\n";
}

GpioWrapper::~GpioWrapper() {
    //gpioTerminate()
    std::cout << "[GPIO]: Gpio terminated\n";
}

void GpioWrapper::setGpioModeInput(int pin) {
    // gpioSetMode(pin, PI_INPUT)
    std::cout << "[GPIO]: Setting pin " << pin << " to INPUT mode\n";
}

void GpioWrapper::setGpioModeOutput(int pin) {
    // gpioSetMode(pin, PI_OUTPUT)
    std::cout << "[GPIO]: Setting pin " << pin << " to OUTPUT mode\n";
}

void GpioWrapper::writeGpioOutput(int pin, int value) {
    // gpioWrite() or gpioPWM()
    std::cout << "[GPIO]: Writing pin " << pin << " to " << value << "%\n";
}

int GpioWrapper::readGpioInput(int pin) {
    // gpioRead()
    int value = 42;
    //std::cout << "[GPIO]: Reading pin " << pin << " value = " << value << "\n";
    return value;
}

int GpioWrapper::readGpioInput(int pin, int old_value) {
    // gpioRead()
    old_value += 5;
    if (old_value > 100)
        old_value -= 100;

    //std::cout << "[GPIO]: Reading pin " << pin << " value = " << old_value << "\n";
    return old_value;
}