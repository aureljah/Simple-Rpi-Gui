#include "modelive.h"

modeLive::modeLive(QWidget *live_tab, serverApi *server_api)
    : live_tab(live_tab), server_api(server_api)
{
    QWidget *widget = new QWidget();

    QScrollArea *output_scrollArea = live_tab->findChild<QScrollArea*>("live_output_scrollArea", Qt::FindChildrenRecursively);
    if (output_scrollArea != nullptr)
    {
        //ui->live_output_scrollArea->setWidget( widget );
        output_scrollArea->setWidget( widget );
        this->vlayout = new QVBoxLayout();
        widget->setLayout(this->vlayout);
        output_scrollArea->setWidgetResizable(true);
        //ui->live_output_scrollArea->setWidgetResizable(true);
    }
    else
    {
        std::cerr << "Error in modeLive(ctor): live_output_scrollArea not found\n";
        delete widget;
    }
}

void modeLive::startGpioSettingDialog(gpioSettingDialog::gpio_type type, QString old_name)
{
    int old_pin = -1;
    if (old_name != nullptr)
    {
        old_pin = this->dyn_widgets[old_name]->getPin();
    }
    auto win = new gpioSettingDialog(this->getUsedName(old_name), this->getUsedPins(old_pin), type, old_name, old_pin, this->live_tab);

    QObject::connect(win, &gpioSettingDialog::dialog_accepted,
                     this, &modeLive::addInputOutput);

    win->setAttribute(Qt::WA_DeleteOnClose);
    win->show();
}

void modeLive::addInputOutput(QString name, int pin, gpioSettingDialog::gpio_type type, QString old_name, int old_pin)
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
            dyn_out = this->dyn_widgets[old_name];
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
                this->dyn_widgets.erase(old_name);
                this->dyn_widgets[name] = dyn_out;
            }
        }
        else // add/create new output
        {
            dyn_out = new dynamicOutput(server_api, pin, name);
            QObject::connect(dyn_out, &dynamicOutput::live_output_editButton_clicked,
                             this, &modeLive::live_output_editButton_clicked);
            QObject::connect(dyn_out, &dynamicOutput::live_output_delButton_clicked,
                             this, &modeLive::live_output_delButton_clicked);
            QObject::connect(dyn_out, &dynamicOutput::live_output_upButton_clicked,
                             this, &modeLive::live_output_upButton_clicked);
            QObject::connect(dyn_out, &dynamicOutput::live_output_downButton_clicked,
                             this, &modeLive::live_output_downButton_clicked);

            this->vlayout->addWidget(dyn_out);
            this->dyn_widgets[name] = dyn_out;
        }
    }
    else if (type == gpioSettingDialog::INPUT)
    {
        // TODO
    }
}

std::list<QString> modeLive::getUsedName(QString name_ignored)
{
    std::list<QString> used_name;

    for(std::map<QString, dynamicOutput*>::iterator it = this->dyn_widgets.begin(); it != this->dyn_widgets.end(); it++)
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

    for(std::map<QString, dynamicOutput*>::iterator it = this->dyn_widgets.begin(); it != this->dyn_widgets.end(); it++)
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
    dynamicOutput *item = this->dyn_widgets[name];
    int idx = this->vlayout->indexOf(item);
    if (idx <= 0)
    {
        //qInfo() << "live_output_upButton_clicked: idx: " + QString::number(idx) + " => will do nothing.\n";
        return;
    }
    //qInfo() << "live_output_upButton_clicked: idx: " + QString::number(idx) + " \n";

    this->vlayout->removeWidget(item);
    this->vlayout->insertWidget(idx - 1, item);
}

void modeLive::live_output_downButton_clicked(QString name)
{
    dynamicOutput *item = this->dyn_widgets[name];
    int idx = this->vlayout->indexOf(item);
    if (idx < 0 && idx >= this->vlayout->count() - 1)
    {
        //qInfo() << "live_output_downButton_clicked: idx: " + QString::number(idx) + " => will do nothing.\n";
        return;
    }
    //qInfo() << "live_output_downButton_clicked: idx: " + QString::number(idx) + " \n";

    this->vlayout->removeWidget(item);
    this->vlayout->insertWidget(idx + 1, item);
}

void modeLive::live_output_editButton_clicked(QString name)
{
    this->startGpioSettingDialog(gpioSettingDialog::OUTPUT, name);
}

void modeLive::live_output_delButton_clicked(QString name)
{
    dynamicOutput *item = this->dyn_widgets[name];
    item->deleteLater();
    for(std::map<QString, dynamicOutput*>::iterator it = this->dyn_widgets.begin(); it != this->dyn_widgets.end(); it++)
    {
        if((it->first) == name)
        {
            this->dyn_widgets.erase(it);
            break;
        }
    }
}
