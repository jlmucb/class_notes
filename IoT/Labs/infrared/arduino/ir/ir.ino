// Infrared
// Manferdelli

#include <IRremote.h>

typedef uint8_t byte;
const int dataPin= 12;
const int measurementDelay= 200;

IRrecv receiver(dataPin);

void setup() {
  Serial.begin(9600);
  receiver.enableIRIn();
}

void loop() {
  decode_results results;

  if (receiver.decode(&results)) {
    Serial.print(results.value, HEX);
    Serial.println("");
    receiver.resume();
  }
  delay(measurementDelay);
}
