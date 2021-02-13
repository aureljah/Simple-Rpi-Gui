#include "main.hpp"

void test_socket()
{
  OpensslWrapper *opensslWrap = new OpensslWrapper();

  ISocket *sock = new Socket(opensslWrap);
  ISocket *client = NULL;
  try {
    
    sock->bind(4242);
    sock->listen(1);
    while(1)
    {
      if ((client = sock->accept()) != NULL)
      { 
        std::string msg = client->read(4096);
        std::cout << "[SERVER] client: " << msg << std::endl;
        client->write("Heyyy !!!");
        delete client;
      }
    }
  }
  catch(char const *msg) {
    std::cerr << "Error: " << msg << "\n";
  }
  delete sock;
}

int main(int ac, char **av)
{
  std::cout << "Starting Server...\n" << std::endl;
  test_socket();
  return (0);
}
