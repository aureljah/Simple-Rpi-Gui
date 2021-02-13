#include <sys/types.h>
#include <iostream>
#include <string.h>
#include "Socket.hpp"

Socket::Socket(OpensslWrapper* openssl)
	: _fd(0), _sin(), _ssl(NULL), _openssl(openssl)
{
	WSADATA WSAData;
	
	WSAStartup(MAKEWORD(2, 0), &WSAData);
    _fd = socket(AF_INET, SOCK_STREAM, /*IPPROTO_TCP*/ 0);
    char on = 1;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    _sin.sin_family = AF_INET;
}

Socket::Socket(int fd, const struct sockaddr_in& sin, SSL *ssl, OpensslWrapper* openssl)
	: _fd(fd), _sin(sin), _ssl(ssl), _openssl(openssl)
{
    int sin_size = sizeof(_sin);
    getsockname(_fd, reinterpret_cast<struct sockaddr*>(&_sin), &sin_size);
}

Socket::~Socket()
{
	this->_openssl->freeSSL(this->_ssl);
	closesocket(_fd);
	WSACleanup();
}

void Socket::connect(std::string ip, int port)
{
	int ret = 0;

    //associate(ip.data(), port, ::connect);
    _sin.sin_port = htons(port);
    _sin.sin_addr.s_addr = inet_addr(ip.c_str());
    if (!::connect(_fd, reinterpret_cast<struct sockaddr*>(&_sin), sizeof(_sin)))
    {
        int sin_size = sizeof(_sin);
        getsockname(_fd, reinterpret_cast<struct sockaddr*>(&_sin), &sin_size);

		this->_ssl = this->_openssl->newSSL(this->_fd);
		if ((ret = SSL_connect(this->_ssl)) == -1)
		{
			SSL_get_error(this->_ssl, ret);
			throw("SSL_connect");
		}
    }
    else
        throw("could not connect !");
}

void Socket::bind(int port)
{
    //associate(NULL, port, ::bind);
    std::cout << "INFO: bind on port " << port << "\n";
    _sin.sin_port = htons(port);
    _sin.sin_addr.s_addr = INADDR_ANY;
    if (!::bind(_fd, reinterpret_cast<struct sockaddr*>(&_sin), sizeof(_sin)))
    {
        int sin_size = sizeof(_sin);
        getsockname(_fd, reinterpret_cast<struct sockaddr*>(&_sin), &sin_size);
    }
    else
        std::cout << "ERROR: error on bind --> " << strerror(errno) << "\n";
}

void Socket::listen(int queueSize)
{
    std::cout << "INFO: listen...\n";
    ::listen(this->_fd, queueSize);
}

ISocket* Socket::accept()
{
	int ret = 0;
	SSL *cssl;
    struct sockaddr_in csin;
    int sin_size = sizeof(csin);
    const int cfd = ::accept(_fd, reinterpret_cast<struct sockaddr*>(&csin), &sin_size);
    if (cfd < 0)
	    throw("accept");

	cssl = this->_openssl->newSSL(cfd);
	if ((SSL_accept(cssl)) <= 0) {
		SSL_get_error(cssl, ret);
		//ERR_print_errors_fp(stdout);
		throw("SSL_accept");
	}

	std::cout << "INFO: new connexion accepted - " << SSL_get_version(cssl) << " used.\n";
	X509 *peer = NULL;
	peer = SSL_get_peer_certificate(cssl);
	if (peer) {
		if (SSL_get_verify_result(cssl) == X509_V_OK) {
			std::cout << "INFO: verify peer cert is OK" << std::endl;
		}
		else
			std::cout << "INFO: SSL_get_verify_result failed\n";
	}
	else
		std::cout << "INFO: SSL_get_peer_certificate failed\n";

	return (new Socket(cfd, csin, cssl, this->_openssl));
}

int Socket::read(void* buff, size_t len)
{
    //int r = recv(_fd, static_cast<char *>(buff), len, 0);
	int r = SSL_read(this->_ssl, buff, len);
	if (r < 0)
	{
		SSL_get_error(this->_ssl, r);
		throw("SSL_read");
	}
    return r;
}

int Socket::write(const void* buff, size_t len)
{
    //int r = send(_fd, static_cast<const char *>(buff), len, 0);
	int r = SSL_write(this->_ssl, buff, len);
	if (r <= 0)
	{
		SSL_get_error(this->_ssl, r);
		throw("SSL_write");
	}
    return r;
}

int Socket::write(const std::string& msg)
{
    std::string tmp = msg + "\r\n";
    const void *buff = static_cast<const void *>(tmp.c_str());
    return (this->write(buff, tmp.length()));
}


std::string Socket::read(size_t read_len)
{
    char *buff = new char[read_len + 1];
    memset(buff, 0, read_len + 1);
    this->read(buff, read_len);
    std::string msg(buff);
    delete[] buff;
    return (msg);
}

SSL *Socket::getSocksslFd() const
{	return (this->_ssl);	}

int Socket::getSockFd() const
{   return (this->_fd); }

std::string Socket::getIpStr() const
{
    std::string str;

    str = inet_ntoa(this->_sin.sin_addr);
    return (str);
}

uint32_t Socket::getIpInt() const
{
    uint32_t i;

    i = static_cast<uint32_t> (this->_sin.sin_addr.s_addr);
    return (i);
}

uint16_t Socket::getPort() const
{
    uint16_t port;

    port = static_cast<uint16_t>(this->_sin.sin_port);
    return (port);
}

OpensslWrapper *Socket::getOpensslWrapperPtr() const
{	return this->_openssl;	}