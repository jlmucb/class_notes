// Tilt
// Manferdelli

typedef uint8_t byte;
const int dataPin= 8;
const int measurementDelay= 500;

void setup() {
  Serial.begin(9600);
  pinMode(dataPin, INPUT);
  digitalWrite(dataPin, HIGH);  // Why?
}

void loop() {
  int tilted= digitalRead(dataPin);
  Serial.println("");
  if (tilted == 0) {
    Serial.println("Tilted");
  } else {
    Serial.println("Not tilted");
  }
  delay(measurementDelay);
}
