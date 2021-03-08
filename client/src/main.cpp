#include "main.hpp"

int main(int ac, char **av)
{
  QApplication app (ac, av);

  qRegisterMetaType<std::string>("std::string");
  //Q_DECLARE_METATYPE(std::string);

  std::cout << "Starting Client...\n" << std::endl;
  try {
    //auto win = new MainWindow();
    //win->show();
    new MainWindow();
    return app.exec();
  }
  catch(char const *msg) {
    std::cerr << "Error: " << msg << "\n";
  }

  return (-1);
}
