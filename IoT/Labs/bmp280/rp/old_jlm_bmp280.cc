//  Manferdelli
//    Raspberry Pi  bmp280

#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <bmp280.h>

#ifndef byte
typedef unsigned char byte;
#endif
#ifndef uint16 
typedef unsigned short int uint16;
#endif
#ifndef int8 
typedef char int8;
#endif

// this program is basically working but there is a library problem

void clear(byte* buf, int n) {
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

int fd = -1;

byte read_reg(byte i2c_addr, byte reg_addr, byte* data, uint16 len) {
  *data = 0;
  byte v =  wiringPiI2CReadReg8(fd, reg_addr);
  data[0] = v;
printf("read_reg %x %x %x %d\n", i2c_addr, reg_addr, *data, (int)len);
  return 0;
}

byte write_reg(byte i2c_addr, byte reg_addr, byte* data, uint16 len) {
printf("write_reg %x %x %x %d\n", i2c_addr, reg_addr, *data, len);
  wiringPiI2CWriteReg8(fd, reg_addr, *data);
  return 0;
}

void my_delay(unsigned t) {
  delay(1000);
}

//  Connection scheme
//    Same as I2C Analog

int main(int an, char** av) {
  struct bmp280_dev s;
  struct bmp280_config c;
  struct bmp280_uncomp_data u;
  byte result;
  double temperature, pressure;

  fd = wiringPiI2CSetup(BMP280_I2C_ADDR_PRIM);
  if (fd < 0) {
    printf("Can't initialize i2c interface %d\n", result);
    return 1;
  }

  // set call-backs
  s.delay_ms = (bmp280_delay_fptr_t) my_delay;
  s.dev_id = BMP280_I2C_ADDR_PRIM;
  s.intf = BMP280_I2C_INTF;
  s.read = (bmp280_com_fptr_t) read_reg;
  s.write = (bmp280_com_fptr_t) write_reg;

  result = bmp280_init(&s);
  if (result != BMP280_OK) {
    printf("Can't init BMP280 %d\n", result);
    return 1;
  }

  // temperature
  delay(1000);
  result = bmp280_get_config(&c, &s);
  if (result != BMP280_OK) {
    printf("Can't get config BMP280\n");
    return 1;
  }
  c.filter = BMP280_FILTER_COEFF_2;
  c.os_temp = BMP280_OS_4X;
  c.os_pres = BMP280_OS_4X;
  c.odr = BMP280_ODR_1000_MS;

  delay(1000);
  result = bmp280_set_config(&c, &s);
  if (result != BMP280_OK) {
    printf("Can't set config BMP280\n");
    return 1;
  }
#if 0
  result = bmp280_set_power_mode(BMP280_NORMAL_MODE, &s);
  if (result != BMP280_OK) {
    printf("Can't set BMP280 power mode\n");
    return 1;
  }
#endif

  for(int i = 0; i < 2; i++) {
    delay(100);
    result = bmp280_get_uncomp_data(&u, &s);
    if (result != BMP280_OK) {
      printf("Can't get uncompensated data %x\n", result);
      continue;
    }
    result = bmp280_get_comp_temp_double(&temperature, u.uncomp_temp, &s);
    if (result != BMP280_OK) {
      printf("Can't get compensated temperature %x\n", result);
      continue;
    }
#if 0
    result = bmp280_get_comp_pres_double(&pressure, u.uncomp_press, &s);
    if (result != BMP280_OK) {
      printf("Can't get compensated pressure %x\n", result);
      continue;
    }
    printf("Temperature: %7.2f, pressure: %8.4f\n", temperature, pressure);
#endif
    printf("Temperature: %7.2f\n", temperature);
  }
  return 0;

}
