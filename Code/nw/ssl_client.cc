#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define FAIL  -1

int OpenConnection(const char* hostname, int port) {
  struct hostent *host;
  struct sockaddr_in addr;

  if ((host = gethostbyname(hostname)) == NULL) {
    return FAIL;
  }
  int sd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = *(long*)(host->h_addr);
  if (connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
    close(sd);
    return FAIL;
  }
  return sd;
}

SSL_CTX* InitCTX(void) {
  SSL_CTX *ctx;

  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();
  SSL_METHOD* method = (SSL_METHOD*)SSLv23_client_method();
  ctx = SSL_CTX_new(method);
  if (ctx == NULL) {
    return nullptr;
  }
  return ctx;
}

void ShowCerts(SSL* ssl) {
  X509 *cert = nullptr;
  char *line;

  // SSL_get_peer_certificate() returns a pointer to the X509 certificate the peer
  // presented. If the peer did not present a certificate, NULL is returned.
  // SSL_get_verify_result() returns the result of the verification of the X509 
  // certificate presented by the peer, if any.
  // SSL_CTX_set_verify() sets the verification flags for ctx to be mode 
  // and specifies the verify_callback function to be used. If no callback function 
  // shall be specified, the NULL pointer can be used for verify_callback.
  // mode SSL_VERIFY_PEER: the server sends a client certificate request to the client.
  //  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, verify_callback);
  cert = SSL_get_peer_certificate(ssl);
  if (cert != NULL) {
    printf("Server certificates:\n");
    line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    printf("Subject: %s\n", line);
    free(line);
    line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    printf("Issuer: %s\n", line);
    free(line);
    X509_free(cert);
  } else
    printf("Info: No client certificates configured.\n");
}

int main(int count, char *av[]) {
  SSL_CTX *ctx;
  int server;
  SSL *ssl;
  char buf[1024];
  int bytes;
  char *hostname, *portnum;

  if (count != 3) {
    printf("usage: %s <hostname> <portnum>\n", av[0]);
    exit(0);
  }
  SSL_library_init();
  hostname = av[1];
  portnum = av[2];

  ctx = InitCTX();
  server = OpenConnection(hostname, atoi(portnum));
  ssl = SSL_new(ctx);
  SSL_set_fd(ssl, server);
  if (SSL_connect(ssl) == FAIL) {
    ERR_print_errors_fp(stderr);
    return 1;
  }

  const char *msg = "Hello???";
  printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
  ShowCerts(ssl);
  SSL_write(ssl, (unsigned char*)msg, strlen(msg));
  bytes = SSL_read(ssl, buf, sizeof(buf));
  buf[bytes] = 0;
  printf("Received: \"%s\"\n", buf);
  SSL_free(ssl);

  close(server);
  SSL_CTX_free(ctx);
  return 0;
}
