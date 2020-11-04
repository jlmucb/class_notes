// Rotary encoder
// Manferdelli

typedef uint8_t byte;
const int clockPin= 2;
const int dataPin= 4;
const int measurementDelay= 100;

int position = -1;
void encoder() {
  if (digitalRead(clockPin) == digitalRead(dataPin))
    position++;
  else
    position--;
}

void setup() {
  Serial.begin(9600);
  pinMode(clockPin, INPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, HIGH);
  attachInterrupt(0, encoder, CHANGE);
}

void loop() {
  Serial.println("");
  Serial.print("Position: ");
  Serial.print(position);
  Serial.println("");
  delay(measurementDelay);
}
