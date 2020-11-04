#ifndef _JLMBMP280LIB_H_
#define _JLMBMP280LIB_H_

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#ifndef byte
typedef unsigned char byte;
#endif
#ifndef uint16 
typedef unsigned short int uint16;
#endif
#ifndef int16 
typedef short int int16;
#endif
#ifndef int8 
typedef char int8;
#endif

const byte REG_T1 = 0x88;
const byte REG_T2 = 0x8a;
const byte REG_T3 = 0x8c;
const byte REG_P1 = 0x8e;
const byte REG_P2 = 0x90;
const byte REG_P3 = 0x92;
const byte REG_P4 = 0x94;
const byte REG_P5 = 0x96;
const byte REG_P6 = 0x98;
const byte REG_P7 = 0x9a;
const byte REG_P8 = 0x9c;
const byte REG_P9 = 0x9e;
const byte CHIP_ID = 0xd0;
const byte REG_SOFT_RESET = 0xe0;
const byte REG_CALIBRATION = 0xe1;
const byte REG_STATUS = 0xf3;
const byte REG_CONTROL = 0xf4;
const byte REG_CONFIG = 0xf5;
const byte REG_PRESSURE_DATA = 0xf7;
const byte REG_TEMP_DATA = 0xfa;

const byte MODE_NORMAL = 0x03;
const byte MODE_SOFT_RESET_CODE = 0xB6;

const byte FILTER_OFF = 0x00;
const byte FILTER_X1 = 0x01;
const byte FILTER_X2 = 0x02;
const byte FILTER_X4 = 0x03;
const byte FILTER_X8 = 0x04;
const byte FILTER_X16 = 0x05;

const byte SAMPLING_X1 = 0x01;
const byte SAMPLING_X2 = 0x02;
const byte SAMPLING_X4 = 0x03;
const byte SAMPLING_X8 = 0x04;
const byte SAMPLING_X16 = 0x05;

const byte ODR_1000_MS = 0x05;


class calibration_data {
public:
  uint16 t1_;
  int16 t2_;
  int16 t3_;
  uint16 p1_;
  int16 p2_;
  int16 p3_;
  int16 p4_;
  int16 p5_;
  int16 p6_;
  int16 p7_;
  int16 p8_;
  int16 p9_;
  int fine_;
};

class config_data {
public:
  byte standby_;
  byte mode_;
  byte filter_;
  byte samp_temp_;
  byte samp_press_;
  byte odr_;
};

class bmp280 {
public:
  int i2c_address_ = 0x76;
  byte  chip_id_;
  calibration_data cal_data_;
  config_data conf_data_;
};

bool read_reg(int fd, byte reg_addr, byte* data);
bool write_reg(int fd, byte reg_addr, uint16* data);
bool read_word(int fd, byte reg_addr, uint16* data);
bool write_word(int fd, byte reg_addr, uint16* data);
void clear(byte* buf, int n);
void printBuf(byte* in, int n);
bool get_calibration_data(int fd, calibration_data* d);
bool set_config_data(int fd, config_data& d);
bool get_config_data(int fd, config_data* d);
bool get_temperature(int fd, bmp280& state, double* temperature);
bool get_pressure(int fd, bmp280& state, double* pressure);
bool init_config_data(config_data* d);
double sealevel_altitude(double alt, double atm);
void print_calibration_data(calibration_data& d);
void print_configuration_data(config_data& d);
bool soft_reset(int fd);
bool bmp280_init(int fd, bmp280* s);
bool read_temperature(int fd, bmp280& s, double* temp);
bool read_pressure(int fd,  bmp280& s, double* press);
double calc_altitude(double sealevelPressure, double measuredPressure);
double sealevel_pressure(double alt, double atm_press);
#endif


