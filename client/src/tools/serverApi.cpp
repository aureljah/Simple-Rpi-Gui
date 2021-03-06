#include "serverApi.hpp"
#include <string>

serverApi::serverApi(ISocket *server_socket)
    : server_socket(server_socket), thread_count(0), last_err_time(0)
{
    QObject::connect(this, &serverApi::sig_sendAndCheckBasicResp,
                     this, &serverApi::slot_sendAndCheckBasicResp);
}

serverApi::~serverApi()
{

}

std::vector<std::string> serverApi::splitStrToArray(std::string cmd)
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
        if (cmd[idx_start] == '"' && cmd.find("\"", idx_start + 1) != std::string::npos)
        {
            idx_start += 1;
            idx_end = cmd.find("\"", idx_start);
        }
        std::string tmp = cmd.substr(idx_start, (idx_end - idx_start));
        array.push_back(tmp);
        //std::cout << "[DEBUG]: splitStrToArray: idx_start: " << idx_start << " - idx_end: (till the end) - tmp: " << tmp << "\n";
    }

    return array;
}

bool serverApi::checkBasicRespArray(std::vector<std::string> resp_array, std::string fct_name)
{
    if (resp_array.size() < 1)
    {
        qInfo() << "Error " << QString::fromStdString(fct_name) << ": no response\n";
        return false;
    }

    if (resp_array[0] == "500")
    {
        if (resp_array.size() > 1)
        {
            qint64 err_time = QDateTime::currentMSecsSinceEpoch();
            if ((err_time - this->last_err_time) < ERROR_MSGBOX_TIMEOUT)
                return false;

            this->error_handler_lock.lock();
            this->last_err_time = err_time;
            qInfo() << QString::fromStdString(fct_name) << " Error 500: " << QString::fromStdString(resp_array[1]) << "\n";

            emit error_500(fct_name, resp_array[1]);
            qInfo() << "EMITED error_500\n";

            this->error_handler_lock.unlock();
        }
        else
            qInfo() << QString::fromStdString(fct_name) << " Error 500: <no msg>\n";
        return false;
    }
    else if (resp_array[0] == "200")
        return true;

    return false;
}

std::string serverApi::sendToServer(std::string cmd)
{
    this->socket_lock.lock();
    std::string serv_msg;

    emit send_recv_server_msg(cmd, "send");

    try {
        this->server_socket->write(cmd);
        serv_msg = this->server_socket->readLine(4096);
    }
    catch(char const *msg) {
        // emit disconnection

        qInfo() << "Error: " << msg << "\n";
        QMessageBox msgBox;

        msgBox.setText("Error: " + QString(msg));
        //msgBox.show();

        msgBox.exec();
    }

    emit send_recv_server_msg(serv_msg, "recv");

    this->socket_lock.unlock();
    return serv_msg;
}

void serverApi::slot_sendAndCheckBasicResp(std::string cmd, std::string fct_name, bool can_skip)
{
    this->thread_count_lock.lock();
    if (can_skip == true && this->thread_count >= THREAD_LIMIT)
    {
        qWarning() << "Warning: slot_sendAndCheckBasicResp: Thread limit exceeded, will skip\n";
        this->thread_count_lock.unlock();
        return;
    }
    this->thread_count += 1;
    this->thread_count_lock.unlock();

    std::thread th = std::thread(&serverApi::sendAndCheckBasicResp, this, cmd, fct_name);
    th.detach();
}

/*
 * ONLY FOR THREAD
*/
void serverApi::sendAndCheckBasicResp(std::string cmd, std::string fct_name)
{
    //qInfo() << "STARTING sendAndCheckBasicResp\n";
    std::string resp = this->sendToServer(cmd);

    std::vector<std::string> resp_array = this->splitStrToArray(resp);
    this->checkBasicRespArray(resp_array, fct_name);
    //qInfo() << "FINISHED sendAndCheckBasicResp\n";
    this->thread_count_lock.lock();
    this->thread_count -= 1;
    this->thread_count_lock.unlock();
}

void serverApi::parseLiveStatus(std::vector<std::string> resp_array, size_t start_idx)
{
    size_t i = start_idx;
    while (resp_array.size() > i)
    {
        bool input_cond = (resp_array[i] == "input" && resp_array.size() > (i + 2));
        bool output_cond = (resp_array[i] == "output" && resp_array.size() > (i + 3));
        if (input_cond || output_cond)
        {
            i++;
            int pin = std::stoi(resp_array[i]);
            i++;
            int value = 0;
            if (output_cond)
            {
                value = std::stoi(resp_array[i]);
                i++;
            }
            std::string name = resp_array[i];
            i++;

            if (input_cond)
                emit new_live_input(pin, name);
            else if (output_cond)
                emit new_live_output(pin, value, name);
        }
        else // invalid data
            break;
    }
}

void serverApi::liveParseInputReceiver(std::string cmd)
{
    emit send_recv_server_msg(cmd, "recv");
    std::vector<std::string> resp_array = this->splitStrToArray(cmd);

    if (resp_array.size() > 2 && resp_array[0] == "input")
    {
        int pin = std::stoi(resp_array[1]);
        int value = std::stoi(resp_array[2]);
        emit input_value_changed(pin, value);
    }
}

