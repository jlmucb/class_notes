// LCD display
// Manferdelli

#include <LiquidCrystal.h>

typedef uint8_t byte;

//  16x2 LCD display.  The LiquidCrystal library works with all LCD displays 
//  that are compatible with the Hitachi HD44780 driver.
//
// LCD RS pin to digital pin 12
// LCD Enable pin to digital pin 11
// LCD D4 pin to digital pin 5
// LCD D5 pin to digital pin 4
// LCD D6 pin to digital pin 3
// LCD D7 pin to digital pin 2
// LCD R/W pin to ground
// LCD VSS pin to ground
// LCD VCC pin to 5V
// 10K resistor: ends to +5V and ground
// wiper to LCD VO pin (pin 3)
//
// See: http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.leftToRight();
  lcd.setCursor(0, 1);
  lcd.write("Hello, John");
  lcd.setCursor(0, 0);
  lcd.write("0123456789abcdef");

}

void loop() {
  // set the cursor to column 0, line 1 line 1 is the second row
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
}
