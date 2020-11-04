// scan i2c
// Manferdelli

#include <Wire.h>

const int measurementDelay= 5000;
typedef uint8_t byte;
const int maxAddress= 127;

void setup() {
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  byte err, address;
  int num_devices = 0;

  Serial.println("Scanning");
  for (address = 1; address < maxAddress; address++) {
    Wire.beginTransmission(address);
    err= Wire.endTransmission();
    if (err == 0) {
      num_devices++;
      Serial.print("i2c device found at 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("");
    } else if (err == 4) {
      Serial.print("i2c error at 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("");
    }
  }

  Serial.print(num_devices);
  Serial.println(" devices");
  delay(measurementDelay);
}
