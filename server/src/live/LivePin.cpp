#include "LivePin.hpp"


LivePin::LivePin(IGpioWrapper *rpi, int pin, GPIO_TYPE::gpio_type type, std::string name, int value) 
    : rpi(rpi), pin(pin), type(type), name(name), value(value), real_value(value),
    fade_thread_active(false), use_fade_in(false), use_fade_out(false)
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
        this->set_lock.lock();
        this->value = 0;
        this->real_value = 0;
        this->rpi->writeGpioOutput(this->pin, 0);
        this->set_lock.unlock();
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
    this->set_lock.lock();
    this->name = name;
    this->set_lock.unlock();
    return true;
}

bool LivePin::setValue(int value) {
    if (this->type != GPIO_TYPE::OUTPUT || value < 0 || value > 100) {
        return false;
    }
    this->set_lock.lock();

    this->value = value;

    if (this->use_fade_in == true || this->use_fade_out == true)
    {
        if (this->fade_thread_active == false)
        {
            this->fade_thread_active = true;
            std::thread fade_thread = std::thread(&LivePin::doFading, this);
            fade_thread.detach();
        }
    }
    else
    {
        this->real_value = value;
        this->rpi->writeGpioOutput(this->pin, value);
    }

    this->set_lock.unlock();
    return true;
}

void LivePin::setFade(bool fade_in, bool fade_out)
{
    this->use_fade_in = fade_in;
    this->use_fade_out = fade_out;
}

void LivePin::doFading()
{
    //std::cout << "doFading STARTED !\n";
    while (true)
    {
        this->set_lock.lock();
        if (this->real_value == this->value)
            break;

        if (this->real_value < this->value) // fade in
        {
            if (this->use_fade_in == true)
                this->real_value += 1;
            else
                this->real_value = this->value;
        }
        else if (this->real_value > this->value) // fade out
        {
            if (this->use_fade_out == true)
                this->real_value -= 1;
            else
                this->real_value = this->value;
        }
        else // real_value == value
            break;

        this->rpi->writeGpioOutput(this->pin, this->real_value);

        this->set_lock.unlock();
        usleep(FADE_RATE_MS * 1000);
    }
    this->fade_thread_active = false;
    //std::cout << "doFading FINISHED !\n";
    this->set_lock.unlock();
}
