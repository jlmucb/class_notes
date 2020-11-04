//  Manferdelli
//    Raspberry Pi EEPROM

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPiI2C.h>

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

const int i2c_address = 0x54;
const byte eeprom_address= 0x54;
const int measurementDelay= 500;
const unsigned maxMillis= 2000;

#define MAX_BYTES 64

// The 24LC256 has a 64-byte page write buffer but
// we'll write 16 at a time to be safe.
#define MAX_I2C_WRITE 16

// This address is determined by the way the address pins are wired.
// In the diagram from earlier, we connected A0 and A1 to Ground and
// A2 to 5V. To get the address, we start with the control code from
// the datasheet (1010) and add the logic state for each address pin
// in the order A2, A1, A0 (100) which gives us 0b1010100, or in
// Hexadecimal, 0x54.

// Read
byte readEEProm(int fd, unsigned address) {
  byte msb= address >> 8;
  byte lsb= address & 0xff;
  wiringPiI2CWriteReg8(fd, address, msb);
  wiringPiI2CWriteReg8(fd, address, lsb);
  byte r = wiringPiI2CReadReg8(fd, address);
  // send stop message
  return r;
}

// Write
void writeEEProm(int fd, unsigned address, byte* data, int size) {
  byte msb= address >> 8;
  byte lsb= address & 0xff;
  wiringPiI2CWriteReg8(fd, address, msb);
  wiringPiI2CWriteReg8(fd, address, lsb);
  for (int b = 0; b < size; b++)
    wiringPiI2CWriteReg8(fd, address, data[b]);
  // send stop message?
}

int initBuf(byte* buf, int max) {
  int i = 0;
  for (i = 0; i < 64; i++)
    buf[i] = (byte) (i + 16);
  return i;
}

//  Connection scheme
//    I2C connections (see analog), same chip connections as arduino

#define BUF_SIZE 64
#define MAX_I2C_WRITE 16

int main(int an, char** av) {

  int fd = wiringPiI2CSetup(eeprom_address);
  if (fd < 0) {
    printf("Can't initialize I2C interface\n");
    return 1;
  }

  int count = 0;
  byte to_write[BUF_SIZE];
  byte to_read[BUF_SIZE];
  unsigned reset= 0;
  int current = 0;
  int num_bytes= initBuf(to_write, BUF_SIZE);
  int num_left= num_bytes;
  int n;
  int end_address= BUF_SIZE;

  clearBuf(to_write, BUF_SIZE);
  clearBuf(to_read, BUF_SIZE);

  printf("Bytes to write:\n");
  printBuf(to_write, num_bytes);

  while(num_left > 0) {
    if (num_left >= MAX_I2C_WRITE)
      n = MAX_I2C_WRITE;
    else
      n = num_left;
    writeEEProm(fd, current, &to_write[current], n);
    num_left -= n;
    current += n;
  }
  printf("Write complete\n");

  // Read bytes to end_address
  byte v;
  for (int a = 0; a < end_address; a++) {
    to_read[a] = readEEProm(fd, a);
  }
  printf("\nBytes read:\n");
  printBuf(to_read, end_address);
    
  return 0;
}
