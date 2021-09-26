#pragma once

#include <iostream>
#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>

class OpensslWrapper
{
public:
    enum socketType {
        CLIENT,
        SERVER
    };

public:
    OpensslWrapper(std::string path_ctx, std::string path_key, socketType stype);
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
    const char *path_ctx;
    const char *path_key;
    socketType stype;
    SSL_CTX *ctx;

};