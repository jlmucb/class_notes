// GPS-pressure-temp-humidity with lcd-1602
// Manferdelli

// For LCD
//    SDA-->A4
//    SDL-->A5
//    Datapin 8 for DHT-11

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>

#ifndef byte
typedef uint8_t byte;
#endif

const int dataPin= 8;
const int gpsReceivePin= 3;
const int gpsTransmitPin= 5;

const int i2cBarometeraddress = 0x77;
const int i2cLcdAddress = 0x27;

const int tempDelay= 5;
const int printDelay = 50;
const int freezeDelay = 2000;
const int measurementDelay= 100;
const int initReadDelay = 18;
const int numCols = 16;
const int numRows = 2;
const int packetSize = 40;      // bits
const int pulseOneLength = 40;  // pulse length to be 1
const int pulseDelay= 18;

class calibration_data {
public:
  int16_t ac1_;
  int16_t ac2_;
  int16_t ac3_;
  uint16_t ac4_;
  uint16_t ac5_;
  uint16_t ac6_;
  int16_t b1_;
  int16_t b2_;
  int16_t mb_;
  int16_t mc_;
  int16_t md_;

  // temps
  long b5_;
};

class gpm_msg_values {
public:
  int hour_;
  int min_;
  double seconds_;
  double degrees_lat_;
  double degrees_long_;
  int num_sats_;
};

// Note: GPS transmit pin is SoftwareSerial receive pin
//   and vice-versa
SoftwareSerial serialGPS = SoftwareSerial(gpsTransmitPin, gpsReceivePin);
Adafruit_GPS GPS(&serialGPS);
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

void printTwoDigitInt(int x) {
  if (x < 10)
    Serial.print("0");
  Serial.print(x);
}

void printTwoDigitDouble(double x) {
  if (x < 10.0)
    Serial.print("0");
  Serial.print(x);
}

// s: string to match
// t: message string
char* strmatch(const char* s, char* t) {
  for (;;) {
    if (*s == 0 || *t == 0)
      return NULL;
    if (*s != *t)
      return NULL;
    s++; t++;
    if (*s == 0)
      return t;
  }
}

char* find_string_in_msg(const char* str, char* msg) {
  int l = strlen(msg);
  char* s = NULL;

  for (int i = 0; i < l; i++) {
    s = strmatch(str, &msg[i]);
    if (s != NULL)
      return s;
  }
  return NULL;
}

// NMEA message format
//  $GNGGA,HHMMSS.SSS,DDMM.MMMM,N/S,DDDMM.MMMM,E/W,n,NS...A*20
bool parseNMEAMessage(char* msg, struct gpm_msg_values* v) {
  char* time_string = find_string_in_msg("$GNGGA,", msg);
  if (time_string == NULL)
    return false;
  sscanf(time_string, "%02d%02d", &v->hour_, &v->min_);
  sscanf(time_string+4, "%f", &v->seconds_);

  char* latitude_string = find_string_in_msg(",", time_string);
  if (latitude_string == NULL)
    return false;
  int deglat;
  int mlat;
  int minlat;
  sscanf(latitude_string, "%02d%02d.%04d", &deglat, &mlat, &minlat);
  v->degrees_lat_ = ((double)deglat) + (((double)mlat) + ((double)minlat) / 10000.0) / 60.0;
  char* ns_string = find_string_in_msg(",", latitude_string);
  if (ns_string == NULL || (*ns_string != 'N' && *ns_string != 'S'))
    return false;
  if (*ns_string == 'S')
    v->degrees_lat_ *= -1.0;
  char* longitude_string = ns_string + 2;
  int deglong;
  int mlong;
  int minlong;
  sscanf(longitude_string, "%03d%02d.04d", &deglong, &mlong, &minlong);
  v->degrees_long_ = ((double)deglong) + (((double)mlong) + ((double)minlong)/ 10000.0) / 60.0;
  char* ew_string = find_string_in_msg(",", longitude_string);
  if (ew_string == NULL || (*ew_string != 'E' && *ew_string != 'W'))
    return false;
  if (*ew_string == 'W')
    v->degrees_long_ *= -1.0;

  return true;
}

// Barometer

void printCalibration(calibration_data& cd) {
  Serial.println("");
  Serial.println("Calibration data: ");
  Serial.print("ac1: ");
  Serial.print(cd.ac1_);
  Serial.print(", ac2: ");
  Serial.print(cd.ac2_);
  Serial.print(", ac3: ");
  Serial.print(cd.ac3_);
  Serial.println("");
  Serial.print("ac4: ");
  Serial.print(cd.ac4_);
  Serial.print(", ac5: ");
  Serial.print(cd.ac5_);
  Serial.print(", ac6: ");
  Serial.print(cd.ac6_);
  Serial.println("");
  Serial.print("b1: ");
  Serial.print(cd.b1_);
  Serial.print(", b2: ");
  Serial.print(cd.b2_);
  Serial.println("");
  Serial.print("mb: ");
  Serial.print(cd.mb_);
  Serial.print(", mc: ");
  Serial.print(cd.mc_);
  Serial.print(", md: ");
  Serial.print(cd.md_);
  Serial.println("");
}

