#include "modelive.h"

modeLive::modeLive(QWidget *live_tab, serverApi *server_api)
    : live_tab(live_tab), server_api(server_api), input_thread(nullptr), input_thread_running(false)
{
    QWidget *out_widget = new QWidget();
    QScrollArea *output_scrollArea = live_tab->findChild<QScrollArea*>("live_output_scrollArea", Qt::FindChildrenRecursively);
    if (output_scrollArea != nullptr)
    {
        //ui->live_output_scrollArea->setWidget( widget );
        output_scrollArea->setWidget( out_widget );
        this->out_vlayout = new QVBoxLayout();
        out_widget->setLayout(this->out_vlayout);
        output_scrollArea->setWidgetResizable(true);
        //ui->live_output_scrollArea->setWidgetResizable(true);
    }
    else
    {
        std::cerr << "Error in modeLive(ctor): live_output_scrollArea not found\n";
        delete out_widget;
    }

    QWidget *in_widget = new QWidget();
    QScrollArea *input_scrollArea = live_tab->findChild<QScrollArea*>("live_input_scrollArea", Qt::FindChildrenRecursively);
    if (input_scrollArea != nullptr)
    {
        input_scrollArea->setWidget( in_widget );
        this->in_vlayout = new QVBoxLayout();
        in_widget->setLayout(this->in_vlayout);
        input_scrollArea->setWidgetResizable(true);
    }
    else
    {
        std::cerr << "Error in modeLive(ctor): live_input_scrollArea not found\n";
        delete in_widget;
    }
}

modeLive::~modeLive()
{
    this->input_thread_running = false;

    // TODO
}

void modeLive::server_input_receiver()
{
    bool will_exit = false;
    std::thread *this_thread = this->input_thread;

    ISocket *input_sock = new Socket("mycert.pem", OpensslWrapper::CLIENT);
    while (will_exit == false) {
        try {
            input_sock->connect(this->server_api->getServerIp(), this->server_api->getInputReceiverPort());
            qInfo() << "server_input_receiver: CONNECTED\n";

            while (will_exit == false) {
                try {
                    //std::string data = input_sock->readLine(4096);
                    //this->server_api->liveParseInputReceiver(data);
                    qInfo() << "server_input_receiver: RUNNING/READING (read disabled)\n";
                    systemcall::sys_usleep(INPUT_POLL_TIME);
                }
                catch(char const *msg) {
                    qInfo() << "Error read server_input_receiver: " << msg << "\n";
                }

                this->input_thread_mutex.lock();
                if (this->input_thread_running == false) {
                    //qInfo() << "server_input_receiver: WILL EXIT(reading loop)\n";
                    this->input_thread = nullptr;
                    will_exit = true;
                }
                this->input_thread_mutex.unlock();
            }
            qInfo() << "server_input_receiver: STOPPED READING\n";
            break;
        }
        catch(char const *msg) {
            qInfo() << "Error connect server_input_receiver: " << msg << "\n";
        }

        this->input_thread_mutex.lock();
        if (this->input_thread_running) {
            this->input_thread_mutex.unlock();
            qInfo() << "server_input_receiver: WILL TRY CONNECT AGAIN IN " << INPUT_POLL_TIME << "millisec\n";
            systemcall::sys_usleep(INPUT_POLL_TIME);
        }
        else {
            //qInfo() << "server_input_receiver: WILL EXIT(connect loop)\n";
            this->input_thread = nullptr;
            will_exit = true;
            this->input_thread_mutex.unlock();
        }
    }
    //qInfo() << "server_input_receiver: IS FINISHED\n";
    delete input_sock;
    delete this_thread;
}

void modeLive::startGpioSettingDialog(gpioSettingDialog::gpio_type type, QString old_name)
{
    int old_pin = -1;
    if (old_name != nullptr)
    {
        old_pin = this->dyn_o_widgets[old_name]->getPin();
    }
    auto win = new gpioSettingDialog(this->getUsedName(old_name), this->getUsedPins(old_pin), type, old_name, old_pin, this->live_tab);

    QObject::connect(win, &gpioSettingDialog::dialog_accepted,
                     this, &modeLive::addInputOutput);

    win->setAttribute(Qt::WA_DeleteOnClose);
    win->show();
}

