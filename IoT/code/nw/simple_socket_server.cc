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


  if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
  }
  if (listen(sd, 10) != 0) {
  }

  int addrlen = sizeof(sockaddr);
  int conn = accept(sd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
  if (conn < 0) {
  }

  // concurrent
  /*
  pthread_t tid[60];
  int i = 0;
  while(1) {
        addr_size = sizeof serverStorage;
        ns = accept(sd, (struct sockaddr *) &serverStorage, &addr_size);

        if (pthread_create(&tid[i++], NULL, socketThread, &ns) != 0 )
           printf("Failed to create thread\n");

        if( i >= 50) {
          i = 0;
          while(i < 50) {
            pthread_join(tid[i++],NULL);
          }
          i = 0;
        }
    }
   */

  const int buf_size = 8192;
  char buf[buf_size];
  int n = read(conn, buf, buf_size);

  std::string response = "Hello\n";
  send(connection, response.c_str(), response.size(), 0);

  close(conn);
  close(sd);
  return 0;
}
