// Manferdelli
//   Rasperry Pi read hall sensor

#include <stdio.h>
#include <wiringPi.h>
#include <pcf8591.h>

const int base = 120;
const int i2c_address = 0x48;
// const int zero_field = 527;
const int zero_field = 7;  //this is better for 3.3v
const int measurement_delay = 500;

// PCF Connections
//    PCF         RP
//    1(ain0)     analog-signal
//    5, 6, 7, 8  gnd (pin 6)
//    16          3.3 (pin 1)
//    14          3.3v
//    13          gnd
//    12          gnd
//    10(scl)     5 (scl)
//    9(sda)      3 (sda)

int main(int an, char** av) {
  int strength = 0;

  if (wiringPiSetup() < 0) {
    printf("Can't initialize wiringPi\n");
    return 1;
  }
  pcf8591Setup(base, i2c_address);

  for(;;) {
    strength = analogRead(base);
    int calibrated_strength = strength - zero_field;
    printf("Strength: %d, calibrated measurement: %d\n", strength, calibrated_strength);
    delay(measurement_delay);
  }
  return 0;
}

