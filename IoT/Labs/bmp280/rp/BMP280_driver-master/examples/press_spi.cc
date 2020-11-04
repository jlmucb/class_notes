#include "stdio.h"
#include "bmp280.h"

void delay_ms(uint32_t period_ms);
int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t spi_reg_write(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t spi_reg_read(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
void print_rslt(const char api_name[], int8_t rslt);

int main(void) {
  int8_t rslt;
  struct bmp280_dev bmp;
  struct bmp280_config conf;
  struct bmp280_uncomp_data ucomp_data;
  uint32_t pres32, pres64;
  double pres;

  // Map the delay function pointer with the function responsible for implementing the delay
  bmp.delay_ms = delay_ms;
  // Assign device I2C address based on the status of SDO pin (GND for PRIMARY(0x76) & VDD for SECONDARY(0x77))
  bmp.dev_id = BMP280_I2C_ADDR_PRIM;
  // Select the interface mode as I2C
  bmp.intf = BMP280_I2C_INTF;
  // Map the I2C read & write function pointer with the functions responsible for I2C bus transfer
  bmp.read = i2c_reg_read;
  bmp.write = i2c_reg_write;

  rslt = bmp280_init(&bmp);
  print_rslt(" bmp280_init status", rslt);
  rslt = bmp280_get_config(&conf, &bmp);
  print_rslt(" bmp280_get_config status", rslt);

  // configuring the temperature oversampling, filter coefficient and output data rate
  // Overwrite the desired settings */
  conf.filter = BMP280_FILTER_COEFF_2;
  // Pressure oversampling set at 4x
  conf.os_pres = BMP280_OS_4X;
  // Setting the output data rate as 1HZ(1000ms)
  conf.odr = BMP280_ODR_1000_MS;
  rslt = bmp280_set_config(&conf, &bmp);
  print_rslt(" bmp280_set_config status", rslt);
  // Always set the power mode after setting the configuration
  rslt = bmp280_set_power_mode(BMP280_NORMAL_MODE, &bmp);
  print_rslt(" bmp280_set_power_mode status", rslt);
  while (1) {
    rslt = bmp280_get_uncomp_data(&ucomp_data, &bmp);
    rslt = bmp280_get_comp_pres_32bit(&pres32, ucomp_data.uncomp_press, &bmp);
    rslt = bmp280_get_comp_pres_64bit(&pres64, ucomp_data.uncomp_press, &bmp);
    rslt = bmp280_get_comp_pres_double(&pres, ucomp_data.uncomp_press, &bmp);
    printf("UP: %ld, P32: %ld, P64: %ld, P64N: %ld, P: %f\r\n", ucomp_data.uncomp_press,
         pres32, pres64, pres64 / 256, pres);
    bmp.delay_ms(1000); /* Sleep time between measurements = BMP280_ODR_1000_MS */
  }
  return 0;
}

int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length) {
  return -1;
}

int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length) {
  return -1;
}

void print_rslt(const char api_name[], int8_t rslt) {
  if (rslt != BMP280_OK) {
    printf("%s\t", api_name);
    if (rslt == BMP280_E_NULL_PTR) {
      printf("Error [%d] : Null pointer error\r\n", rslt);
    } else if (rslt == BMP280_E_COMM_FAIL) {
      printf("Error [%d] : Bus communication failed\r\n", rslt);
    } else if (rslt == BMP280_E_IMPLAUS_TEMP) {
      printf("Error [%d] : Invalid Temperature\r\n", rslt);
    } else if (rslt == BMP280_E_DEV_NOT_FOUND) {
      printf("Error [%d] : Device not found\r\n", rslt);
    } else {
      /* For more error codes refer "*_defs.h" */
      printf("Error [%d] : Unknown error code\r\n", rslt);
    }
  }
}
