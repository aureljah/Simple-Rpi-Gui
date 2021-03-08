#include "MainServer.hpp"
#include "live/LiveMode.hpp"
#include <cstddef>
#include <exception>
#include <string>

MainServer::MainServer()
    : live_mode(nullptr), setting_stay_active(true)
{

}

MainServer::~MainServer()
{
    
}

std::vector<std::string> MainServer::splitStrToArray(std::string cmd)
{
    std::vector<std::string> array;
    
    size_t idx_start = 0;
    size_t idx_end = cmd.find(" ");
    while (idx_end != std::string::npos && idx_start < cmd.size())
    {
        if (cmd[idx_start] == '"' && cmd.find("\"", idx_start + 1) != std::string::npos)
        {
            idx_start += 1;
            idx_end = cmd.find("\"", idx_start);
        }

        if (cmd[idx_start] != ' ')
        {
            std::string tmp = cmd.substr(idx_start, (idx_end - idx_start));
            array.push_back(tmp);
            //std::cout << "[DEBUG]: splitStrToArray: idx_start: " << idx_start << " - idx_end: " << idx_end << " - tmp: " << tmp << "\n";
        }

        idx_start = idx_end + 1;
        idx_end = cmd.find(" ", idx_start);
    }

    if (idx_start < cmd.size())
    {
        std::string tmp = cmd.substr(idx_start);
        array.push_back(tmp);
        //std::cout << "[DEBUG]: splitStrToArray: idx_start: " << idx_start << " - idx_end: (till the end) - tmp: " << tmp << "\n";
    }

    return array;
}

std::string MainServer::parseCommand(std::string cmd)
{
    std::vector<std::string> cmd_array = this->splitStrToArray(cmd);

    size_t i = 0;
    if (cmd_array.size() <= i)
        throw std::runtime_error("Command <> not found");

    if (cmd_array[i] == "STATUS")
    {
        i++;
        return this->getStatusCommand();
    }
    else if (cmd_array[i] == "SETTING")
    {
        i++;
        if (cmd_array.size() <= i)
            return this->getSettingCommand();
        else if (cmd_array.size() > i + 1)
            return this->setSettingCommand(cmd_array[i], cmd_array[i + 1]);
        else
            throw std::runtime_error("SETTING: missing parameters");
    }
    else if (cmd_array[i] == "LIVE")
    {
        i++;
        if (cmd_array.size() <= i)
            throw std::runtime_error("LIVE: Command <> not found");

        if (cmd_array[i] == "set")
        {
            i++;
            if (cmd_array.size() <= i)
                throw std::runtime_error("LIVE: set: Command <> not found");

            if (cmd_array[i] == "input")
            {
                if (cmd_array.size() > i + 2)
                    return this->liveSetInputCommand(cmd_array[i + 1], cmd_array[i + 2]);
                else
                    throw std::runtime_error("LIVE set input: missing parameters");
            }
            else if (cmd_array[i] == "output")
            {
                if (cmd_array.size() > i + 3)
                    return this->liveSetOutputCommand(cmd_array[i + 1], cmd_array[i + 2], cmd_array[i + 3]);
                else
                    throw std::runtime_error("LIVE set output: missing parameters");
            }
            else
                throw std::runtime_error("LIVE: set: Command <" + cmd_array[i] + "> not found");
        }
        else if (cmd_array[i] == "del")
        {
            if (cmd_array.size() > i + 1)
                return this->liveDelPinCommand(cmd_array[i + 1]);
            else
                throw std::runtime_error("LIVE del: missing parameter pin");
        }
        else
            throw std::runtime_error("LIVE: Command <" + cmd_array[i] + "> not found");
    }
    else
        throw std::runtime_error("Command <" + cmd_array[i] + "> not found");

    return "TODO";
}

std::string MainServer::getStatusCommand()
{
    std::string status = "NONE";
    if (this->live_mode)
    {
        status = "LIVE";
        status += this->live_mode->getStatus();
    }
    // else if script mode

    return status;
}

std::string MainServer::getSettingCommand()
{
    std::string settings = "SETTING";

    settings += " stay_active ";
    settings += this->boolToString(this->setting_stay_active);

    return settings;
}

