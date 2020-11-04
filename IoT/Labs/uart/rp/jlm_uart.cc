// Manferdelli, Raspberry Pi UART using RX/TX pins

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#ifndef byte
typedef unsigned char byte;
#endif

// /dev/ttyAMO is the Tx/Rx pins for raspberry pi older models.
//#define RxTxDevice "/dev/ttyAMA0"
// We can now use /dev/serial0
#define RxTxDevice "/dev/serial0"

//  Tx is RP pin 8
//  Rx is RP pin 10

void clearBuf(byte* buf, int n) {
  for (int i = 0; i < n; i++) {
    buf[i] = 0;
  }
}

void printBuf(byte* in, int n) {
  for (int i = 0; i < n; i++) {
    printf(" %02x", in[i]);
    if ((i % 16) == 15) {
      printf("\n");
    }
  }
  printf("\n");
}

#define BUF_SIZE 256

int main(int an, char** av) {
  int fd = open(RxTxDevice, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    printf("Can't open RxTxDevice\n");
    return 1;
  }

  byte buf[BUF_SIZE];
  speed_t baud_rate = B9600;
  for (int i = 0; i < (an - 1); i++) {
    if (strcmp(av[i], "-baud") == 0) {
      unsigned ls = 0;
      ls = atoi(av[++i]);
      baud_rate = ls;
    }
  }
  // turn off blocking for reads.
  fcntl(fd, F_SETFL, 0); //fcntl(fd, F_GETFL)& ~O_NONBLOCK);
  // set baud rate
  struct termios t;
  tcgetattr(fd, &t);
  cfsetispeed(&t, baud_rate);
  cfsetospeed(&t, baud_rate);
  tcsetattr(fd, TCSANOW, &t);


  for(;;) {
    clearBuf(buf, BUF_SIZE);
    int n = read(fd, buf, BUF_SIZE - 1);
    if (n < 0) {
      printf("Can't read device\n");
      return 1;
    }
    if (n == 0)
      continue;
    printf("Read (%d): \n", n);
    printBuf(buf, n);
    buf[n+1] = 0;
    printf("%s\n", (char*)buf);
  }

  close(fd);
  return 0;
}
