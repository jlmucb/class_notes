// Manferdelli
//  Raspberry Pi interface to infrared sensor

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>

#ifndef byte
typedef unsigned char byte;
#endif

void clearBuf(byte* buf, int n) {
  for (int i = 0; i < n; i++) {
    buf[i] = 0;
  }
}

const int ir_hdr_pin = 7;
const int messageDelay = 1000;
const int max_counter = 255;
#define DATA_BUF_SIZE 10

/*
  IR Protocol
    Uses pulse distance encoding for bits
        0: 562.5µs pulse burst followed by a 562.5µs space
        1: 562.5µs pulse burst followed by a 1.6875ms space
    IR message consists of:
        a 9ms leading pulse burst 
        a 4.5ms space
        the 8-bit address of receiving device followed by its 8-bit logical 
        the 8-bit command followed by its 8-bit logical inverse
        a final 562.5µs pulse burst.

  Note:: pulses are inverted.
 */

const int space_interval = 2000;

void test() {
  int  c;
  byte last = HIGH;
  byte now;

  for(int j = 0; j < 80; j++) {
    c = 0;
    for(;;) {
      now = digitalRead(ir_hdr_pin);
      if (last != now) {
        printf("val: %d, count: %d\n", last, c);
        last = now;
        break;
      }
      c++;
      delayMicroseconds(1);
    }
  }
}

bool got_attention_sequence() {
  while(digitalRead(ir_hdr_pin) == HIGH)
    delayMicroseconds(2);
    // Expect
    //   9ms leading high pulse burst 
    //   4.5ms low pulse burst

  int on_counter = 0;
  for(int i = 0; i < space_interval; i++) {
    if (digitalRead(ir_hdr_pin) == HIGH)
      break;
    on_counter++;
    delayMicroseconds(2);
  }
printf("on_counter %d\n", on_counter);
  if (on_counter < 4000)
    return false;

  int off_counter = 0;
  for(int i = 0; i < space_interval; i++) {
    if (digitalRead(ir_hdr_pin) == LOW)
      break;
    delayMicroseconds(2);
    off_counter++;
  }
printf("off_counter %d\n", off_counter);
  if (on_counter < 5000 && on_counter > 4000 && off_counter < 3000 && off_counter > 1500)
    return true;
  return false;
}

const int max_in_state = 1000;
int get_ir_bits(byte* data, int num_bits_to_get, int size_buf) {
printf("get_ir_bits\n");

  if (size_buf < (num_bits_to_get+7/8))
    return -1;

  int on_counter = 0;
  int off_counter = 0;
  int i = 0;
  int j = 0;
  byte b = 0;
  byte bit;

  for (i = 0; i < num_bits_to_get; i++) {
    on_counter = 0;
    off_counter = 0;
    while(digitalRead(ir_hdr_pin) == HIGH) {
      on_counter++;
      delayMicroseconds(2);
      if (on_counter == max_in_state)
        return -1;
    }
    while(digitalRead(ir_hdr_pin) == LOW) {
      off_counter++;
      delayMicroseconds(2);
      if (off_counter == max_in_state)
        return -1;
    }

    // 0: 562.5µs pulse burst followed by a 562.5µs space
    // 1: 562.5µs pulse burst followed by a 1.6875ms space
    if (off_counter > (on_counter + 100))
      bit = 0;
    else
      bit = 1;
    b |= (bit << j);
    j++;
    if (j == 8) {
      data[i/8] = b;
      j = 0;
      b = 0;
    }
  }
  if (j > 0)
    data[(i+7)/8] = b;
  return i;
}


// expect attention sequence followed by
// the 8-bit address of receiving device followed by its 8-bit logical inverse
// the 8-bit command followed by its 8-bit logical inverse
bool get_ir_message(byte* address, byte* command) {
  byte t[4];
  clearBuf(t,4);

printf("get_ir_message\n");
  while(!got_attention_sequence());

  int n = get_ir_bits(t, 32, 4);
  if (n < 32)
    return false;

  // make sure inverses are right
  if (t[0] != (0xff^t[1])) {
    printf("address inverse is wrong\n");
  }
  if (t[2] != (0xff^t[3])) {
    printf("command inverse is wrong\n");
  }
  *address = t[0];
  *command = t[2];
  return true;
}

// Needs to be run as su

int main(int an, char** av) {
  if (wiringPiSetup() < 0) {
    printf("Can't initialize wiringPi\n");
    return 1;
  }

test();
return 0;
  byte a, b;
  for(;;) {
    a = 0;
    b = 0;
    if (get_ir_message(&a, &b)) {
      printf("address: %02x, command: %02x\n", a, b);
    }
    delay(messageDelay);
  }

  return 0;
}