int16_t swap_bytes(int16_t in) {
  return ((in << 8) & 0xff00) | ((in >> 8) & 0x00ff);
}

int read_i2c(byte r, byte* buf, int size) {
  Wire.beginTransmission(i2cBarometeraddress);
  Wire.write(r);
  Wire.endTransmission(false);
  Wire.requestFrom(i2cBarometeraddress, size, true);

  int i= 0;
  while (Wire.available() && i < size) {
    buf[i++]= Wire.read();
  }
  return i;
}

byte read_i2c_byte(byte r) {
  Wire.beginTransmission(i2cBarometeraddress);
  Wire.write(r);
  Wire.endTransmission(false);
  Wire.requestFrom(i2cBarometeraddress, 1, true);

  if (Wire.available()) {
    return Wire.read();
  }
}

void write_i2c(byte r, byte d) {
  Wire.beginTransmission(i2cBarometeraddress);
  Wire.write(r);
  Wire.write(d);
  delay(tempDelay);
  Wire.endTransmission(true);
}

int read_i2c_int(byte address) {
  int16_t d;

  read_i2c(address, (byte*)&d, sizeof(int16_t));
  d = swap_bytes(d);
  return d;
}

const double pressure_at_sea_level= 101325.0;  // Pa
void calibrate(calibration_data* d) {
  read_i2c(0xaa, (byte*)d, sizeof(calibration_data));
  int16_t* ip = (int16_t*)d;
  for (int i = 0; i < 16; i++) {
    ip[i]= swap_bytes(ip[i]);
  }
}

double calculate_altitude(long pressure) {
  double normalizedP= ((double)pressure)/ pressure_at_sea_level;
  double a = (1.0 - pow(normalizedP, 1 / 5.255)) * 44330.0;
  return a;
}

const long OSS = 1;

long calculate_real_temperature(calibration_data& cd, long raw) {
  long x1 = ((raw - ((long)cd.ac6_)) * ((long)cd.ac5_)) >> 15;
  long x2 = (((long)cd.mc_) << 11) / (x1 + ((long)cd.md_));
  cd.b5_ = x1 + x2;
  long t = (cd.b5_ + 8) >> 4;
  Serial.println("");
  Serial.println("Real temperature");
  Serial.print("x1: ");
  Serial.println(x1);
  Serial.print("x2: ");
  Serial.println(x2);
  Serial.print("b5: ");
  Serial.println(cd.b5_);
  return t / 10;
}

long calculate_real_pressure(calibration_data& cd, long raw) {
  long p;

  long b6 = cd.b5_ - 4000;
  Serial.print("b6: ");
  Serial.println(b6);
  long x1 = (((long)cd.b2_) * ((b6 * b6) >> 12)) >> 11;
  Serial.print("x1: ");
  Serial.println(x1);
  long x2 = (((long)cd.ac2_) * b6) >> 11;
  Serial.print("x2: ");
  Serial.println(x2);
  long x3 = x1 + x2;
  Serial.print("x3: ");
  Serial.println(x3);
  long b3 = (((((long)cd.ac1_) * 4 + x3) << OSS) + 2) / 4;
  Serial.print("b3: ");
  Serial.println(b3);

  x1= (((long)cd.ac3_) * b6) >> 13;
  Serial.print("x1: ");
  Serial.println(x1);
  x2 = (((long)cd.b1_) * ((b6 * b6) >> 12)) >> 16;
  Serial.print("x2: ");
  Serial.println(x2);
  x3 = (x1 + x2 + 2) / 4;
  Serial.print("x3: ");
  Serial.println(x3);
  long b4 = (((long)cd.ac4_) * (x3 + 32768)) >> 15;
  Serial.print("b4: ");
  Serial.println(b4);
  long b7 = (raw - b3) * (50000 >> OSS);
  Serial.print("b7: ");
  Serial.println(b7);
  if (b7 < 0x80000000UL)
    p = (b7 * 2) / b4;
  else
    p = (b7 / b4) * 2;
  Serial.print("p: ");
  Serial.println(p);
  x1 = (p >> 8) * (p >> 8);
  Serial.print("x1: ");
  Serial.println(x1);
  x1 = (x1 * 3038) >> 16;
  Serial.print("x1: ");
  Serial.println(x1);
  x2 = (-7357 * p) >> 16;
  Serial.print("x2: ");
  Serial.println(x2);
  p += (x1 + x2 + 3791) >> 4;
  Serial.print("p: ");
  Serial.println(p);
  return p;
}

long raw_temperature() {
  write_i2c(0xf4, 0x2e);
  delay(tempDelay);
  long t = read_i2c_int(0xf6);
  return t;
}

