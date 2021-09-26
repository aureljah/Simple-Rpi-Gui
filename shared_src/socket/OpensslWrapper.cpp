#include "OpensslWrapper.hpp"

OpensslWrapper::OpensslWrapper(std::string path_ctx, std::string path_key, socketType stype) :
    path_ctx(path_ctx.c_str()), path_key(path_key.c_str()), stype(stype), ctx(NULL)
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

    if (this->stype == SERVER)
        method = SSLv23_server_method(); // all SSL/TLS version supported
    else
        method = SSLv23_client_method();

    ctx = SSL_CTX_new(method);
    if (!ctx)
	    throw("Unable to create SSL context");

    this->ctx = ctx;
}

void OpensslWrapper::configureContext()
{
    SSL_CTX_set_ecdh_auto(this->ctx, 1);

    SSL_CTX_set_verify(this->ctx, SSL_VERIFY_PEER
             | SSL_VERIFY_FAIL_IF_NO_PEER_CERT
            /*| SSL_VERIFY_CLIENT_ONCE*/
            , NULL);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(this->ctx, this->path_ctx, SSL_FILETYPE_PEM) <= 0)
        throw("SSL_CTX_use_certificate_file");

    if (SSL_CTX_use_PrivateKey_file(this->ctx, this->path_key, SSL_FILETYPE_PEM) <= 0)
        throw("SSL_CTX_use_PrivateKey_file");

    if(!SSL_CTX_load_verify_locations(ctx, this->path_ctx, NULL))
        throw("SSL_CTX_load_verify_locations: Could not load trusted CA certificates.");

    if (this->stype == SERVER)
    {
        STACK_OF(X509_NAME) *cert_name = NULL;
        if ((cert_name = SSL_load_client_CA_file(this->path_ctx)) == NULL)
            throw("SSL_load_client_CA_file failed !");

        SSL_CTX_set_client_CA_list(ctx, cert_name);
    }
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