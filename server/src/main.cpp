#include "main.hpp"

void test_socket()
{
  ISocket *sock = new Socket("mycert.pem", OpensslWrapper::SERVER);
  ISocket *client = NULL;
  try {
    sock->bind(4242);
    sock->listen(1);

    int count = 0;
    while(true)
    {
      std::cout << "INFO: Ready, Waiting client to connect...\n\n";
      if ((client = sock->accept()) != NULL)
      { 
        //sleep(1);
        std::thread t2(test_second_socket, client->getIpStr(), 4243);
        
        //client->write("Heyyy !!! - " + std::to_string(count) + "\n");
        //test_second_socket(client->getIpStr(), 4243);

        while (true)
        {
          try {
            std::string msg = client->read(4096);
            std::cout << "[SERVER] client: " << msg << std::endl;
            client->write("Heyyy !!! - " + std::to_string(count) + "\n");
            sleep(1);
            count++;
          }
          catch(char const *msg) {
            std::cout << "INFO: client disconnected ! - " << msg << "\n";
            break;
          }
        }
        delete client;
        t2.join();
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
  std::cout << "\nin test_second_socket\n";
  ISocket *client2 = new Socket("mycert.pem", OpensslWrapper::CLIENT);

  try {
    sleep(2);
    client2->connect(ip, port);
    int count = 0;
    while (true)
    {
      sleep(5);
      client2->write("PING from 2nd server thread - " + std::to_string(count));
      count++;
    }
  }
  catch(char const *msg) {
    std::cerr << "2nd thread: Error: " << msg << "\n";
  }
  delete client2;
  std::cout << "==> End of second thread\n\n";
}

int main(int ac, char **av)
{
  signal(SIGPIPE, SIG_IGN);

  std::cout << "[INFO]: Starting Server...\n" << std::endl;
  //test_socket();
  MainServer* server = new MainServer();
  server->run(4242, "mycert.pem");

  return (0);
}
