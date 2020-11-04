// eeprom read/write
// Manferdelli

#include <Wire.h>

typedef uint8_t byte;

const int measurementDelay= 500;
const int initial_clock_setting= 400000;
const uint32_t maxMillis= 2000;

#define MAX_BYTES 64

// Theoretically, the 24LC256 has a 64-byte page write buffer but 
// we'll write 16 at a time to be safe.
#define MAX_I2C_WRITE 16 

// This address is determined by the way your address pins are wired.
// In the diagram from earlier, we connected A0 and A1 to Ground and 
// A2 to 5V. To get the address, we start with the control code from 
// the datasheet (1010) and add the logic state for each address pin
// in the order A2, A1, A0 (100) which gives us 0b1010100, or in 
// Hexadecimal, 0x54.
const byte eeprom_address= 0x54;

// Read
byte readEEProm(uint32_t address) {
  Wire.beginTransmission(eeprom_address);
  Wire.write((int) (address >> 8));     // MSB
  Wire.write((int) (address & 0xff));   // LSB
  Wire.endTransmission();
  Wire.requestFrom(eeprom_address, 1);
  byte r = 0xff;
  if (Wire.available())
    r= Wire.read();
  return r;
}

// Write
void writeEEProm(uint32_t address, byte* data, int size) {
  Wire.beginTransmission(eeprom_address);
  Wire.write((int) (address >> 8));     // MSB
  Wire.write((int) (address & 0xff));   // LSB
  for (byte  b = 0; b < size; b++)
    Wire.write(data[b]);
  Wire.endTransmission();
}

void printHexByte(byte b) {
  if (b < 16)
    Serial.print("0");
  Serial.print(b, HEX);
}

int readStream(byte* buf, int max) {
  int num_bytes = 0;
  for(;;) {
    while(Serial.available()) {
      if (num_bytes >= max)
        return num_bytes;
    }
    buf[num_bytes++] = Serial.read();
  }
  return num_bytes;
}

int initBuf(byte* buf, int max) {
  int i = 0;
  for (i = 0; i < 64; i++)
    buf[i] = (byte) (i + 16);
  return i;
}

void printBytes(byte* buf, int n) {
  for (int i = 0; i < n; i++) {
    printHexByte(buf[i]);
    Serial.print(" ");
    if ((i%16) ==15)
      Serial.println("");
  }
}

uint32_t end_address= 0x40; //0x7d00;
void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(initial_clock_setting);

  //  32000 is hex 0x7D00
  uint32_t reset= 0;

  int count = 0;
  byte to_write[MAX_BYTES];

  int current = 0;
  int num_bytes= initBuf(to_write, MAX_BYTES);
  int num_left= num_bytes;
  int n;

  Serial.println("Bytes accepted:");
  printBytes(to_write, num_bytes);
  Serial.println("");

  while(num_left > 0) {
    if (num_left >= MAX_I2C_WRITE)
      n = MAX_I2C_WRITE;
    else
      n = num_left;

    writeEEProm(current, &to_write[current], n);
    num_left -= n;
    current += n;
  }
  Serial.println("Bytes written");
  Serial.println("");

  // Read bytes to end_address
  Serial.println("Bytes read:");
  byte v;
  for (uint32_t a = 0; a < end_address; a++) {
    v= readEEProm(a);
    printHexByte(v);
    if ((a%16) ==15)
      Serial.println("");
  }
}

void loop() {
}
