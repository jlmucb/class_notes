// Seven segment display
// Manferdelli

#ifndef byte
typedef uint8_t byte;
#endif

int latchPin = 12;
int clockPin = 13;
int dataPin = 11;
const int measurementDelay= 1500;

// Pin11 to SH_CP 74HC595
// Pin12 connected to ST_CP 74HC595,
// Pin13 connected to DS of 74HC595

// encoding of digits
//    a b c d e f g h
//  0 1 1 1 1 1 1 0 0   0xfc
//  1 0 1 1 0 0 0 0 0   0x60
//  2 1 1 0 1 1 0 1 0   0xda
//  3 1 1 1 1 0 0 1 0   0xf2
//  4 0 1 1 0 0 1 1 0   0x66
//  5 1 0 1 1 0 1 1 0   0xb6
//  6 1 0 1 1 1 1 1 0   0xbe
//  7 1 1 1 0 0 0 0 0   0xe0
//  8 1 1 1 1 1 1 1 0   0xfe
//  9 1 1 1 0 0 1 1 0   0xe6
//  a 1 1 1 1 1 0 1 0   0xfa
//  b 0 0 1 1 1 1 1 0   0x3e
//  c 0 0 0 1 1 0 1 0   0x1a
//  d 0 1 1 1 1 0 1 0   0x7a
//  e 1 0 0 1 1 1 1 0   0x9e
//  f 1 1 0 0 1 1 1 0   0x8e
byte digit_pattern_seg_on[] = {
  0xfc, 0x60, 0xda, 0xf2,
  0x66, 0xb6, 0xbe, 0xe0,
  0xfe, 0xe6, 0xfa, 0x3e,
  0x1a, 0x7a, 0x9e, 0x8e,
};
byte digit_pattern_seg_off[] = {
  0x03, 0x9f, 0x25, 0x0d,
  0x99, 0x49, 0x41, 0x1f,
  0x01, 0x19, 0x05, 0xc1,
  0xe4, 0x85, 0x61, 0x71,
};

void setup() {
  Serial.begin(9600);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, OUTPUT);
}

void writeData(int value) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  digitalWrite(latchPin, HIGH);
}

void loop() {
  for (int j = 0; j < 16; j++) {
    writeData(digit_pattern_seg_off[j]);
    delay(measurementDelay);
  }
}
