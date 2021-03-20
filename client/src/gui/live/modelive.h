#ifndef MODELIVE_H
#define MODELIVE_H

#include <QVBoxLayout>
#include <QScrollArea>
#include <QMainWindow>
#include <QDebug>
#include <QStyle>
#include <QDesktopWidget>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <map>
#include <list>
#include "Systemcall.hpp"
#include "dynamicoutput.h"
#include "dynamicinput.h"
#include "gpiosettingdialog.h"
#include "../tools/serverApi.hpp"
#include "../tools/tools.h"

# define INPUT_POLL_TIME 2000

class modeLive : public QObject
{
    Q_OBJECT

public:
    modeLive(QWidget *live_tab, serverApi *server_api);
    ~modeLive();

    void addInputOutput(QString name, int pin, gpioSettingDialog::gpio_type type, QString old_name, int old_pin, int value = 0);
    void startGpioSettingDialog(gpioSettingDialog::gpio_type type, QString old_name = nullptr);

    std::list<QString> getUsedName(QString name_ignored = nullptr);
    std::list<int> getUsedPins(int pin_ignored = -1);
    std::map<QString, int> getOutputNamePinList();

    QString getOutputNameFromPin(int pin);
    //int getOutputValueFromPin(int pin);

private:
    void server_input_receiver();
    dynamicOutput *find_dyn_out(QString name);
    dynamicInput *find_dyn_in(QString name);

private slots:
    void live_output_editButton_clicked(QString name);
    void live_output_delButton_clicked(QString name);
    void live_output_upButton_clicked(QString name);
    void live_output_downButton_clicked(QString name);

    void live_input_editButton_clicked(QString name);
    void live_input_delButton_clicked(QString name);
    void live_input_upButton_clicked(QString name);
    void live_input_downButton_clicked(QString name);

public slots:
    void add_input(int pin, std::string name);
    void add_output(int pin, int value, std::string name);
    void update_input_value(int pin, int value);
    void update_output_value(int pin, int value, bool dont_send_to_server = false);

private:
    QWidget *live_tab;
    serverApi *server_api;

    std::thread *input_thread;
    std::mutex input_thread_mutex;
    bool input_thread_running;

    QVBoxLayout *out_vlayout;
    std::map<QString, dynamicOutput*> dyn_o_widgets;
    QVBoxLayout *in_vlayout;
    std::map<QString, dynamicInput*> dyn_i_widgets;
};


#endif // MODELIVE_H
