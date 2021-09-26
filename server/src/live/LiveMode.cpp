#include "LiveMode.hpp"
#include "LivePin.hpp"
#include <csignal>
#include <cstdio>
#include <string>

LiveMode::LiveMode(int base_port, bool use_fade_in, bool use_fade_out)
    : input_monitor_running(true), monitor_poll_time(DEFAULT_MONITOR_POLL_TIME),
    use_fade_in(use_fade_in), use_fade_out(use_fade_out)
{
    // get client socket
    this->rpi = new GpioWrapper();
    this->input_monitor_thread = new std::thread(&LiveMode::input_monitor, this, base_port);
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

    this->input_monitor_running = false;
    this->input_monitor_thread->join();
    delete this->input_monitor_thread;
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
        new_output->setFade(this->use_fade_in, this->use_fade_out);
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

        LivePin *new_input = new LivePin(this->rpi, pin, GPIO_TYPE::INPUT, name);
        this->inputs[pin] = new_input;
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

void LiveMode::setFade(bool fade_in, bool fade_out)
{
    bool changed = false;
    if (this->use_fade_in != fade_in)
    {
        changed = true;
        this->use_fade_in = fade_in;
    }
    if (this->use_fade_out != fade_out)
    {
        changed = true;
        this->use_fade_out = fade_out;
    }

    if (changed == true)
    {
        for(std::map<int, LivePin*>::iterator it = this->outputs.begin(); it != this->outputs.end(); it++)
            it->second->setFade(this->use_fade_in, this->use_fade_out);
    }
}

void LiveMode::input_monitor(int base_port)
{
  ISocket *sock = new Socket("mycert.pem", "key.pem", OpensslWrapper::SERVER);
  ISocket *client = NULL;
  try {
    sock->bind(base_port + 1);
    sock->listen(1);

    std::cout << "[INFO]: input_monitor: Ready, Waiting client to connect...\n\n";
    while(this->input_monitor_running)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sock->getSockFd(), &rfds);
        struct timeval tv;
        if (this->monitor_poll_time > 999)
            tv.tv_sec = floor((double) this->monitor_poll_time / 1000);
        else
            tv.tv_sec = 0;
        tv.tv_usec = floor(this->monitor_poll_time % 1000) * 1000;

        int fd_ready = select(sock->getSockFd() + 1, &rfds, NULL, NULL, &tv);
        //std::cout << "select returned: " << fd_ready << "\n\n";

        if (fd_ready == -1)
            perror("select");
        else if (fd_ready > 0 && (client = sock->accept()) != NULL)
        {
            while (this->input_monitor_running)
            {
                try {
                    for(std::map<int, LivePin*>::iterator it = this->inputs.begin(); it != this->inputs.end(); it++)
                    {
                        std::string msg = "input ";
                        msg += std::to_string(it->second->getPin());
                        msg += " ";
                        msg += std::to_string(it->second->getValue());
                        client->write(msg);
                    }
                    if (this->input_monitor_running)
                        usleep(this->monitor_poll_time * 1000);
                }
                catch(char const *msg) {
                    std::cout << "[INFO]: input_monitor: client disconnected ! - " << msg << "\n";
                    break;
                }
            }
            delete client;
        }
    }
  }
  catch(char const *msg) {
    std::cerr << "Error input_monitor: " << msg << "\n";
  }
  delete sock;
}