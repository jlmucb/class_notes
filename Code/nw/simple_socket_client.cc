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

const char* host = "www.localhost.com";
int port = 8122;

int main(int an, char** av) {

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));

  int sd = socket(AF_INET, SOCK_STREAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  struct hostent* he = gethostbyname(host);
  memcpy(&addr.sin_addr.s_addr, he->h_addr, he->h_length);
  // addr.sin_addr.s_addr = INADDR_ANY;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return 1;
  }
   
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
       
  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return 1;
  }
  const char* hello = "hello";
  send(sd, hello , strlen(hello) , 0 );
  const int buf_size = 8192;
  char buf[buf_size];
  int n = read(conn, buf, buf_size);

  return 0;
}
