// Manferdelli
//  Raspberry Pi interface to dht11

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

void printBuf(byte* in, int n) {
  for (int i = 0; i < n; i++) {
    printf(" %02x", in[i]);
    if ((i % 16) == 15) {
      printf("\n");
    }
  }
  printf("\n");
}

const int dht_pin = 7;
const int timingDelay = 18;
const int messageDelay = 2000;
const int max_timings = 90;
const int bits_to_read = 40;
#define DATA_BUF_SIZE 10

class temp_sensor {
public:
  double h_;
  double c_;
  double f_;
};

bool get_dht_data(byte* data, int size, temp_sensor* r) {
  if (size < 5)
    return false;

  byte last_state = HIGH;
  int counter = 0;
  int i = 0;
  int j = 0;

  pinMode(dht_pin, OUTPUT);
  digitalWrite(dht_pin, LOW);
  delay(timingDelay);
  pinMode(dht_pin, INPUT);

  // Detect change
  for (i = 0; i < max_timings; i++) {
    counter = 0;
    while(digitalRead(dht_pin) == last_state) {
      counter++;
      delayMicroseconds(2);
      if (counter == 255)
        break;
    }
    last_state = digitalRead(dht_pin);

    // ignore first 3 transitions
    if (i>=4 && (i%2)==0) {
      data[j/8] <<= 1;
      if (counter > 16)
        data[j/8] |= 1;
      j++;
    } 
  }

  // Make sure we read 40 bits and verify checksum
  if (j < bits_to_read)
    return false;

  byte check_sum = data[0] + data[1] + data[2] + data[3];
  if (check_sum != data[4])
    return false;

  // calculate readings
  int t;

  // humidity
  t = data[0];
  t <<= 8;
  t += data[1];
  r->h_ = ((double)t)/10.0;
  if (r->h_ > 100.0)
    r->h_ = ((double)data[0]);
 
  // temperature 
  t = data[2] & 0x7f;
  t <<= 8;
  t += data[3];
  r->c_ = ((double)t) / 10.0;
  if (r->c_ > 125)
    r->c_ = ((double) data[2]);
  if (data[2] & 0x80)
    r->c_ = -r->c_;
  r->f_ = 1.8 * r->c_ + 32.0;

  return true;
}

// Needs to be run as su

int main(int an, char** av) {
  if (wiringPiSetup() < 0) {
    printf("Can't initialize wiringPi\n");
    return 1;
  }

  temp_sensor r;
  byte data[DATA_BUF_SIZE];
  for(;;) {
    clearBuf(data, DATA_BUF_SIZE);
    if (get_dht_data(data, DATA_BUF_SIZE, &r))
      printf("Humidity: %7.3lf, Temperature: %7.3lf (C), %7.3lf (F)\n",
        r.h_, r.c_, r.f_);
    delay(messageDelay);
  }

  return 0;
}
