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
    if (this->last_read_value == 0) {
        this->last_read_value = 1;
    }
    else {
        this->last_read_value = 0;
    }
    int value = this->last_read_value;

    std::cout << "[GPIO]: Reading pin " << pin << " value = " << value << "\n";
    return value;
}