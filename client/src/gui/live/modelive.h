#ifndef MODELIVE_H
#define MODELIVE_H

#include <QVBoxLayout>
#include <QScrollArea>
#include <QMainWindow>
#include <QDebug>
#include <QStyle>
#include <QDesktopWidget>
#include <iostream>
//#include <thread>
//#include <mutex>
#include <string>
#include <map>
#include <list>
#include "dynamicoutput.h"
#include "gpiosettingdialog.h"
#include "../tools/serverApi.hpp"
#include "../tools/tools.h"

class modeLive : public QObject
{
    Q_OBJECT

public:
    modeLive(QWidget *live_tab, serverApi *server_api);

    void addInputOutput(QString name, int pin, gpioSettingDialog::gpio_type type, QString old_name, int old_pin);
    void startGpioSettingDialog(gpioSettingDialog::gpio_type type, QString old_name = nullptr);

    std::list<QString> getUsedName(QString name_ignored = nullptr);
    std::list<int> getUsedPins(int pin_ignored = -1);


private slots:
    void live_output_editButton_clicked(QString name);
    void live_output_delButton_clicked(QString name);
    void live_output_upButton_clicked(QString name);
    void live_output_downButton_clicked(QString name);

private:
    QWidget *live_tab;
    serverApi *server_api;

    QVBoxLayout *vlayout;
    std::map<QString, dynamicOutput*> dyn_widgets;
};


#endif // MODELIVE_H
