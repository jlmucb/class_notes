// Measure temperature and humidity with DHT-11
// Manferdelli

const int dataPin= 8;
const int measurementDelay= 500;

typedef uint8_t byte;

void setup() {
  Serial.begin(9600);
}

void clear_buf(byte* p, int size) {
  for (int i = 0; i < size; i++)
    p[i] = 0;
}

const int initReadDelay = 18;
void start_measurement() {
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, LOW);
  delay(initReadDelay);
  digitalWrite(dataPin, HIGH);
}

const int packetSize = 40;      // bits
const int pulseOneLength = 40;  // pulse length to be 1
const int pulseDelay= 18;
int measure_th(int* t, int* h) {
  byte buf[8];
  clear_buf(buf, 8);

  byte byte_index = 0;
  byte bit_cnt = 7;

  pinMode(dataPin, INPUT);
  delayMicroseconds(pulseDelay);
  pulseIn(dataPin, HIGH);

  // Collect each data bit
  int time_out = 10000;
  for (int i = 0; i < packetSize; i++) {
    while (digitalRead(dataPin) == LOW) {
      if (time_out-- <= 0)
        return -1;
    }
    time_out = 10000;
    unsigned long t = micros();
    while (digitalRead(dataPin) == HIGH) {
      if (time_out-- <= 0)
        return -1;
    }
    if ((micros() - t) > pulseOneLength) {
      buf[byte_index] |= 1 << bit_cnt;
    }
    if (bit_cnt == 0) {
      byte_index++;   // next byte
      bit_cnt = 7;    // MSB
    } else {
      bit_cnt--;
    }
  }

  *t = buf[2];
  *h = buf[0];
  if ((buf[0] + buf[2]) != buf[4])
    return -1;

  return 0;
}

void read_dht11(int* t, int* h) {
  start_measurement();
  if (measure_th(t, h) < 0) {
    *t = 0.0;
    *h= 0.0;
    return;
  }
}

void loop() {
  int temperature, humidity;

  for(;;) {
    delay(measurementDelay);
    read_dht11(&temperature, &humidity);
    Serial.print("Temp (C): "); 
    Serial.print(temperature); 
    Serial.print(", Temp (F): "); 
    double temp_F = ((double) temperature) * 9.0 / 5.0 + 32.0;
    Serial.print(temp_F); 
    Serial.print(", Humidity (%): "); 
    Serial.print(humidity); 
    Serial.println(""); 
  }
}
