// Vibration
// Manferdelli

typedef uint8_t byte;
const int dataPin= 2;
const int measurementDelay= 20;

volatile int state = -1;
void trigger() {
  state= 1;
}

void setup() {
  Serial.begin(9600);
  attachInterrupt(dataPin, trigger, RISING);
}

void loop() {
  if (state == 1) {
    Serial.println("Vibration detected");
    delay(1);
    state= 0;
  }
  delay(measurementDelay);
}
