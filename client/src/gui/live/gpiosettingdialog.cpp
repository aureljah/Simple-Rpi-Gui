#include "gpiosettingdialog.h"
#include "ui_gpiosettingdialog.h"

gpioSettingDialog::gpioSettingDialog(std::list<QString> used_name, std::list<int> used_pins, gpio_type type, QString old_name, int old_pin, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gpioSettingDialog), used_name(used_name), type(type),
    old_name(old_name), old_pin(old_pin)
{
    ui->setupUi(this);

    if (type == gpio_type::INPUT)
    {
        this->setWindowTitle("Input setting");
    }
    else if (type == gpio_type::OUTPUT)
    {
        this->setWindowTitle("Output setting");
    }

    ui->comboBox->clear();
    std::list<int> pins = Tools::getGPIONumberList();
    for(std::list<int>::iterator it = pins.begin(); it != pins.end(); it++)
    {
        if (std::find(used_pins.begin(), used_pins.end(), *it) == used_pins.end())
        {
            ui->comboBox->addItem(QString::number(*it));
        }
    }

    if (old_name != nullptr)
    {
        ui->lineEdit->setText(old_name);

        if (old_pin != -1)
        {
            int idx = ui->comboBox->findText(QString::number(old_pin));
            if (idx != -1)
            {
                ui->comboBox->setCurrentIndex(idx);
            }
        }
    }
    else // set a valid default name (Output 1 / Output 2...)
    {
        for (int num = 1 ; num < 40 ; num++)
        {
            QString default_name = "Output " + QString::number(num);
            if (type == gpio_type::INPUT)
                default_name = "Input " + QString::number(num);

            if (std::find(this->used_name.begin(), this->used_name.end(), default_name) == this->used_name.end())
            {
                ui->lineEdit->setText(default_name);
                break;
            }
        }

    }

    ui->lineEdit->setFocus();
}

gpioSettingDialog::~gpioSettingDialog()
{
    delete ui;
}

bool gpioSettingDialog::checkInput()
{
    QString name = ui->lineEdit->text();
    if (name.length() < 1)
    {
        Tools::errorDialog("This name is not valid");
        return false;
    }
    else if (std::find(this->used_name.begin(), this->used_name.end(), name) != this->used_name.end())
    {
        Tools::errorDialog("This name is already used");
        return false;
    }

    return true;
}

void gpioSettingDialog::on_buttonBox_accepted()
{
    if (this->checkInput() == true)
    {
        emit dialog_accepted(ui->lineEdit->text(), ui->comboBox->currentText().toInt(),
                                 this->type, this->old_name, this->old_pin, 0);
        this->close();
    }
}

void gpioSettingDialog::on_buttonBox_rejected()
{
    this->close();
}
