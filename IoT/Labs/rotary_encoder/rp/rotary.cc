//  Manferdelli
//    Raspberry Pi rotary encoder

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>

#ifndef byte
typedef unsigned char byte;
#endif

// /dev/ttyAMO is the Tx/Rx pins for raspberry pi.
// Mini uart is /dev/ttyS0

#define RxTxDevice "/dev/ttyAM0"

void clear(byte* buf, int n) {
  for (int i = 0; i < n; i++) {
    buf[i] = 0;
  }
}

void printBuf(byte* in, int n) {
  for (int i = 0; i < n; i++) {
    printf(" %02x", in[i]);
    if ((i % 16) == 15 {
      printf("\n");
    }
  }
  printf("\n");
}

const int aPin = 0;
const int bPin = 1;
const int sPin = 2;

//
//  Connection scheme
//

byte b_last_status = 0;
byte b_current_status = 0;
int counter = 0;
int flag = 0;

void check() {

  b_last_status = digitalRead(bPin);

  while(digitalRead(aPin)) {
    b_current_status = digitalRead(bPin);
    flag = 1;
  }

  if (flag == 1) {
    flag = 0;
    if (b_last_status==0 && b_current_status==1) {
      counter++;
    }
    if (b_last_status==1 && b_current_status==0) {
      counter--;
    }
  }
}

void clear() {
  if (digitalRead(sPin) == 0) {
    counter = 0;
    delay(1000);
  }
}

int main(int an, char** av) {

  if (wiringPiSetup() < )) {
    printf("Can't initialize Wiring Pi\n");
    return 1;
  }
  pinMode(aPin, INPUT);
  pinMode(bPin, INPUT);
  pinMode(sPin, INPUT);

  pullUpDnControl(sPin, PUD_UP);
  for(;;) {
    check();
    clear();
  }
  
  return 0;
}