std::string MainServer::setSettingCommand(std::string setting, std::string value)
{
    std::string ret;
    if (setting == "stay_active")
    {
        if (this->isBoolStringValid(value) == true)
            this->setting_stay_active = this->stringToBool(value);
        else
            throw std::runtime_error("SETTING stay_active: invalid value");
    }
    else
        throw std::runtime_error("SETTING: setting <" + setting + "> doesn't exist");

    return ""; // OK
}

std::string MainServer::liveSetInputCommand(std::string cmd_pin, std::string cmd_name)
{
    int pin = std::stoi(cmd_pin);
    if (this->isPinNumberValid(pin) == false)
        throw std::runtime_error("LIVE set input: invalid pin number");

    if (this->isNameValid(cmd_name) == false)
        throw std::runtime_error("LIVE set input: invalid name");

    this->useModeLive();
    return this->live_mode->setInput(pin, cmd_name);
}

std::string MainServer::liveSetOutputCommand(std::string cmd_pin, std::string cmd_value, std::string cmd_name)
{
    int pin = std::stoi(cmd_pin);
    if (this->isPinNumberValid(pin) == false)
        throw std::runtime_error("LIVE set output: invalid pin number");

    int value = std::stoi(cmd_value);
    if (this->isValueValid(value) == false)
        throw std::runtime_error("LIVE set output: invalid value number");

    if (this->isNameValid(cmd_name) == false)
        throw std::runtime_error("LIVE set output: invalid name");

    this->useModeLive();
    return this->live_mode->setOutput(pin, value, cmd_name);
}

std::string MainServer::liveDelPinCommand(std::string cmd_pin)
{
    int pin = std::stoi(cmd_pin);
    if (this->isPinNumberValid(pin) == false)
        throw std::runtime_error("LIVE del: invalid pin number");

    if (this->live_mode == nullptr)
        throw std::runtime_error("LIVE del: Live mode is not active");

    return this->live_mode->delPin(pin);
}

bool MainServer::useModeLive()
{
    // del mode script if active
    if (this->live_mode == nullptr)
        this->live_mode = new LiveMode(this->base_port);

    return true;
}

bool MainServer::deleteAllMode()
{
    // del mode script
    if (this->live_mode)
    {
        delete this->live_mode;
        this->live_mode = nullptr;
    }

    return true;
}

std::string MainServer::boolToString(bool var)
{
    std::string str = "false";
    if (var == true)
        str = "true";

    return str;
}

bool MainServer::stringToBool(std::string str)
{
    bool var = false;
    if (str == "true")
        var = true;

    return var;
}

bool MainServer::isPinNumberValid(int pin)
{
    if (pin < 2 || pin > 26)
        return false;

    return true;
}

bool MainServer::isValueValid(int value)
{
    if (value < 0 || value > 100)
        return false;

    return true;
}

bool MainServer::isNameValid(std::string name)
{
    if (name == "" || name.length() > 32)
        return false;

    return true;
}

bool MainServer::isBoolStringValid(std::string str_bool)
{
    if (str_bool == "true" || str_bool == "false")
        return true;

    return false;
}

void MainServer::onDisconnected()
{
    if (this->setting_stay_active == false)
        this->deleteAllMode();
}

void MainServer::run(int port, std::string cert_path)
{
    ISocket *sock = new Socket(cert_path, OpensslWrapper::SERVER);
    ISocket *client = NULL;
    try {
        sock->bind(port);
        sock->listen(1);
        this->base_port = port;

        while(true)
        {
            std::cout << "INFO: Ready, Waiting client to connect...\n\n";
            if ((client = sock->accept()) != NULL)
            {
                while (true)
                {
                    try {
                        std::string msg = client->readLine(4096);

                        std::cout << "[SERVER] client: " << msg << std::endl;
                        std::string resp = "";
                        try {
                            resp = "200 " + this->parseCommand(msg);
                        }
                        catch(std::exception &ex) {
                            std::cout << "[WARNING]: will respond error 500: " << ex.what() << "\n";
                            resp = "500 " + std::string(ex.what());
                        }

                        client->write(resp);
                    }
                    catch(char const *msg) {
                        std::cout << "INFO: client disconnected ! - " << msg << "\n";
                        break;
                    }
                }
                this->onDisconnected();
                delete client;
            }
        }
    }
    catch(char const *msg) {
        std::cerr << "Error: " << msg << "\n";
    }
    delete sock;
}