void modeLive::addInputOutput(QString name, int pin, gpioSettingDialog::gpio_type type, QString old_name, int old_pin, int value)
{
    std::list<QString> used_name = this->getUsedName(old_name);
    if (std::find(used_name.begin(), used_name.end(), name) != used_name.end())
    {
        Tools::errorDialog("This name is already used");
        return;
    }
    std::list<int> used_pin = this->getUsedPins(old_pin);
    if (std::find(used_pin.begin(), used_pin.end(), pin) != used_pin.end())
    {
        Tools::errorDialog("This pin number is already used");
        return;
    }

    if (type == gpioSettingDialog::OUTPUT)
    {
        dynamicOutput *dyn_out;
        if (old_name != nullptr) // edit an existing output
        {
            dyn_out = this->dyn_o_widgets[old_name];
            if (dyn_out == nullptr)
            {
                qInfo() << "addInputOutput Error: Output not found with name=" + old_name + ".\n";
                return;
            }

            if (dyn_out->getPin() != pin)
            {
                dyn_out->setPin(pin);
            }
            if (dyn_out->getName() != name)
            {
                dyn_out->setName(name);
                this->dyn_o_widgets.erase(old_name);
                this->dyn_o_widgets[name] = dyn_out;
            }
        }
        else // add/create new output
        {
            dyn_out = new dynamicOutput(server_api, pin, name, value);
            QObject::connect(dyn_out, &dynamicOutput::live_output_editButton_clicked,
                             this, &modeLive::live_output_editButton_clicked);
            QObject::connect(dyn_out, &dynamicOutput::live_output_delButton_clicked,
                             this, &modeLive::live_output_delButton_clicked);
            QObject::connect(dyn_out, &dynamicOutput::live_output_upButton_clicked,
                             this, &modeLive::live_output_upButton_clicked);
            QObject::connect(dyn_out, &dynamicOutput::live_output_downButton_clicked,
                             this, &modeLive::live_output_downButton_clicked);

            this->out_vlayout->addWidget(dyn_out);
            this->dyn_o_widgets[name] = dyn_out;
        }
    }
    else if (type == gpioSettingDialog::INPUT)
    {
        dynamicInput *dyn_in;
        if (old_name != nullptr) // edit an existing output
        {
            dyn_in = this->dyn_i_widgets[old_name];
            if (dyn_in == nullptr)
            {
                qInfo() << "addInputOutput Error: Input not found with name=" + old_name + ".\n";
                return;
            }

            if (dyn_in->getPin() != pin)
            {
                dyn_in->setPin(pin);
            }
            if (dyn_in->getName() != name)
            {
                dyn_in->setName(name);
                this->dyn_i_widgets.erase(old_name);
                this->dyn_i_widgets[name] = dyn_in;
            }
        }
        else // add/create new output
        {
            dyn_in = new dynamicInput(server_api, pin, name);
            QObject::connect(dyn_in, &dynamicInput::live_input_editButton_clicked,
                             this, &modeLive::live_input_editButton_clicked);
            QObject::connect(dyn_in, &dynamicInput::live_input_delButton_clicked,
                             this, &modeLive::live_input_delButton_clicked);
            QObject::connect(dyn_in, &dynamicInput::live_input_upButton_clicked,
                             this, &modeLive::live_input_upButton_clicked);
            QObject::connect(dyn_in, &dynamicInput::live_input_downButton_clicked,
                             this, &modeLive::live_input_downButton_clicked);

            this->in_vlayout->addWidget(dyn_in);
            this->dyn_i_widgets[name] = dyn_in;

            // start a 2nd thread&socket to receive every input value update from server
            this->input_thread_mutex.lock();
            this->input_thread_running = true;
            if (this->input_thread == nullptr) {
                this->input_thread = new std::thread(&modeLive::server_input_receiver, this);
                this->input_thread->detach();
            }
            this->input_thread_mutex.unlock();
        }
    }
}

std::list<QString> modeLive::getUsedName(QString name_ignored)
{
    std::list<QString> used_name;

    for(std::map<QString, dynamicOutput*>::iterator it = this->dyn_o_widgets.begin(); it != this->dyn_o_widgets.end(); it++)
    {
        if (name_ignored != nullptr && it->second->getName() == name_ignored)
        {
            continue;
        }
        used_name.push_back(it->second->getName());
    }
    for(std::map<QString, dynamicInput*>::iterator it = this->dyn_i_widgets.begin(); it != this->dyn_i_widgets.end(); it++)
    {
        if (name_ignored != nullptr && it->second->getName() == name_ignored)
        {
            continue;
        }
        used_name.push_back(it->second->getName());
    }
    return used_name;
}

std::list<int> modeLive::getUsedPins(int pin_ignored)
{
    std::list<int> used_pins;

    for(std::map<QString, dynamicOutput*>::iterator it = this->dyn_o_widgets.begin(); it != this->dyn_o_widgets.end(); it++)
    {
        if (pin_ignored != -1 && it->second->getPin() == pin_ignored)
        {
            continue;
        }
        used_pins.push_back(it->second->getPin());
    }
    for(std::map<QString, dynamicInput*>::iterator it = this->dyn_i_widgets.begin(); it != this->dyn_i_widgets.end(); it++)
    {
        if (pin_ignored != -1 && it->second->getPin() == pin_ignored)
        {
            continue;
        }
        used_pins.push_back(it->second->getPin());
    }
    return used_pins;
}

