#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

// ttyAMA0 is the Tx, Rx pins for the RP. 
// In the RP 3b it was used for the Bluetooth. Because of this,
// pins 8 & 10 were using the miniUART which is labeled ttyS0.
// So it is both the RX and TX pins as it refers to the device itself.

int main(int argc, char ** argv) {
  int fd;
  // Open the Port. We want read/write, no "controlling tty" status, and open it no matter what state DCD is in
  fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
    perror("open_port: Unable to open /dev/ttyAMA0 - ");
    return(-1);
  }

  // Turn off blocking for reads, use (fd, F_SETFL, FNDELAY) if you want that
  fcntl(fd, F_SETFL, 0);

  // Write to the port
  int n = write("Hello Peripheral\n");
  if (n < 0) {
    perror("Write failed - ");
    return -1;
  }

  // Read up to 255 characters from the port if they are there
  char buf[256];
  n = read(fd, (void*)buf, 255);
  if (n < 0) {
    perror("Read failed - ");
    return -1;
  } else if (n == 0) printf("No data on port\n");
  else {
    buf[n] = '\0';
    printf("%i bytes read : %s", n, buf);
  }

  // Don't forget to clean up
  close(fd);
  return 0;
}

