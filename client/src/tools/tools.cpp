#include "tools.h"

std::list<int> Tools::getGPIONumberList()
{
    int gpio_pins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
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
