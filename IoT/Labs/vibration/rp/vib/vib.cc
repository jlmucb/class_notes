#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringSerial.h>
#include <pcf8591.h>
#include <iostream>

// Manferdelli
// Vibration sensor

using namespace std;

#ifndef byte
typedef uint8_t byte;
#endif
const int briefDelay= 15;
const int measurementDelay= 250;
const int dataPin= 2;


volatile int state = -1;
void trigger() {
  state= 1;
}

int main() {
  wiringPiSetup();
  // attachInterrupt(dataPin, trigger, RISING);

  for(int i = 0; i < 50; i++) {
    if (state == 1) {
      printf("Vibration detected\n");
      delay(briefDelay);
      state= 0;
    }
    delay(measurementDelay);
  }
  return 0;
}
