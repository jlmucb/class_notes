#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pcf8591.h>

// Manferdelli
// Wetness sensor

using namespace std;

#ifndef byte
typedef uint8_t byte;
#endif

const int measurementDelay= 500;
const int base = 120;
const int i2c_address = 0x48;

int main(void) {
  if (wiringPiSetup() < 0) {
    printf("Can't initialize wiringPi\n");
    return 1;
  }
  pcf8591Setup(base, i2c_address);
  delay(measurementDelay);
  int wetness = analogRead(base);
  printf("Wetness: %d\n", wetness);
  return 0;
}

