#ifndef GPIOSETTINGDIALOG_H
#define GPIOSETTINGDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QtDebug>
#include <QMessageBox>
#include <QStyle>
#include <QDesktopWidget>
#include "../../../tools/tools.h"

namespace Ui {
class gpioSettingDialog;
}

class gpioSettingDialog : public QDialog
{
    Q_OBJECT

public:
    enum gpio_type {
        INPUT,
        OUTPUT
    };

public:
    // old_name is used only when editing an existant
    explicit gpioSettingDialog(std::list<QString> used_name, std::list<int> used_pins,
                               gpio_type type, QString old_name, int old_pin, QWidget *parent = nullptr);
    ~gpioSettingDialog();

    bool checkInput();

signals:
    void dialog_accepted(QString name, int pin, gpio_type type, QString old_name, int old_pin);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::gpioSettingDialog *ui;
    std::list<QString> used_name;
    gpio_type type;
    QString old_name;
    int old_pin;
};

#endif // GPIOSETTINGDIALOG_H
