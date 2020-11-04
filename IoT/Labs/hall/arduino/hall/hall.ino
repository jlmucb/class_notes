// Hall sensor
// Manferdelli

typedef uint8_t byte;
const int dataPin= A0;
const int measurementDelay= 500;


void setup() {
  Serial.begin(9600);
  pinMode(dataPin, INPUT);
}

const int zero_field= 527;
void loop() {
  int field_strength= analogRead(dataPin);
  Serial.print("Field: ");
  Serial.print(field_strength);
  int calibrated_field = field_strength - zero_field;
  Serial.print(", calibrated field: ");
  Serial.print(calibrated_field);
  Serial.println("");
  delay(measurementDelay);
}
