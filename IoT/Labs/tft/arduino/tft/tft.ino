// TFT
// Manferdelli

const int dataPin= 8;
const int measurementDelay= 500;

typedef uint8_t byte;

void setup() {
  Serial.begin(9600);
}

void loop() {
}

/*
// There is no output on the LCD SCREEN. Run and check Serial monitor if the ILI9341 communicates.

// Connect data pins LCD_D 0-7 to arduino UNO:
// LCD_D 0 -- D8
// LCD_D 1 -- D9
// LCD_D 2 -- D2
// LCD_D 3 -- D3
// LCD_D 4 -- D4
// LCD_D 5 -- D5
// LCD_D 6 -- D6
// LCD_D 7 -- D7
// Connect command pins:
// LCD_RST -- A4   1 -> 0 min 15 micros 0 -> 1 
// LCD_CS  -- A3   chip select, aktiv LOW
// LCD_RS  -- A2   data/command select, 0 command, 1 data
// LCD_WR  -- A1   0 -> 1, HIGH when not used
// LCD_RD  -- A0   0 -> 1, HIGH when not used

// arduino UNO ports:
// B (digital pin 8 to 13) 
// C (analog input pins) 
// D (digital pins 0 to 7)   0 1 are RX TX, don't use 

void LCD_write(uint8_t d) {
  // ILI9341 reads data pins when WR rises from LOW to HIGH (A1 pin on arduino)
  PORTC = PORTC & B11111101; // WR 0
  
  // data pins of ILI9341 connected to two arduino ports
  PORTD = (PORTD & B00000011) | ((d) & B11111100); 
  PORTB = (PORTB & B11111100) | ((d) & B00000011); 
  
  PORTC = PORTC | B00000010; // WR 1
}

void LCD_command_write(uint8_t d) {
  PORTC = PORTC & B11111011; // LCD_RS = 0, arduino pin A2
  // write data pins
  LCD_write(d);
}

void LCD_data_write(uint8_t d) {
  PORTC = PORTC | B00000100; // LCD_RS = 1, arduino pin A2
  // write data pins
  LCD_write(d);
}

uint8_t LCD_read(void) {
  // CS LOW, WR HIGH, RD HIGH->LOW>HIGH, RS(D/C) HIGH 
  PORTC = PORTC | B00000100; // RS 1
  
  // LCD_RD - arduino pin A0
  // After RD falls from HIGH to LOW ILI9341 outputs data until RD returns to HIGH
  PORTC = PORTC & B11111110; // RD 0
  BD_as_input(); // Set arduino pins as input
  uint8_t pin72 = PIND & B11111100; // Read data pins 7-2
  uint8_t pin10 = PINB & B00000011; // Read data pins 1-0
  PORTC = PORTC | B00000001; // RD 1
  BD_as_output(); // Re-Set arduino pins as output
  return pin72 | pin10;
}

void BD_as_input(void) {
  // Pins 7-2 as input, no change for pins 1,0 (RX TX)
  DDRD = DDRD & B00000011; 
  // Pins 8-9 as input
  DDRB = DDRB & B11111100; 
}

void BD_as_output(void) {
  // Pins 7-2 as output, no change for pins 1,0 (RX TX)
  DDRD = DDRD | B11111100; 
  // Pins 8-9 as output
  DDRB = DDRB | B00000011; 
}

void LCD_init(void) {
  // LCD_RESET 1 - 0 - 1, arduino pin A4
  PORTC = PORTC | B00010000; // 1
  delay(10);
  PORTC = PORTC & B11101111; // 0
  delay(20);
  PORTC = PORTC | B00010000; // 1
  delay(20);
  
  // CS HIGH, WR HIGH, RD HIGH, CS LOW
  PORTC = PORTC | B00001000; // CS 1
  PORTC = PORTC | B00000010; // WR 1
  PORTC = PORTC | B00000001; // RD 1
  PORTC = PORTC & B11110111; // CS 0
  
  LCD_command_write(0xF7); // Pump ratio control
  LCD_data_write(0x20); // 
  
  LCD_command_write(0x3A); // COLMOD: Pixel Format Set
  LCD_data_write(0x55); 
  
  LCD_command_write(0x36); // Memory Access Control 
  // MY  - Row Address Order (bit7)
  // MX  - Column Address Order
  // MV  - Row / Column Exchange
  // ML  - Vertical Refresh Order
  // BGR - RGB-BGR Order
  // MH  - Horizontal Refresh ORDER(bit2)
  LCD_data_write(B00001000); 
  
  LCD_command_write(0x11); // Sleep OUT
  LCD_command_write(0x29); // Display ON
  
  delay(50);
}

void setup() {
  Serial.begin(9600);
  // Set pins 1-8 as output
  BD_as_output();
  
  // Set pins A0-A4 as output
  DDRC = DDRC | B00011111; 
  
  LCD_init();
  
  uint8_t r;
  Serial.println("d3 Read ID4");
  LCD_command_write(0xd3); // read ID4, should return 9341
  //delay(50);
  r = LCD_read(); //dummy
  //delay(50);
  r = LCD_read();
  Serial.println(r,BIN);
  //delay(50);
  r = LCD_read();
  Serial.println(r,BIN);
  //delay(50);
  r = LCD_read();
  Serial.println(r,BIN);
  Serial.println("");
  
  Serial.println("09 Read Display Status");
  LCD_command_write(0x09); // Read Display Status
  r = LCD_read(); //dummy
  r = LCD_read();
  Serial.println(r,BIN);
  r = LCD_read();
  Serial.println(r,BIN);
  r = LCD_read();
  Serial.println(r,BIN);
  r = LCD_read();
  Serial.println(r,BIN);
  Serial.println("");
  
  Serial.println("0a Read Display Power Mode");
  LCD_command_write(0x0a); // Read Display Power Mode
  r = LCD_read(); //dummy
  r = LCD_read();
  Serial.println(r,BIN);
  Serial.println("");
}

void loop() {
}
*/
