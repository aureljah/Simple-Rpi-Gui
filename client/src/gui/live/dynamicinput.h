#ifndef DYNAMICINPUT_H
#define DYNAMICINPUT_H

#include <QWidget>
#include <QStyle>
#include <QDebug>
#include <cmath>
#include "../tools/serverApi.hpp"

namespace Ui {
class dynamicInput;
}

class dynamicInput : public QWidget
{
    Q_OBJECT

public:
    explicit dynamicInput(serverApi *server_api, int pin, QString name, QWidget *parent = nullptr);
    ~dynamicInput();

    int getPin();
    void setPin(int pin);
    QString getName();
    void setName(QString name);

    void updateValue(int value);

private:
    void updateTitle();

signals:
    void live_input_editButton_clicked(QString name);
    void live_input_delButton_clicked(QString name);
    void live_input_upButton_clicked(QString name);
    void live_input_downButton_clicked(QString name);

private slots:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void on_delButton_clicked();
    void on_downButton_clicked();
    void on_upButton_clicked();

private:
    serverApi *server_api;
    int pin;
    QString name;
    Ui::dynamicInput *ui;

    int value;
};

#endif // DYNAMICINPUT_H
