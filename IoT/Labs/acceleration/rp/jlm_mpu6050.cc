//  Manferdelli
//    Raspberry Pi, mpu-6050

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#ifndef byte
typedef unsigned char byte;
#endif

// /dev/ttyAMO is the Tx/Rx pins for raspberry pi.
// Mini uart is /dev/ttyS0
const byte i2c_address = 0x68;
const byte x_address= 0x3b;
const byte y_address = 0x3d;
const byte z_address = 0x3f;
const double pi = 3.1415926535;

// for L3G44200D Addreses are
//  control: 0x68
//  config: 0x23, put 0x30 in it
//  settings: 0x20, put 0xf in it (x,y,z) disable power down
//  xaddr: 0x28 lsb
//  xaddr: 0x29 msb
//  yaddr: 0x2a lsb
//  yaddr: 0x2b msb
//  zaddr: 0x2c lsb
//  zaddr: 0x2d msb

//  Connection scheme
//      RP          6050
//      gnd(HDR 6)  Gnd
//      scl(HDR 5)  scl
//      5v(HDR 1)   Vcc
//      sda(HDR 3)  sda

int read_int(int fd, byte address) {
  int r = 0;
  int v = 0;

  v =  wiringPiI2CReadReg8(fd, address);
  v <<= 8;
  v |=  wiringPiI2CReadReg8(fd, address + 1);
  if (v &0x8000)
    v = -(65536 - v);
  return v;
}

double dist(double a, double b) {
  return sqrt(a * a + b * b);
}

double get_x_rotation(double x, double y, double z) {
  double s = dist(x, z);
  if (s == 0.0) {
    if (y < 0.0)
      return -90.0;
    return 90.0;
  }
  double r = atan(y / s);
  return -(r * 180.0 / pi);
}

double get_y_rotation(double x, double y, double z) {
  double s = dist(y, z);
  if (s == 0.0) {
    if (x < 0.0)
      return -90.0;
    return 90.0;
  }
  double r = atan(x / s);
  return -(r * 180.0 / pi);
}

int main(int an, char** av) {

  int fd = wiringPiI2CSetup(i2c_address);
  if (fd < 0) {
    printf("Can't initialize Wiring Pi\n");
    return 1;
  }

  // disable sleep
  wiringPiI2CWriteReg8(fd, 0x6b, 0);
  printf("Control reg: %x\n", wiringPiI2CReadReg8(fd, 0x6b));

  int a_x= 0;
  int a_y= 0;
  int a_z= 0;
  double scaled_x, scaled_y, scaled_z;
  double rot_x, rot_y;
  for(;;) {
    a_x = read_int(fd, x_address);
    a_y = read_int(fd, y_address);
    a_z = read_int(fd, z_address);
    printf("Raw a_x: %d, a_y: %d, a_z: %d\n", a_x, a_y, a_z);
    scaled_x = ((double)a_x) / 16384.0;
    scaled_y = ((double)a_y) / 16384.0;
    scaled_z = ((double)a_z) / 16384.0;
    rot_x =  get_x_rotation(scaled_x, scaled_y, scaled_z);
    rot_y =  get_y_rotation(scaled_x, scaled_y, scaled_z);
    double total_g = sqrt(scaled_x*scaled_x+scaled_y*scaled_y+scaled_z*scaled_z);
    printf("Scaled acceleration: (%07.2f, %07.2f, %07.2f), x-y rotation: (%07.2f, %07.2f), total g: %7.3f\n",
      scaled_x, scaled_y, scaled_z, rot_x, rot_y, total_g);
    delay(1000);
  }

  return 0;
}
