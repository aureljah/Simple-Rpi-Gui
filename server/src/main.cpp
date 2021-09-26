#include "main.hpp"

int main(int ac, char **av)
{
  signal(SIGPIPE, SIG_IGN);

  std::cout << "[INFO]: Starting Server...\n" << std::endl;
  try {
    MainServer* server = new MainServer();
    server->run(4242, "mycert.pem", "key.pem");
  }
  catch(char const *msg) {
    std::cerr << "Main Error: " << msg << "\n";
  }

  return (0);
}
