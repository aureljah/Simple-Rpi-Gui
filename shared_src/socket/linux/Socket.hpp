#pragma once

#include <iostream>
#include <string.h>
#include <netinet/in.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include "ASocket.hpp"
#include "ISocket.hpp"

class Socket : public ASocket
{
public:
    Socket(std::string path_cert, OpensslWrapper::socketType type); // server's socket
    Socket(int fd, const struct sockaddr_in& sin, SSL* ssl, OpensslWrapper* openssl); // client's socket
    ~Socket();

    void connect(std::string ip, int port);
    void bind(int port);
    void listen(int size);
    ISocket* accept();

    int read(void *msg, size_t len);
    std::string read(size_t read_len);
    std::string readLine(size_t buffer_len);
    int write(const void *msg, size_t len);
    int write(const std::string& msg); // with '\r\n' at the end

    SSL *getSocksslFd() const;
    OpensslWrapper *getOpensslWrapperPtr() const;
    int getSockFd() const;
    std::string getIpStr() const;
    //uint32_t getIpInt() const;
    uint16_t getPort() const;

private:
    int _fd;
    struct sockaddr_in _sin;
    SSL *_ssl;
    OpensslWrapper *_openssl;

    u_int16_t port; // to save the port used with connect
    std::string ip;

    std::string buffer;
};
