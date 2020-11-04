// turn LED on and off
// Manferdelli

const int switchPin= 4;
const int oscDelay= 500;
const int analogOn= 255;
const int analogOff= 0;

void setup() {
  pinMode(switchPin, OUTPUT);
}

void loop() {
  for(;;) {
    analogWrite(switchPin, analogOff);
    delay(oscDelay);
    analogWrite(switchPin, analogOn);
    delay(oscDelay);
  }
}
