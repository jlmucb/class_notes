// Keypad
// Manferdelli


#include <keypad.h>

const int dataPin= 8;
const int measurementDelay= 500;

typedef uint8_t byte;

byte rowPins[4] = {11, 10, 9, 8};
byte colPins[4] = {7, 6, 5, 4};
Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);

// define the symbols on the buttons of the keypad 
char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Get the character input
  char keyPressed = myKeypad.getKey();
  // If there is a character input, sent it to the serial port
  if (keyPressed) {
    Serial.println(keyPressed);
  }
}
