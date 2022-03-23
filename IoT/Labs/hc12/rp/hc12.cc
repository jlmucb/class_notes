// Manferdelli, HC-12

#define PIN_ACCESS
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

// HC12 Interfacing with RP Tx and Rx pins
// Connections
//    RP            HC12
//    vcc           vcc
//    gnd           gnd
//    rxd (pin 10)  txd
//    txd (pin  8)  rxd

//    RP            HC12
//    vcc           vcc
//    gnd           gnd
//    rxd (pin 10)  txd
//    txd (pin  8)  rxd

#define uartDevice "/dev/serial0"
#define BUF_SIZE 512
const int set_pin = 7;

void clearBuf(byte* buf, int n) {
  for (int i = 0; i < n; i++) {
    buf[i] = 0;
  }
}

int bytes_available(int fd) {
  int num_bytes = 0;
  ioctl(fd, FIONREAD, &num_bytes);
  return num_bytes;
}

const char* eol = "\r\n";
void send_command(int fd, const char* command) {
  byte response[128];

  printf("Command: %s\n", command);

  // command mode
  digitalWrite(set_pin, LOW);
  delay(200);

  tcflush(fd, TCIFLUSH);
  write(fd, command, strlen(command));
  int n = 0;
  delay(200);
  while (bytes_available(fd) > 0) {
    clearBuf(response, 128);
    n = read(fd, response, 127);
    if (n > 1) {
      printf("Response: %s", (char*)response);
    }
    tcflush(fd, TCIFLUSH);
  }

  digitalWrite(set_pin, HIGH);
  delay(200);
}

bool setup(int fd, int new_baud_rate, int new_channel) {
#ifdef PIN_ACCESS
  int n = 0;
  byte request[128];

  pinMode(set_pin, OUTPUT);

  // test command, hc12 should return "OK"
  clearBuf(request, 128);
  send_command(fd, "AT");

  // baud rate
  clearBuf(request, 128);
  sprintf((char*)request, "AT+B%04d", new_baud_rate);
  send_command(fd, (char*)request);

  // channel command
  clearBuf(request, 128);
  sprintf((char*)request, "AT+C%03d", new_channel);
  send_command(fd, (char*)request);

  // back to transparent mode
  digitalWrite(set_pin, HIGH);
  delay(200);
#endif
  return true;
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

  int fd = open(uartDevice, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    printf("Can't open %s\n", uartDevice);
    return 1;
  }

  speed_t new_baud_rate = 9600;
  int new_channel = 6;
  for (int i = 0; i < (an - 1); i++) {
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
  }
  if (new_channel < 1 || new_channel>100)
    new_channel = 1;
  // turn off blocking for reads.
  fcntl(fd, F_SETFL, 0); //fcntl(fd, F_GETFL)& ~O_NONBLOCK);

  // set baud rate
  struct termios t;
  tcgetattr(fd, &t);
  cfsetispeed(&t, new_baud_rate);
  tcsetattr(fd, TCSANOW, &t);

  // setup hc-12
  // We need pin access for this
  setup(fd, new_baud_rate, new_channel);
  int in_size, out_size;

  tcflush(fd, TCIFLUSH);
  for(int i = 0; i < 5; i++) {
    while (bytes_available(fd) > 0) {
      clearBuf(receive_buf, BUF_SIZE);
      in_size = read(fd, receive_buf, BUF_SIZE - 1);
      receive_buf[in_size++] = 0;
      printf("Received: %s", (const char*)receive_buf);
    }
    clearBuf(send_buf, BUF_SIZE);
    sprintf((char*)send_buf,"Message %d", i);
    out_size = strlen((char*)send_buf);
    write(fd, send_buf, out_size);
    printf("Sent: %s\n", (char*)send_buf);
#ifdef PIN_ACCESS
    delay(200);
#else
    sleep(1);
#endif
  }

  close(fd);
  return 0;
}

