// Manferdelli
//   Rasperry Pi analog read

#include <stdio.h>
#include <wiringPi.h>
#include <pcf8591.h>

const int base = 120;
const int i2c_address = 0x48;
const int measure_delay = 200;
const double volt_scale = 3.3;
const int int_scale = 255;

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
//
//  Breakout board connections are
//    SDA, SCL, AIN0 (Not Aout!)

int main(int an, char** av) {
  int v = 0;

  if (wiringPiSetup() < 0) {
    printf("Can't initialize wiringPi\n");
    return 1;
  }
  pcf8591Setup(base, i2c_address);

  for(;;) {
    delay(measure_delay);
    v = analogRead(base);
    printf("Voltage: %d units, %5.2f volts (scale: %5.3f volts)\n", v,
      volt_scale*(((double)v)/((double)int_scale)), volt_scale);
  }
  return 0;
}

