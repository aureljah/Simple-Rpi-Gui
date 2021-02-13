#ifndef TOOLS_H
#define TOOLS_H

//#include <QMainWindow>
#include <QDebug>
//#include <QDesktopWidget>
#include <QMessageBox>
#include <thread>
#include <mutex>
#include <map>
#include <list>

class Tools
{
public:
    static void errorDialog(QString msg);
    static std::list<int> getGPIONumberList(); // get a list with all GPIO number usable
 
private:
    Tools() {} // ctor
};

#endif // TOOLS_H