long raw_pressure() {
  write_i2c(0xf4, 0x2e);
  delay(tempDelay);
  byte msb= read_i2c_byte(0xf6);
  byte lsb1= read_i2c_byte(0xf7);
  byte lsb2= read_i2c_byte(0xf8);
  long rp = ((((long)msb) << 16) + (((long)lsb1) << 8) + ((long)lsb2)) >> (8 - OSS);
  return rp;
}

long get_temperature(calibration_data& cd) {
  long rt= raw_temperature();
  Serial.print("Raw temperature: ");
  Serial.print(rt);
  Serial.println("");
  return calculate_real_temperature(cd, rt);
}

long get_pressure(calibration_data& cd) {
  long rp = raw_pressure();
  
  Serial.print("ATM pressure: ");
  Serial.println(pressure_at_sea_level);
  Serial.print("Raw pressure: ");
  Serial.print(rp);
  Serial.println("");
  long p = calculate_real_pressure(cd, rp);
  Serial.print("Real pressure: ");
  Serial.print(p);
  Serial.println("");
  return p;
}

// LCD
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
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  Wire.begin();
}

void format_fp(double x, int* a, int* b) {
  *a = (int) x;
  double t = (double)*a;
  *b = (int)(10000.0 * (x - t));
  if (*b < 0)
    *b *= -1;
}

void loop() {
  int temperature, humidity;
  double barometer_temperature;
  double barometer_pressure;
  double temp_F;
  double altitude;
  calibration_data cd;
  struct gpm_msg_values gps_values;
  char dht_line1[numCols];
  char dht_line2[numCols];
  char gps_line1[numCols];
  char gps_line2[numCols];
  char barometer_line1[numCols];
  char barometer_line2[numCols];
  bool got_fix = false;

  while (!got_fix) {
    for(;;) {
      char c = GPS.read();     // causes the message to be read
      if (GPS.newNMEAreceived())
        break;
    }
    got_fix = parseNMEAMessage(GPS.lastNMEA(), &gps_values);
    if (got_fix) {
      Serial.print("Time: ");
      printTwoDigitInt(gps_values.hour_);
      Serial.print(":");
      printTwoDigitInt(gps_values.min_);
      Serial.print(":");
      printTwoDigitDouble((int)gps_values.seconds_);
      Serial.print(" GMT,  ");
      Serial.print("Position: ");
      Serial.print(gps_values.degrees_lat_);
      Serial.print(" degrees latitude, ");
      Serial.print(gps_values.degrees_long_);
      Serial.println(" degrees longitude");

      int a, b, c, d;
      format_fp(gps_values.seconds_, &a, &b);
      clear_buf((byte*)gps_line1, numCols);
      snprintf(gps_line1, numCols, "%02d:%02d:%02d.%02d",
          gps_values.hour_, gps_values.min_, a, b/100);
      clear_buf((byte*)gps_line2, numCols);
      format_fp(gps_values.degrees_lat_, &a, &b);
      format_fp(gps_values.degrees_long_, &c, &d);
      snprintf(gps_line2, numCols, "%03d.%02d, %03d.%02d",
          a, b/100, c, d/100);
 
      // Print GPS values
      lcd.clear();
      printLine(0, gps_line1);
      printLine(1, gps_line2);
    }
  }
  delay(freezeDelay);

  // temperature and humidity
  read_dht11(&temperature, &humidity);
  temp_F = ((double) temperature) * 9.0 / 5.0 + 32.0;

  Serial.print("Temperature (C): "); 
  Serial.print(temperature); 
  Serial.print(", Temperature (F): "); 
  Serial.print(temp_F); 
  Serial.print(", Humidity (%): "); 
  Serial.println(humidity); 

  clear_buf((byte*)dht_line1, numCols);
  snprintf(dht_line1, numCols, "%d (C), %d (F)", temperature, (int)temp_F);
  clear_buf((byte*)dht_line2, numCols);
  snprintf(dht_line2, numCols, "%d percent", humidity);

  // Print temperature and humidity
  lcd.clear();
  printLine(0, dht_line1);
  printLine(1, dht_line2);
  delay(freezeDelay);

  // Print Barometer
  calibrate(&cd);
  printCalibration(cd);

  barometer_temperature= (double)get_temperature(cd);
  barometer_pressure= (double)get_pressure(cd);
  altitude= calculate_altitude(barometer_pressure);

  Serial.print("Temperature (C): ");
  Serial.print(barometer_temperature);
  Serial.print(", pressure (Pa): ");
  Serial.print(barometer_pressure);
  Serial.print(", altitude (m): ");
  Serial.println(altitude);

  int a, b, c, d;;
  format_fp(barometer_temperature, &a, &b);
  format_fp(temp_F, &c, &d);
  clear_buf((byte*)barometer_line1, numCols);
  snprintf(barometer_line1, numCols, "%d (C), %d (F)", a, c);
  clear_buf((byte*)barometer_line2, numCols);
  format_fp(altitude, &a, &b);
  snprintf(barometer_line2, numCols, "%d (m)", a);
  
  // Print Barometer values
  lcd.clear();
  printLine(0, barometer_line1);
  printLine(1, barometer_line2);
  delay(freezeDelay);
}
