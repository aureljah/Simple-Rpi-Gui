#include "OpensslWrapper.hpp"

OpensslWrapper::OpensslWrapper()
{
    this->initOpenssl();
    this->createContext();
    this->configureContext();
}

OpensslWrapper::~OpensslWrapper()
{
    SSL_CTX_free(this->ctx);
    this->cleanupOpenssl();
}

void OpensslWrapper::initOpenssl()
{ 
    SSL_load_error_strings();	
    OpenSSL_add_ssl_algorithms();
}

void OpensslWrapper::cleanupOpenssl()
{
    EVP_cleanup();
}

void OpensslWrapper::createContext()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_client_method(); // all SSL/TLS version supported
    //method = TLS_client_method(); // only TLS supported (any version)
	// TLS_client_method not supported on windows ???? (maybe openssl version)

    ctx = SSL_CTX_new(method);
    if (!ctx)
	    throw("Unable to create SSL context");

    this->ctx = ctx;
}

void OpensslWrapper::configureContext()
{
    SSL_CTX_set_ecdh_auto(this->ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(this->ctx, "mycert.pem", SSL_FILETYPE_PEM) <= 0)
        throw("SSL_CTX_use_certificate_file");

    if (SSL_CTX_use_PrivateKey_file(this->ctx, "mycert.pem", SSL_FILETYPE_PEM) <= 0 )
        throw("SSL_CTX_use_PrivateKey_file");

    if(!SSL_CTX_load_verify_locations(this->ctx, "mycert.pem", NULL))
        throw("SSL_CTX_load_verify_locations: Could not load trusted CA certificates.");
}

SSL *OpensslWrapper::newSSL(int fd)
{
    SSL *ssl;

    ssl = SSL_new(this->ctx);
    SSL_set_fd(ssl, fd);

    return ssl;
}

void OpensslWrapper::freeSSL(SSL *ssl)
{
    SSL_free(ssl);
}