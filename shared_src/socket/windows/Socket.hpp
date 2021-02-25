#pragma once

#include <string.h>
#include <winsock2.h>
#include "ASocket.hpp"
#include "ISocket.hpp"

class Socket : public ASocket
{
public:
    Socket(std::string path_cert, OpensslWrapper::socketType type); // server's socket
    Socket(int fd, const struct sockaddr_in& sin, SSL *ssl, OpensslWrapper *openssl); // client's socket
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
    uint32_t getIpInt() const;
    uint16_t getPort() const;

public:
	SOCKET _fd; // not really an fd in windows, but kinda the same use
    struct sockaddr_in _sin;
	SSL *_ssl;
	OpensslWrapper *_openssl;
};
