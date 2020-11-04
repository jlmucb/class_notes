// LCD-i2c
// Manferdelli

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// SDA-->A4
// SDL-->A5
typedef uint8_t byte;
const int printDelay = 50;
const int freezeDelay = 1000;

const int i2cAddress = 0x27;
const int numCols = 16;
const int numRows = 2;

LiquidCrystal_I2C lcd(i2cAddress, numCols, numRows);

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
  lcd.init();
  lcd.backlight();
}

void loop() {
  lcd.clear();
  printLine(0, "Hello, John");
  delay(freezeDelay);
  printLine(1, "Welcome to IoT");
  delay(freezeDelay);
}
