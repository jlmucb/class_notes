#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

// turn LED on and off
// Manferdelli

const int switchPin = 0;
const int measurementDelay = 500;
const int loopCount = 250;

int main(int an, char** av) {
  wiringPiSetup ();
  pinMode (switchPin, OUTPUT);
  for (int i = 0; i < loopCount; i++) {
    digitalWrite(switchPin, HIGH); 
    delay(measurementDelay);
    digitalWrite(switchPin, LOW); 
    delay(measurementDelay);
  }
  return 0;
}

