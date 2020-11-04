//  Manferdelli
//    Raspberry Pi  bmp280

#include <jlm_bmp280lib.h>

void my_delay(unsigned t) {
  delay(1000);
}

//  Connection scheme
//    Same as I2C Analog

const int bmp280_i2c_address = 0x76;

int main(int an, char** av) {
  bmp280 state;
  double temperature, pressure;

  state.i2c_address_ = bmp280_i2c_address;
  int fd = wiringPiI2CSetup(bmp280_i2c_address);
  if (fd < 0) {
    printf("Can't initialize i2c interface\n");
    return 1;
  }

  if (!bmp280_init(fd, &state)) {
    printf("Can't init BMP280\n");
    return 1;
  }

  // On 12/29/2019 (it was raining)
  const double pressure_sfo = 1013.2;
  const double alt_sfo = 30.10 / 39.37;
  double sea_level_pressure_sfo = 1018.0; //1013.0 sealevel_pressure(alt_sfo, pressure_sfo);
  double alt = 0.0;
  printf("Sea level pressure: %8.4lf (Pa)\n", sea_level_pressure_sfo);

  for(int i = 0; i < 10; i++) {
    double temperature, pressure;

    printf("\n");
    if(!read_temperature(fd, state, &temperature)) {
      printf("Can't get temperature\n");
      continue;
    }
    if(!read_pressure(fd, state, &pressure)) {
      printf("Can't get pressure\n");
      continue;
    }
    alt = calc_altitude(sea_level_pressure_sfo, pressure);
    printf("Temperature: %7.2lf (C), pressure: %8.4lf (Pa), alt: %8.4lf (m)\n",
       temperature, pressure, alt);
    delay(2000);
  }
  return 0;
}
