#include "main.hpp"

void test_gui(QApplication &app, ISocket *sock)
{
    auto win = new MainWindow();
    win->show();

    app.exec();
}

void test_socket(QApplication &app)
{
  ISocket *sock = new Socket("mycert.pem", OpensslWrapper::SERVER);
  try {
    sock->connect("127.0.0.1", 4242);
    sock->write("Heyyy !!!");
    std::string msg = sock->read(4096);
    std::cout << "[CLIENT] server: " << msg << std::endl;

    test_gui(app, sock);
  }
  catch(char const *msg) {
    std::cerr << "Error: " << msg << "\n";
  }
  delete sock;
}

int main(int ac, char **av)
{
  QApplication app (ac, av);

  std::cout << "Starting Client...\n" << std::endl;
  try {
    auto win = new MainWindow();
    //win->show();
    app.exec();

    //test_socket(app);
  }
  catch(char const *msg) {
    std::cerr << "Error: " << msg << "\n";
  }

  //systemcall::sys_usleep(5000);
  return (0);
}
