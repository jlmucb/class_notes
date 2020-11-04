// Analog read
// Manferdelli

const int dataPin= A0;
const int measurementDelay= 500;
typedef uint8_t byte;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int wetness = analogRead(dataPin);
  Serial.print("Wetness: ");
  Serial.print(wetness);
  Serial.println("");
  delay(measurementDelay);
}

