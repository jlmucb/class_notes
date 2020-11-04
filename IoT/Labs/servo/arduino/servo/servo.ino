// Servo
// Manferdelli

#include <Servo.h>

typedef uint8_t byte;
const int dataPin= 3;
const int measurementDelay= 500;
const int servoDelay= 15;

Servo servo;
void setup() {
  Serial.begin(9600);
  servo.attach(dataPin);
}

void loop() {
  int position= 0;

  for (position = 0; position <= 180; position++) {
    servo.write(position);
    delay(servoDelay);
  }
  for (position = 180; position >= 180; position--) {
    servo.write(position);
    delay(servoDelay);
  }
  delay(measurementDelay);
}
