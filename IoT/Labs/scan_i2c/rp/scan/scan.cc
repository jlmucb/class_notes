#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <iostream>

// Manferdelli
// i2c scan

using namespace std;

#ifndef byte
typedef uint8_t byte;
#endif
const int measurementDelay=250;
const byte maxAddress= 127;

int main() {
  int num_devices = 0;

  wiringPiSetupGpio();
  printf("Scanning\n");
  for (byte r = 0; r < maxAddress; r++) {
    if (wiringPiI2CSetup(r) > 0) {
      printf("Device at %02x\n", r);
      num_devices++;
    }
    delay(measurementDelay);
   }
  printf("%d i2c devices found\n", num_devices);
  return 0;
}
