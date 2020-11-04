// Manferdelli
//   Rasperry Pi analog write

#include <stdio.h>
#include <wiringPi.h>
#include <pcf8591.h>

const int base = 120;
const int i2c_address = 0x48;
// const int pin_mode = PWM_OUTPUT;
// const int outPin = 1;

int main(int an, char** av) {
  int v = 0;

  if (wiringPiSetup() < 0) {
    printf("Can't initialize raspberry pi library\n");
    return 1;
  }
  pcf8591Setup(base, i2c_address);

  for(;;) {
    printf("Output: %d\n", v * 5);
    analogWrite(base, v);
    delay(10);
    v = 1 - v;
  }
  return 0;
}

