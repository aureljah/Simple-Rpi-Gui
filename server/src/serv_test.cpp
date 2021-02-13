#include "main.hpp"

void test_socket()
{
  std::cout << "\nin 2nd thread\n";
  ISocket *sock = new Socket("mycert.pem", OpensslWrapper::SERVER);
  ISocket *client = NULL;
  try {
    sock->bind(4243);
    sock->listen(1);

    int count = 0;
    while(true)
    {
      if ((client = sock->accept()) != NULL)
      { 
        std::cout << "after accept: new connection\n";
        //sleep(1);
        //std::thread t2(test_second_socket, client->getIpStr(), 4243);
        //test_second_socket(client->getIpStr(), 4243);

        while (true)
        {
          try {
            std::string msg = client->read(4096);
            std::cout << "[recv]: " << msg << std::endl;
            //client->write("Hey from 2nd thread - " + std::to_string(count) + "\n");
            sleep(1);
            count++;
          }
          catch(char const *msg) {
            std::cout << "INFO: client disconnected ! - " << msg << "\n";
            break;
          }
        }
        delete client;
      }
    }
  }
  catch(char const *msg) {
    std::cerr << "Error: " << msg << "\n";
  }
  delete sock;
}

void test_second_socket(std::string ip, int port)
{
  ISocket *client2 = new Socket("mycert.pem", OpensslWrapper::CLIENT);

  try {
    client2->connect(ip, port);
    //test_socket();
    std::thread t2(test_socket);
    while (true)
    {
      sleep(3);
      client2->write("Hey from 1st thread !\n");
    }
  }
  catch(char const *msg) {
    std::cerr << "1st thread: Error: " << msg << "\n";
  }
}

int main(int ac, char **av)
{
  signal(SIGPIPE, SIG_IGN);

  std::cout << "Starting Test...\n" << std::endl;
  test_second_socket("127.0.0.1", 4242);
  return (0);
}
