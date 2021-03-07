#include "dynamicinput.h"
#include "ui_dynamicinput.h"

dynamicInput::dynamicInput(serverApi *server_api, int pin, QString name, QWidget *parent) :
    QWidget(parent), server_api(server_api), pin(pin), name(name), ui(new Ui::dynamicInput), value(0)
{
    qInfo() << "New dynamicInput pin: " << pin << "\n";
    ui->setupUi(this);
    ui->downButton->setIcon(ui->delButton->style()->standardIcon(QStyle::SP_ArrowDown));
    ui->upButton->setIcon(ui->delButton->style()->standardIcon(QStyle::SP_ArrowUp));
    //ui->delButton->setIcon(ui->delButton->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    ui->delButton->setIcon(ui->delButton->style()->standardIcon(QStyle::SP_TrashIcon));
    this->setName(this->name); // insted of updateTitle to also use serverApi call
    this->setObjectName(name);
    this->updateValue(this->value);
}

dynamicInput::~dynamicInput()
{
    this->server_api->liveDelPinServer(this->pin);
    delete ui;
}

void dynamicInput::updateTitle()
{
    ui->groupBox->setTitle(this->name + QString(" - GPIO ") + QString::number(this->pin));
}

void dynamicInput::updateValue(int value)
{
    this->value = value;
    ui->progressBar->setValue(value);
    ui->label->setText(QString::fromStdString(std::to_string(value)));

    int green = floor(this->value * 1.7);
    int red = 125 - (this->value * 2);
    if (red < 0)
        red = 0;
    ui->label->setStyleSheet("color: rgb("+QString::number(red)+", "+ QString::number(green) +", 0);");
}

int dynamicInput::getPin()
{
    return this->pin;
}
void dynamicInput::setPin(int pin)
{
    int old_pin = this->pin;
    this->pin = pin;
    this->updateTitle();

    this->server_api->liveDelPinServer(old_pin);
    this->server_api->liveSetInputServer(this->pin, this->name.toStdString());
}

QString dynamicInput::getName()
{
    return this->name;
}

void dynamicInput::setName(QString name)
{
    this->name = name;
    this->setObjectName(name);
    this->updateTitle();

    this->server_api->liveSetInputServer(this->pin, this->name.toStdString());
}

void dynamicInput::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit live_input_editButton_clicked(this->name);
}

void dynamicInput::on_delButton_clicked()
{
    emit live_input_delButton_clicked(this->name);
}

void dynamicInput::on_downButton_clicked()
{
    emit live_input_downButton_clicked(this->name);
}

void dynamicInput::on_upButton_clicked()
{
    emit live_input_upButton_clicked(this->name);
}
