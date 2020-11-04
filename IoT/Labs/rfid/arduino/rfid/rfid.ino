// rfid
// Manferdelli

#include <SPI.h>
#include <MFRC522.h>

typedef uint8_t byte;
const int rstPin = 9;
const int ssPin = 10;
const int measurementDelay= 500;

// Class args: slave select, reset      
MFRC522 rfid(ssPin, rstPin);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
}

bool access_valid(int size, byte* id) {
  // our card has id: FA-7C-63-2E
  if (size != 4)
    return false;
  if ( id[0] != 0xfa || id[1] != 0x7c || id[2] != 0x63 || id[3] != 0x2e)
    return false;
  return true;
}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) {
    if(rfid.PICC_ReadCardSerial()) {
      Serial.print("Card type: ");
      for (int i = 0; i < rfid.uid.size; i++) {
        if (i != 0)
          Serial.print("-");
        if (rfid.uid.uidByte[i] <= 0x9) {
          Serial.print("0");
        }
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println("");
      if (access_valid(rfid.uid.size, rfid.uid.uidByte)) {
        Serial.println("Access granted");
      } else {
        Serial.println("Access denied");
      }
    } else {
    Serial.println("Can't read card serial number");
    }
  } else {
    Serial.println("Card not present");
  }
  delay(measurementDelay);
}
