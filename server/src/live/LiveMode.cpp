#include "LiveMode.hpp"
#include "LivePin.hpp"
#include <string>

LiveMode::LiveMode()
{
    // get client socket
    this->rpi = new GpioWrapper();
}

LiveMode::~LiveMode()
{
    // del all inputs
    for(std::map<int, LivePin*>::iterator it = this->inputs.begin(); it != this->inputs.end(); it++)
    {
        LivePin *tmp = it->second;
        delete tmp;
    }
    this->inputs.clear();
    
    // del all outputs
    for(std::map<int, LivePin*>::iterator it = this->outputs.begin(); it != this->outputs.end(); it++)
    {
        LivePin *tmp = it->second;
        delete tmp;
    }
    this->outputs.clear();

    delete this->rpi;
}

std::string LiveMode::getStatus()
{
    std::string status;

    for(std::map<int, LivePin*>::iterator it = this->inputs.begin(); it != this->inputs.end(); it++)
    {
        LivePin *tmp = it->second;
        status += " input ";
        status += std::to_string(tmp->getPin());
        status += " \"" + tmp->getName() + "\"";
    }

    for(std::map<int, LivePin*>::iterator it = this->outputs.begin(); it != this->outputs.end(); it++)
    {
        LivePin *tmp = it->second;
        status += " output ";
        status += std::to_string(tmp->getPin());
        status += " " + std::to_string(tmp->getValue());
        status += " \"" + tmp->getName() + "\"";
    }

    return status;
}

std::string LiveMode::setOutput(int pin, int value, std::string name)
{
    if (this->outputs.find(pin) != this->outputs.end())
    {
        this->outputs[pin]->setName(name);
        this->outputs[pin]->setValue(value);
    }
    else
    {
        //this->delPin(pin); // in case one input have same pin
        if (this->inputs.find(pin) != this->inputs.end())
            throw std::runtime_error("Pin already exists as input");

        LivePin *new_output = new LivePin(this->rpi, pin, GPIO_TYPE::OUTPUT, name, value);
        this->outputs[pin] = new_output;
    }
    return ""; // OK
}

std::string LiveMode::setInput(int pin, std::string name)
{
    if (this->inputs.find(pin) != this->inputs.end())
    {
        this->inputs[pin]->setName(name);
    }
    else
    {
        //this->delPin(pin); // in case one output have same pin
        if (this->outputs.find(pin) != this->outputs.end())
            throw std::runtime_error("Pin already exists as output");

        LivePin *new_output = new LivePin(this->rpi, pin, GPIO_TYPE::INPUT, name);
        this->inputs[pin] = new_output;
    }
    return ""; // OK
}

std::string LiveMode::delPin(int pin)
{
    if (this->inputs.find(pin) != this->inputs.end())
    {
        LivePin *old_pin = this->inputs.at(pin);
        this->inputs.erase(pin);
        delete old_pin;
    }
    if (this->outputs.find(pin) != this->outputs.end())
    {
        LivePin *old_pin = this->outputs.at(pin);
        this->outputs.erase(pin);
        delete old_pin;
    }
    return ""; // OK
}