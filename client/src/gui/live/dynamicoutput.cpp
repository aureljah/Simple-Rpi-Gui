#include "dynamicoutput.h"
#include "ui_dynamicoutput.h"

dynamicOutput::dynamicOutput(serverApi *server_api, int pin, QString name, int value, QWidget *parent) :
    QWidget(parent), server_api(server_api), pin(pin), name(name), ui(new Ui::dynamicOutput), value(0)
{
    qInfo() << "New dynamicOutput pin: " << pin << "\n";
    ui->setupUi(this);
    ui->downButton->setIcon(ui->delButton->style()->standardIcon(QStyle::SP_ArrowDown));
    ui->upButton->setIcon(ui->delButton->style()->standardIcon(QStyle::SP_ArrowUp));
    //ui->delButton->setIcon(ui->delButton->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    ui->delButton->setIcon(ui->delButton->style()->standardIcon(QStyle::SP_TrashIcon));
    this->updateTitle();
    this->setObjectName(name);

    ui->pwm_spinBox->setRange(0, 100);
    ui->pwm_spinBox->setSingleStep(1);
    ui->pwm_slider->setRange(0, 100);

    this->updatePinValue(value);
}

dynamicOutput::~dynamicOutput()
{
    this->server_api->liveDelPinServer(this->pin);
    delete ui;
}

void dynamicOutput::updateTitle()
{
    ui->groupBox->setTitle(this->name + QString(" - GPIO ") + QString::number(this->pin));
}

int dynamicOutput::getPin()
{
    return this->pin;
}
void dynamicOutput::setPin(int pin)
{
    int old_pin = this->pin;
    this->pin = pin;
    this->updateTitle();

    this->server_api->liveDelPinServer(old_pin);
    this->server_api->liveSetOutputServer(this->pin, this->value, this->name.toStdString());
}

QString dynamicOutput::getName()
{
    return this->name;
}
void dynamicOutput::setName(QString name)
{
    this->name = name;
    this->setObjectName(name);
    this->updateTitle();

    this->server_api->liveSetOutputServer(this->pin, this->value, this->name.toStdString());
}

void dynamicOutput::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit live_output_editButton_clicked(this->name);
}

void dynamicOutput::on_delButton_clicked()
{
    emit live_output_delButton_clicked(this->name);
}

void dynamicOutput::on_upButton_clicked()
{
    emit live_output_upButton_clicked(this->name);
}

void dynamicOutput::on_downButton_clicked()
{
    emit live_output_downButton_clicked(this->name);
}

void dynamicOutput::updateLcdNumber()
{
    ui->lcdNumber->display(this->value);

    int green = floor(this->value * 1.7);
    int red = 125 - (this->value * 2);
    if (red < 0)
        red = 0;
    ui->lcdNumber->setStyleSheet("background-color: rgb("+QString::number(red)+", "+ QString::number(green) +", 0);");
}

void dynamicOutput::updatePinValue(int new_value, bool dont_send_to_server)
{
    //qInfo() << "updatePinValue value: " << value << "\n";
    if (new_value < 0)
        this->value = 0;
    else if (new_value > 100)
        this->value = 100;
    else
        this->value = new_value;

    ui->pwm_slider->blockSignals(true);
    ui->pwm_slider->setValue(this->value);
    ui->pwm_slider->blockSignals(false);

    ui->pwm_spinBox->blockSignals(true);
    ui->pwm_spinBox->setValue(this->value);
    ui->pwm_spinBox->blockSignals(false);

    this->updateLcdNumber();

    if (this->value == 0)
        ui->offButton->setChecked(true);
    else
    {
        ui->offButton->setAutoExclusive(false);
        ui->offButton->setChecked(false);
        ui->offButton->setAutoExclusive(true);
    }

    if (this->value == 100)
        ui->onButton->setChecked(true);
    else
    {
        ui->onButton->setAutoExclusive(false);
        ui->onButton->setChecked(false);
        ui->onButton->setAutoExclusive(true);
    }

    if (dont_send_to_server == false)
        this->server_api->liveSetOutputServer(this->pin, this->value, this->name.toStdString());
}

void dynamicOutput::on_onButton_clicked(bool checked)
{
    if (checked == true)
    {
        this->updatePinValue(100);
    }
}

void dynamicOutput::on_offButton_clicked(bool checked)
{
    if (checked == true)
    {
        this->updatePinValue(0);
    }
}

void dynamicOutput::on_pwm_slider_valueChanged(int value)
{
    this->updatePinValue(value);
}

void dynamicOutput::on_pwm_spinBox_valueChanged(int arg1)
{
    this->updatePinValue(arg1);
}