/* API */
bool serverApi::getServerStatus()
{
    std::string resp = this->sendToServer("STATUS");

    std::vector<std::string> resp_array = this->splitStrToArray(resp);
    if (this->checkBasicRespArray(resp_array, "getServerStatus") == false)
        return false;

    if (resp_array.size() >= 2)
    {
        if (resp_array[1] == "LIVE")
            this->parseLiveStatus(resp_array, 2);
        //else if (resp_array[1] == "SCRIPT") // TODO
        //else if (resp_array[1] == "NONE")
    }
    return true;
}

bool serverApi::getServerSetting()
{
    std::string resp = this->sendToServer("SETTING");

    std::vector<std::string> resp_array = this->splitStrToArray(resp);
    if (this->checkBasicRespArray(resp_array, "getServerStatus") == false)
        return false;

    if (resp_array.size() > 1 && resp_array[1] == "SETTING")
    {
        size_t i = 2;
        while (resp_array.size() > i)
        {
            if (resp_array[i] == "stay_active" && resp_array.size() > (i + 1))
            {
                i++;
                if (resp_array[i] == "true")
                    emit stay_alive_setting(true);
                else if (resp_array[i] == "false")
                    emit stay_alive_setting(false);
            }
            else if (resp_array[i] == "use_fade_in" && resp_array.size() > (i + 1))
            {
                i++;
                if (resp_array[i] == "true")
                    emit use_fade_in_setting(true);
                else if (resp_array[i] == "false")
                    emit use_fade_in_setting(false);
            }
            else if (resp_array[i] == "use_fade_out" && resp_array.size() > (i + 1))
            {
                i++;
                if (resp_array[i] == "true")
                    emit use_fade_out_setting(true);
                else if (resp_array[i] == "false")
                    emit use_fade_out_setting(false);
            }
            else // invalid
                break;
            i++;
        }
    }
    return true;
}

void serverApi::setServerSetting(std::string setting, std::string value)
{
    std::string cmd = "SETTING";
    if (setting == "stay_active" || setting == "use_fade_in" || setting == "use_fade_out")
    {
        cmd += " " + setting;
        if (value == "true" || value == "false")
            cmd += " " + value;
        else
            return;
    }
    else
        return;

    emit sig_sendAndCheckBasicResp(cmd, "setServerSetting", false);
    /*
    std::string resp = this->sendToServer(cmd);

    std::vector<std::string> resp_array = this->splitStrToArray(resp);
    this->checkBasicRespArray(resp_array, "setServerSetting");
    */
}

void serverApi::liveSetInputServer(int pin, std::string name)
{
    std::string cmd = "LIVE set input ";
    cmd += std::to_string(pin);
    cmd += " \"" + name + "\"";

    emit sig_sendAndCheckBasicResp(cmd, "liveSetInputServer", false);
    /*
    std::string resp = this->sendToServer(cmd);

    std::vector<std::string> resp_array = this->splitStrToArray(resp);
    this->checkBasicRespArray(resp_array, "liveSetInputServer");
    */
}

void serverApi::liveSetSeveralOutputServer(std::vector<int> pins, std::vector<int> values, std::vector<std::string> names)
{
    std::string cmd = "LIVE set output";
    if (pins.size() > 0 && pins.size() == values.size() && pins.size() == names.size())
    {
        for (size_t i = 0 ; i < pins.size() ; i++)
        {
            cmd += " ";
            cmd += std::to_string(pins[i]);
            cmd += " " + std::to_string(values[i]);
            cmd += " \"" + names[i] + "\"";
        }
    }
    else
    {
        qInfo() << "liveSetSeveralOutputServer: Error: size == 0 or size mismatch\n";
        qInfo() << "liveSetSeveralOutputServer: pins.size: " << pins.size() << " - values.size: " << values.size() << " - names.size: " << names.size() << "\n";
        return;
    }

    //qInfo() << "liveSetSeveralOutputServer: cmd: " << QString::fromStdString(cmd) << "\n";
    emit sig_sendAndCheckBasicResp(cmd, "liveSetOutputServer", true);
}

void serverApi::liveSetOutputServer(int pin, int value, std::string name)
{
    std::string cmd = "LIVE set output ";
    cmd += std::to_string(pin);
    cmd += " " + std::to_string(value);
    cmd += " \"" + name + "\"";

    emit sig_sendAndCheckBasicResp(cmd, "liveSetOutputServer", true);
    //qInfo() << "AFTER sig_sendAndCheckBasicResp\n";
    //th->detach();
    /*
    std::string resp = this->sendToServer(cmd);

    std::vector<std::string> resp_array = this->splitStrToArray(resp);
    this->checkBasicRespArray(resp_array, "liveSetOutputServer");
    */
}

void serverApi::liveDelPinServer(int pin)
{
    std::string cmd = "LIVE del ";
    cmd += std::to_string(pin);

    emit sig_sendAndCheckBasicResp(cmd, "liveDelPinServer", false);
    /*
    std::string resp = this->sendToServer(cmd);

    std::vector<std::string> resp_array = this->splitStrToArray(resp);
    this->checkBasicRespArray(resp_array, "liveDelPinServer");
    */
}
