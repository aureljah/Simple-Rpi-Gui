#pragma once

#include <openssl/ssl.h>
#include <openssl/err.h>

class OpensslWrapper
{
public:
    OpensslWrapper();
    ~OpensslWrapper();

public:
    SSL *newSSL(int fd); // on new connected socket
    void freeSSL(SSL *ssl); // when closing a socket

private:
    void initOpenssl();
    void cleanupOpenssl();
    void createContext();
    void configureContext();

private:
    SSL_CTX *ctx;
};