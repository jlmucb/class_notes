// Measure distance with ping sensor
// Manferdelli

const int triggerPin= 8;
const int echoPin= 7;
const int measurementDelay= 500;
const int pingDelay= 5;
const double temp = 20; // C
const double soundSpeed = 331.5 + .6 * temp;  // m/s

void setup() {
  Serial.begin(9600);
  Serial.println("Ping sensor test");
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

// pulseIn reads a pulse (either HIGH or LOW) on a pin. If value is HIGH,
// pulseIn() waits for the pin to go from LOW to HIGH to starts timing, 
// it then waits for the pin to go LOW and stops timing.
// Returns the length of the pulse in microseconds.

void measure_distance() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(3);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(triggerPin, LOW);
  double return_time_microsecs = pulseIn(echoPin, HIGH);
  double one_way_return_time = return_time_microsecs / 2e6;
  double d = soundSpeed * one_way_return_time * 100.0; // cm
  Serial.print("Measured distance (cm): ");
  Serial.println(d);
}

void loop() {
  for(;;) {
    delay(measurementDelay);
    measure_distance();
  }
}
