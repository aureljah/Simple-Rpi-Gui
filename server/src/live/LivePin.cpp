#include "LivePin.hpp"


LivePin::LivePin(IGpioWrapper *rpi, int pin, GPIO_TYPE::gpio_type type, std::string name, int value) 
    : rpi(rpi), pin(pin), type(type), name(name), value(value)
{
    if (this->type == GPIO_TYPE::INPUT) {
        this->rpi->setGpioModeInput(this->pin);
    }
    else if (this->type == GPIO_TYPE::OUTPUT) {
        this->rpi->setGpioModeOutput(this->pin);
    }
}

LivePin::~LivePin() {
    if (this->type == GPIO_TYPE::OUTPUT) {
        this->rpi->writeGpioOutput(this->pin, 0);
    }
}

int LivePin::getPin() {
    return this->pin;
}

GPIO_TYPE::gpio_type LivePin::getType() {
    return this->type;
}

std::string LivePin::getName() {
    return this->name;
}

int LivePin::getValue() {
    if (this->type == GPIO_TYPE::INPUT) {
        this->value = this->rpi->readGpioInput(this->pin, this->value);
    }
    return this->value;
}

bool LivePin::setName(std::string name) {
    this->name = name;
    return true;
}

bool LivePin::setValue(int value) {
    if (this->type != GPIO_TYPE::OUTPUT || value < 0 || value > 100) {
        return false;
    }

    this->rpi->writeGpioOutput(this->pin, value);
    this->value = value;

    return true;
}