void modeLive::live_output_upButton_clicked(QString name)
{
    dynamicOutput *item = this->dyn_o_widgets[name];
    int idx = this->out_vlayout->indexOf(item);
    if (idx <= 0)
    {
        //qInfo() << "live_output_upButton_clicked: idx: " + QString::number(idx) + " => will do nothing.\n";
        return;
    }
    //qInfo() << "live_output_upButton_clicked: idx: " + QString::number(idx) + " \n";

    this->out_vlayout->removeWidget(item);
    this->out_vlayout->insertWidget(idx - 1, item);
}

void modeLive::live_output_downButton_clicked(QString name)
{
    dynamicOutput *item = this->dyn_o_widgets[name];
    int idx = this->out_vlayout->indexOf(item);
    if (idx < 0 && idx >= this->out_vlayout->count() - 1)
    {
        //qInfo() << "live_output_downButton_clicked: idx: " + QString::number(idx) + " => will do nothing.\n";
        return;
    }
    //qInfo() << "live_output_downButton_clicked: idx: " + QString::number(idx) + " \n";

    this->out_vlayout->removeWidget(item);
    this->out_vlayout->insertWidget(idx + 1, item);
}

void modeLive::live_output_editButton_clicked(QString name)
{
    this->startGpioSettingDialog(gpioSettingDialog::OUTPUT, name);
}

void modeLive::live_output_delButton_clicked(QString name)
{
    dynamicOutput *item = this->dyn_o_widgets[name];
    item->deleteLater();
    for(std::map<QString, dynamicOutput*>::iterator it = this->dyn_o_widgets.begin(); it != this->dyn_o_widgets.end(); it++)
    {
        if((it->first) == name)
        {
            this->dyn_o_widgets.erase(it);
            break;
        }
    }
}

void modeLive::live_input_upButton_clicked(QString name)
{
    dynamicInput *item = this->dyn_i_widgets[name];
    int idx = this->in_vlayout->indexOf(item);
    if (idx <= 0)
    {
        //qInfo() << "live_input_upButton_clicked: idx: " + QString::number(idx) + " => will do nothing.\n";
        return;
    }
    //qInfo() << "live_input_upButton_clicked: idx: " + QString::number(idx) + " \n";

    this->in_vlayout->removeWidget(item);
    this->in_vlayout->insertWidget(idx - 1, item);
}

void modeLive::live_input_downButton_clicked(QString name)
{
    dynamicInput *item = this->dyn_i_widgets[name];
    int idx = this->in_vlayout->indexOf(item);
    if (idx < 0 && idx >= this->in_vlayout->count() - 1)
    {
        //qInfo() << "live_input_downButton_clicked: idx: " + QString::number(idx) + " => will do nothing.\n";
        return;
    }
    //qInfo() << "live_input_downButton_clicked: idx: " + QString::number(idx) + " \n";

    this->in_vlayout->removeWidget(item);
    this->in_vlayout->insertWidget(idx + 1, item);
}

void modeLive::live_input_editButton_clicked(QString name)
{
    this->startGpioSettingDialog(gpioSettingDialog::INPUT, name);
}

void modeLive::live_input_delButton_clicked(QString name)
{
    dynamicInput *item = this->dyn_i_widgets[name];
    item->deleteLater();
    for(std::map<QString, dynamicInput*>::iterator it = this->dyn_i_widgets.begin(); it != this->dyn_i_widgets.end(); it++)
    {
        if((it->first) == name)
        {
            this->dyn_i_widgets.erase(it);
            break;
        }
    }

    this->input_thread_mutex.lock();
    if (this->dyn_i_widgets.size() < 1) {
        this->input_thread_running = false;
    }
    this->input_thread_mutex.unlock();
}

void modeLive::add_input(int pin, std::string name)
{
    this->addInputOutput(QString::fromStdString(name), pin, gpioSettingDialog::INPUT, nullptr, -1);
}
void modeLive::add_output(int pin, int value, std::string name)
{
    this->addInputOutput(QString::fromStdString(name), pin, gpioSettingDialog::OUTPUT, nullptr, -1, value);
}
void modeLive::update_input_value(int pin, int value)
{
    for(std::map<QString, dynamicInput*>::iterator it = this->dyn_i_widgets.begin(); it != this->dyn_i_widgets.end(); it++)
    {
        if((it->second->getPin()) == pin)
        {
            it->second->updateValue(value);
            break;
        }
    }
}
