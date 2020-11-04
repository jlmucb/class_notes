//  Manferdelli
//    Raspberry Pi, ping sensor

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>

#ifndef byte
typedef unsigned char byte;
#endif

//  Connections
//    Ping        RP
//    trigger     gpio 7 (pin 7)
//    echo        gpio 27 (pin 34)
//    vcc         5v
//    gnd         gnd

// I had to use a scope to check these
// RP pin numbering is mysterious.
// Look at the ifdef'd code below to see it.
const int trigger_pin = 7;
const int echo_pin = 27;
const int max_distance = 500; // cm
long unsigned time_out_period;
const double sound_speed = 340.0; // speed of sound (m/s)

long pulseIn(int pin, int value, int time_out) {

  pinMode(echo_pin, INPUT);
  long unsigned start = micros();
  while(digitalRead(pin) != value) {
    if ((micros() - start) > time_out_period)
      return 0;
  }
  long unsigned set = micros();
  while(digitalRead(pin) == value) {
    if ((micros() - set) > time_out_period)
      return 0;
  }
  long unsigned end = micros();
printf("pulseIn, end: %d, start: %d\n", end,start);
  return end - set;
}

double getDistance() {
#if 1
  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, OUTPUT);
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);
#else
  // Find pins
  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, OUTPUT);
  for(;;) {
    digitalWrite(trigger_pin, LOW);
    delayMicroseconds(1000);
    digitalWrite(trigger_pin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(echo_pin, LOW);
    delayMicroseconds(1000);
    digitalWrite(echo_pin, HIGH);
    delayMicroseconds(1000);
}
#endif

  long unsigned ping_time = pulseIn(echo_pin, HIGH, time_out_period);
  double d = (((double)ping_time) * sound_speed) / 20000.0;
  return d;
} 

int main(int an, char** av) {

  if (wiringPiSetup() < 0) {
    printf("Can't initialize Wiring Pi\n");
    return 1;
  }

  time_out_period = ((max_distance * 20000)  / sound_speed) + 1;

  double d;
  for(;;) {
    d = getDistance();
    printf("Target distance is %8.2lf cm\n", d);
    delay(1000);
  }
  return 0;
}
