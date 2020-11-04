//  Manferdelli
//    Raspberry Pi SPI

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPiSPI.h>

#ifndef byte
typedef unsigned char byte;
#endif

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

const int chip_select = 1;
const int bus_speed = 50000;
#define BUF_SIZE 128

//  RP SPI connections
//    RP      SPI
//    Gnd     Gnd
//    3.3V    Vcc
//    CE1     SS (shift select)
//    SCK     SCK
//    MOSI    SDI
//    MISO    SDO

int main(int an, char** av) {
  int fd = wiringPiSPISetup(chip_select, bus_speed);
  if (fd < 0) {
    printf("Can't initialize SPI interface\n");
    return 1;
  }

  int result;
  byte buf[BUF_SIZE];

  // clear display
  buf[0] = 0x76;
  result = wiringPiSPIDataRW(chip_select, buf, 1);
  if (result < 0) {
    printf("Initial clear failed\n");
  }
  sleep(5);

  // Are we addressing all segments
  for (int i = 1; i <= 0x7f; i++) {

    buf[0] = 0x77;
    buf[1] = i;
    result = wiringPiSPIDataRW(chip_select, buf, 2);
    if (result < 0) {
      printf("First R/W failed\n");
    }

    buf[0] = 0x7b;
    buf[1] = i;
    result = wiringPiSPIDataRW(chip_select, buf, 2);
    if (result < 0) {
      printf("Second R/W failed\n");
    }

    buf[0] = 0x7c;
    buf[1] = i;
    result = wiringPiSPIDataRW(chip_select, buf, 2);
    if (result < 0) {
      printf("Third R/W failed\n");
    }

    buf[0] = 0x7d;
    buf[1] = i;
    result = wiringPiSPIDataRW(chip_select, buf, 2);
    if (result < 0) {
      printf("Fourth R/W failed\n");
    }

    buf[0] = 0x7e;
    buf[1] = i;
    result = wiringPiSPIDataRW(chip_select, buf, 2);
    if (result < 0) {
      printf("Fifth R/W failed\n");
    }

    sleep(5);
  }

  // clear display
  buf[0] = 0x76;
  result = wiringPiSPIDataRW(chip_select, buf, 1);
  if (result < 0) {
    printf("Final clear failed\n");
  }
  return 0;
}
