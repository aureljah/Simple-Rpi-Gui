#include "dynamicoutput.h"
#include "ui_dynamicoutput.h"

dynamicOutput::dynamicOutput(int pin, QString name, QWidget *parent) :
    QWidget(parent), pin(pin), name(name), ui(new Ui::dynamicOutput), value(0)
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

    connect(ui->pwm_slider, &QSlider::valueChanged,
            ui->pwm_spinBox, &QSpinBox::setValue);
    connect(ui->pwm_spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            ui->pwm_slider, &QSlider::setValue);
}

dynamicOutput::~dynamicOutput()
{
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
    this->pin = pin;
    this->updateTitle();
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

void dynamicOutput::updatePinValue(int new_value)
{
    if (new_value < 0)
        this->value = 0;
    else if (new_value > 100)
        this->value = 100;
    else
        this->value = new_value;

    ui->pwm_slider->setValue(this->value);
    ui->pwm_spinBox->setValue(this->value);

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
    //qInfo() << "on_pwm_slider_valueChanged value: " << value << "\n";
    this->updatePinValue(value);
}

void dynamicOutput::on_pwm_spinBox_valueChanged(int arg1)
{
    //qInfo() << "on_pwm_spinBox_valueChanged value: " << arg1 << "\n";
    this->updatePinValue(arg1);
}
