//  Manferdelli
//    Raspberry Pi, i2c-lcd

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#ifndef byte
typedef unsigned char byte;
#endif

const byte lcd_i2c_address = 0x27;

//  Connection 
//    RP            i2c IF
//    Gnd(HDR 6)    Gnd
//    scl(HDR 5)    scl
//    5v(HDR 2)     Vcc
//    sda(HDR 3)    sda
void write_int(int fd, int data) {
  data |= 0x08;
  wiringPiI2CWrite(fd, data);
  return;
}

int read_int(int fd) {
  return wiringPiI2CRead(fd);
}

void send_command(int fd, int c) {
  int t;

  // RS=0, RW=0, EN=1
  t = (c & 0xf0) | 0x04;
  write_int(fd, t);
  delay(2);
  // EN = 0
  t &= 0xfb;
  write_int(fd, t);

  // RS=0, RW=0, EN=1
  t = ((c & 0x0f) << 4) | 0x04;
  write_int(fd, t);
  delay(2);
  // EN = 0
  t &= 0xfb;
  write_int(fd, t);
}

void send_data(int fd, int d) {
  int t;

  // RS=1, RW=0, EN=1
  t = (d & 0xf0) | 0x05;
  write_int(fd, t);
  delay(2);
  // EN = 0
  t &= 0xfb;
  write_int(fd, t);

  // RS=1, RW=0, EN=1
  t = ((d & 0x0f) << 4) | 0x05;
  write_int(fd, t);
  delay(2);
  // EN = 0
  t &= 0xfb;
  write_int(fd, t);
}

bool lcd_init(int fd) {
  // 8 line
  send_command(fd, 0x33);
  delay(5);
  // 4 line
  send_command(fd, 0x32);
  delay(5);
  // 2 line
  send_command(fd, 0x28);
  // display enable
  delay(5);
  send_command(fd, 0x0c);
  // clear
  send_command(fd, 0x01);
  delay(5);
  wiringPiI2CWrite(fd, 0x08);
  return true;
}

void write_line(int fd, int line , int position, const char* data) {
  if (line < 0 || line > 1)
    return;
  if (position < 0 || line > 15)
    return;
  int len = strlen(data);
  if (len > 15)
    return;

  int addr = 0x40 * line + 0x80 + position;
  printf("write_line %d %d, %s\n", line, position, data);
  send_command(fd, addr);
  for (int i = 0; i < len; i++) {
    send_data(fd, data[i]);
  }
}

void clearBuf(byte* buf, int n) {
  for (int i = 0; i < n; i++) {
    buf[i] = 0;
  }
}

int main(int an, char** av) {

  int fd = wiringPiI2CSetup(lcd_i2c_address);
  if (fd < 0) {
    printf("Can't initialize Wiring Pi\n");
    return 1;
  }
  if (!lcd_init(fd)) {
    printf("Can't initialize LCD\n");
    return 1;
  }

  char buf[16];

  for(int j = 0; j < 20; j++) {
    send_command(fd, 0x01);
    delay(5);
    write_line(fd, 0, 0, "Line 0");
    clearBuf((byte*)buf, 16);
    sprintf(buf, "Run %d", j);
    write_line(fd, 1, 1, buf);
    delay(5000);
  }

  return 0;
}
