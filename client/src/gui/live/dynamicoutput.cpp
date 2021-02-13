#include "dynamicoutput.h"
#include "ui_dynamicoutput.h"

dynamicOutput::dynamicOutput(int pin, QString name, QWidget *parent) :
    QWidget(parent), pin(pin), name(name), ui(new Ui::dynamicOutput)
{
    qInfo() << "New dynamicOutput pin: " << pin << "\n";
    ui->setupUi(this);
    ui->downButton->setIcon(ui->delButton->style()->standardIcon(QStyle::SP_ArrowDown));
    ui->upButton->setIcon(ui->delButton->style()->standardIcon(QStyle::SP_ArrowUp));
    //ui->delButton->setIcon(ui->delButton->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    ui->delButton->setIcon(ui->delButton->style()->standardIcon(QStyle::SP_TrashIcon));
    this->updateTitle();
    this->setObjectName(name);
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
