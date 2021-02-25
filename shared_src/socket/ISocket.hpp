#pragma once

#include <string>
#include <stdint.h>
#include "OpensslWrapper.hpp"

class ISocket
{
public:
    virtual ~ISocket() {}

    virtual void connect(std::string ip, int port) = 0;
    virtual void bind(int port) = 0;
    virtual void listen(int size) = 0;
    virtual ISocket* accept() = 0;

    virtual int read(void *msg, size_t len) = 0;
    virtual std::string read(size_t read_len) = 0;
    virtual std::string readLine(size_t buffer_len) = 0; // read until \r\n is found, keep excess in buffer for future use
    virtual int write(const void *msg, size_t len) = 0;
    virtual int write(const std::string& msg) = 0;

    //virtual SSL *getSocksslFd() const = 0;
    virtual OpensslWrapper *getOpensslWrapperPtr() const = 0;
    virtual int getSockFd() const = 0;
    virtual std::string getIpStr() const = 0;
    virtual uint32_t getIpInt() const = 0;
    virtual uint16_t getPort() const = 0;
};
