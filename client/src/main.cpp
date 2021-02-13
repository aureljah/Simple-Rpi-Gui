#include "main.hpp"

void test_socket()
{
  OpensslWrapper *opensslWrap = new OpensslWrapper();

  ISocket *sock = new Socket(opensslWrap);
  try {
    sock->connect("127.0.0.1", 4242);
    sock->write("Heyyy !!!");
    std::string msg = sock->read(4096);
    std::cout << "[CLIENT] server: " << msg << std::endl;
  }
  catch(char const *msg) {
    std::cerr << "Error: " << msg << "\n";
  }
  delete sock;
}

int main(int ac, char **av)
{
  std::cout << "Starting Client...\n" << std::endl;
  try {
    test_socket();
  }
  catch(char const *msg) {
    std::cerr << "Error: " << msg << "\n";
  }
  //systemcall::sys_usleep(5000);
  return (0);
}
