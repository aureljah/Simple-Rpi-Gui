#include "tools.h"

std::list<int> Tools::getGPIONumberList()
{
    int gpio_pins[] = {3, 5, 7, 8, 10, 11, 12, 13, 15, 16, 18, 19, 21, 22, 23, 24, 26, 29, 31, 32, 33, 35, 36, 37, 38, 40};
    int len = sizeof(gpio_pins) / sizeof(gpio_pins[0]);
    return std::list<int>(gpio_pins, gpio_pins + len);
}

void Tools::errorDialog(QString msg)
{
    qInfo() << "Error: " << msg << "\n";
    QMessageBox msgBox;

    msgBox.setText("Error: " + msg);
    msgBox.exec();
}
