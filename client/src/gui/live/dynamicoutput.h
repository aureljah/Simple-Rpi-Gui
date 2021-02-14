#ifndef DYNAMICOUTPUT_H
#define DYNAMICOUTPUT_H

#include <QWidget>
#include <QStyle>
#include <QDebug>
#include <QTimer>

namespace Ui {
class dynamicOutput;
}

class dynamicOutput : public QWidget
{
    Q_OBJECT

    //Q_PROPERTY(QString name READ name)

public:
    explicit dynamicOutput(int pin, QString name, QWidget *parent = nullptr);
    ~dynamicOutput();

    int getPin();
    void setPin(int pin);
    QString getName();
    void setName(QString name);

private:
    void updateTitle();
    void updatePinValue(int new_value);

signals:
    void live_output_editButton_clicked(QString name);
    void live_output_delButton_clicked(QString name);
    void live_output_upButton_clicked(QString name);
    void live_output_downButton_clicked(QString name);

private slots:
    void on_delButton_clicked();
    void mouseDoubleClickEvent(QMouseEvent *event);
    void on_upButton_clicked();
    void on_downButton_clicked();
    void on_onButton_clicked(bool checked);
    void on_offButton_clicked(bool checked);

    void on_pwm_slider_valueChanged(int value);

    void on_pwm_spinBox_valueChanged(int arg1);

private:
    int pin;
    QString name;
    Ui::dynamicOutput *ui;

    int value;
};

#endif // DYNAMICOUTPUT_H
