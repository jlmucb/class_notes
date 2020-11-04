// Measure analog level
// Manferdelli

const int inputPin= A0;
const int pinDelay= 500;
const double maxVoltage= 5.0;
const double maxRange= (double)1024;

void setup() {
  pinMode(inputPin, INPUT);
  Serial.begin(9600);
  Serial.print("Maximum voltage: ");
  Serial.print(maxVoltage);
  Serial.print(", Maximum range: ");
  Serial.print(maxRange);
  Serial.println("");
}

void loop() {
  int x;
  double y;

  for(;;) {
    delay(pinDelay);
    x= analogRead(inputPin);
    y= maxVoltage * ((double)x)/maxRange;
    Serial.print("Voltage: ");
    Serial.print(y);
    Serial.println("");
  }
}
