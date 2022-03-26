// Manferdelli, HC-12

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#ifdef PIN_ACCESS
#include <wiringPi.h>
#endif

#ifndef byte
typedef unsigned char byte;
#endif

// HC12 Connections
//    HC12  FDMI1232  RP
//    vcc   vcc       vcc
//    gnd   gnd       gnd
//    rx    tx        tx (pin 10)
//    tx    rx        rx (pin 8)
//    set   NC        set (pin 7)

#ifdef PIN_ACCESS
#define uartDevice "/dev/serial0"
#else
#define uartDevice "/dev/ttyUSB0"
#endif
#define BUF_SIZE 512
const int set_pin = 7;

const char* eol = "\r\n";
void send_command(int fd, const char* command) {
  for (int i = 0; i < strlen(command); i++)
    write(fd, command, strlen(command));
  write(fd, eol, strlen(eol));
}

bool setup(int fd, int new_baud_rate, int new_channel) {
#ifdef PIN_ACCESS
  int n = 0;
  char request[32];
  char response[32];

  pinMode(set_pin, OUTPUT);

  // command mode
  digitalWrite(set_pin, LOW);
  delay(200);

#ifdef TEST_CMD
  // test command, hc12 should return "OK"
  memset(request, 0, 32);
  memset(response, 0, 32);

  send_command(fd, "AT");
  n = read(n, response, 31);
  if (n > 0) {
    printf("AT test: %s\n", (char*)response);
  }
#endif

  // baud rate
  delay(200);
  memset(request, 0, 32);
  memset(response, 0, 32);
  sprintf(request, "AT_B%04d", new_baud_rate);
  send_command(fd, (char*)request);

  // channel command
  memset(request, 0, 32);
  memset(response, 0, 32);
  delay(200);
  sprintf(request, "AT_C%03d", new_channel);
  send_command(fd, (char*)request);
  n = read(n, response, 31);
  if (n > 0) {
    printf("AT channel select: %s\n", (char*)response);
  }

  // back to transparent mode
  digitalWrite(set_pin, HIGH);
  delay(200);
#endif
  return true;
}

int bytes_available(int fd) {
  int num_bytes = 0;
  ioctl(fd, FIONREAD, &num_bytes);
  return num_bytes;
}

int main(int an, char** av) {
  byte receive_buf[BUF_SIZE];
  byte send_buf[BUF_SIZE];

#ifdef PIN_ACCESS
  if (wiringPiSetup() < 0) {
    printf("Can't init wiringPi\n");
    return 1;
  }
#endif

  speed_t new_baud_rate = 9600;
  char* device_name = (char*)"Device 1";
  int new_channel = 0;
  char* uart_device = (char*)uartDevice;
  bool transmit = true;
  for (int i = 1; i < an; i++) {
    if (strcmp(av[i], "-receive") == 0) {
      transmit = false;
    }

    if (strcmp(av[i], "-uart_device") == 0) {
      uart_device= av[++i];
      continue;
    }

    if (strcmp(av[i], "-baud") == 0) {
      unsigned ls = 0;
      ls = atoi(av[++i]);
      new_baud_rate = ls;
      continue;
    }
    if (strcmp(av[i], "-channel") == 0) {
      unsigned ls = 0;
      ls = atoi(av[++i]);
      new_channel = ls;
      continue;
    }
    if (strcmp(av[i], "-device_name") == 0) {
      device_name = av[++i];
      continue;
    }
  }

  int fd = open(uart_device, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    printf("Can't open %s\n", uart_device);
    return 1;
  }

  // turn off blocking for reads.
  fcntl(fd, F_SETFL, 0); //fcntl(fd, F_GETFL)& ~O_NONBLOCK);

  // set baud rate
  struct termios t;
  tcgetattr(fd, &t);
  cfsetispeed(&t, new_baud_rate);
  tcsetattr(fd, TCSANOW, &t);

  // setup hc-12, if we can set the set pin
  setup(fd, new_baud_rate, new_channel);
  int in_size, out_size;

  if (transmit)
    printf("\n%s transmitting**\n\n", device_name);
  else
    printf("\n%s receiving\n\n", device_name);
  for(int i = 0;; i++) {
    if (!transmit) {
      while (bytes_available(fd) == 0);
      memset(receive_buf, 0, BUF_SIZE);
      while (bytes_available(fd) > 0) {
        in_size = read(fd, receive_buf, BUF_SIZE - 1);
        receive_buf[in_size++] = 0;
        printf("%s", receive_buf);
      }
      if (i > 20)
        break;
    } else {
      memset(send_buf, 0, BUF_SIZE);
      sprintf((char*)send_buf, "%s says Message %d\n", device_name, i);
      out_size = strlen((char*)send_buf);
      write(fd, send_buf, out_size);
      printf("%s, sent: %s\n", device_name, (char*)send_buf);
    }
#ifdef PIN_ACCESS
    delay(200);
else
    sleep(1);
#endif
  }

  close(fd);
  return 0;
}


