// Display temperature and humidity on LCD 1602
// Manferdelli

// For LCD
//    SDA-->A4
//    SDL-->A5
//    Datapin 8 for DHT-11

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

typedef uint8_t byte;
const int dataPin= 8;
const int printDelay = 50;
const int freezeDelay = 1000;
const int measurementDelay= 100;
const int initReadDelay = 18;
const int i2cLcdAddress = 0x27;
const int numCols = 16;
const int numRows = 2;
const int packetSize = 40;      // bits
const int pulseOneLength = 40;  // pulse length to be 1
const int pulseDelay= 18;

LiquidCrystal_I2C lcd(i2cLcdAddress, numCols, numRows);

void clear_buf(byte* p, int size) {
  for (int i = 0; i < size; i++)
    p[i] = 0;
}

void start_measurement() {
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, LOW);
  delay(initReadDelay);
  digitalWrite(dataPin, HIGH);
}

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

void printLine(int line_no, const char* msg) {
  if (strlen(msg) > numCols)
    return;
  int l = strlen(msg);
  int pos = 0;
  for (; pos < l; pos++) {
    lcd.setCursor(pos, line_no);
    lcd.print(msg[pos]);
    delay(printDelay);
  }
  char blank = ' ';
  for (; pos < numCols; pos++) {
    lcd.setCursor(pos, line_no);
    lcd.print(blank);
    delay(printDelay);
  }
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
}

void loop() {
  int temperature, humidity;
  char line1[numCols];
  char line2[numCols];

  for(;;) {
    delay(measurementDelay);
    read_dht11(&temperature, &humidity);
    double temp_F = ((double) temperature) * 9.0 / 5.0 + 32.0;

    clear_buf((byte*)line1, numCols);
    snprintf(line1, numCols, "%d (C), %d (F)", temperature, (int)temp_F);
    clear_buf((byte*)line2, numCols);
    snprintf(line2, numCols, "%d percent", humidity);

    Serial.print("Temp (C): "); 
    Serial.print(temperature); 
    Serial.print(", Temp (F): "); 
    Serial.print(temp_F); 
    Serial.print(", Humidity (%): "); 
    Serial.print(humidity); 
    Serial.println(""); 

    // Print temp
    lcd.clear();
    printLine(0, line1);
    printLine(1, line2);
    delay(freezeDelay);
  }
}

