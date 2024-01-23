#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

#define FAIL    -1

int OpenListener(int port) {
  struct sockaddr_in addr;

  int sd = socket(PF_INET, SOCK_STREAM, 0);
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
    perror("can't bind port");
    abort();
  }
  if (listen(sd, 10) != 0) {
    perror("Can't configure listening port");
    abort();
  }
  return sd;
}

int isRoot() {
  if (getuid() != 0) {
    return 0;
  } else {
    return 1;
  }
}

SSL_CTX* InitServerCTX() {
  SSL_CTX *ctx;

  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();
  SSL_METHOD* method = (SSL_METHOD*) SSLv23_server_method();
  ctx = SSL_CTX_new(method);
  if (ctx == NULL) {
    ERR_print_errors_fp(stderr);
    abort();
  }
  return ctx;
}

void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile) {
  if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 ) {
    ERR_print_errors_fp(stderr);
    abort();
  }
  if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 ) {
    ERR_print_errors_fp(stderr);
    abort();
  }
  if (!SSL_CTX_check_private_key(ctx) ) {
    fprintf(stderr, "Private key does not match the public certificate\n");
    abort();
  }
}

void ShowCerts(SSL* ssl) {
  X509 *cert = nullptr;
  char *line;

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
    printf("No certificates.\n");
}

void Servlet(SSL* ssl) {
  char buf[1024];
  char reply[1024];
  int sd, bytes;
  const char* HTMLecho="<html><body><pre>%s</pre></body></html>\n\n";

  if (SSL_accept(ssl) == FAIL)
    ERR_print_errors_fp(stderr);
  else {
    ShowCerts(ssl);
    bytes = SSL_read(ssl, buf, sizeof(buf));
    if (bytes > 0) {
      buf[bytes] = 0;
      printf("Client msg: \"%s\"\n", buf);
      sprintf(reply, HTMLecho, buf);
      SSL_write(ssl, reply, strlen(reply));
    }
    else
      ERR_print_errors_fp(stderr);
  }
  sd = SSL_get_fd(ssl);
  SSL_free(ssl);
  close(sd);
}


int main(int count, char *av[]) {
  SSL_CTX *ctx;
  int server;
  char* portnum;

  if(!isRoot()) {
    printf("This program must be run as root/sudo user!!");
    exit(0);
  }
  if (count != 2) {
    printf("Usage: %s <portnum>\n", av[0]);
    exit(0);
  }
  SSL_library_init();

  portnum = av[1];
  ctx = InitServerCTX();
  LoadCertificates(ctx, "mycert.pem", "mycert.pem");
  server = OpenListener(atoi(portnum));

  while (1) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    SSL *ssl;

    int client = accept(server, (struct sockaddr*)&addr, &len);
    printf("Connection: %s:%d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client);
    Servlet(ssl);
  }
  close(server);
  SSL_CTX_free(ctx);
}
