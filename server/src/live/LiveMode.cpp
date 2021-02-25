#include "LiveMode.hpp"
#include "LivePin.hpp"

LiveMode::LiveMode()
{
    // get client socket
    this->rpi = new GpioWrapper();
}

LiveMode::~LiveMode()
{
    // delete everything
    
    delete this->rpi;
    
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
        this->delPin(pin); // in case input have same pin
                            // => OR maybe not del but return error "pin already exists in input" ?

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
        this->delPin(pin); // in case output have same pin
                            // => OR maybe not del but return error "pin already exists in output" ?

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