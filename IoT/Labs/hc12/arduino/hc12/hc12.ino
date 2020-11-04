// hc12
// Manferdelli

#include <SoftwareSerial.h>

const int deviceReceivePin= 4;
const int deviceTransmitPin= 5;
const int deviceSetPin = 6;

#ifndef byte
typedef uint8_t byte;
#endif

// Note: device transmit pin is SoftwareSerial receive pin
//   and vice-versa.
SoftwareSerial hc12(deviceTransmitPin, deviceReceivePin);

void copy(char* from, char* to, int size) {
  for(int i = 0; i < size; i++)
    to[i] = from[i];
  return;
}

int read_from_serial(int max, char* b) {
  int i = 0;
  while (hc12.available() != 0 && (i < max)) {
    b[i++] = hc12.read();
  }
  return i;
}

void setup() {
  pinMode(deviceSetPin, OUTPUT);
  delay(100);
  Serial.begin(9600);
  hc12.begin(9600);
}

void loop() {
  char send_buf[65];
  char receive_buf[65];

  copy((char*)"AT", send_buf, 3);
  for (;;) {
    digitalWrite(deviceSetPin, HIGH);  // Transparent mode
    delay(200);
    hc12.listen();
    int n = read_from_serial(64, receive_buf);
    if (n > 0) {
      receive_buf[n] = 0;
      Serial.print("Received: ");
      Serial.println((const char*)receive_buf);
    } else {
      Serial.println("nothing received");
    }
    digitalWrite(deviceSetPin, LOW); // Command mode
    delay(500);
    hc12.print(send_buf);
    Serial.print("Sent ");
    Serial.println((const char *)send_buf);  
  }
}
