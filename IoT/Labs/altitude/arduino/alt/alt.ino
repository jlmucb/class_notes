// gy-65 altitude, pressure, temp
// Manferdelli
//  Reference: https://www.sparkfun.com/tutorials/253

#include <Wire.h>

typedef uint8_t byte;
const int dataPin= 8;
const int measurementDelay= 500;
const int i2c_address= 0x77;
const int tempDelay= 5;

struct calibration_data {
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

int16_t swap_bytes(int16_t in) {
  return ((in << 8) & 0xff00) | ((in >> 8) & 0x00ff);
}

int read_i2c(byte r, byte* buf, int size) {
  Wire.beginTransmission(i2c_address);
  Wire.write(r);
  Wire.endTransmission(false);
  Wire.requestFrom(i2c_address, size, true);

  int i= 0;
  while (Wire.available() && i < size) {
    buf[i++]= Wire.read();
  }
  return i;
}

byte read_i2c_byte(byte r) {
  Wire.beginTransmission(i2c_address);
  Wire.write(r);
  Wire.endTransmission(false);
  Wire.requestFrom(i2c_address, 1, true);

  if (Wire.available()) {
    return Wire.read();
  }
}

void write_i2c(byte r, byte d) {
  Wire.beginTransmission(i2c_address);
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

void setup() {
  Serial.begin(9600);
  Wire.begin();
}

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

void loop() {
  calibration_data cd;
  calibrate(&cd);
  printCalibration(cd);

  double temperature= (double)get_temperature(cd);
  double pressure= (double)get_pressure(cd);
  double altitude= calculate_altitude(pressure);

  Serial.print("Temperature (C): ");
  Serial.print(temperature);
  Serial.print(", pressure (Pa): ");
  Serial.print(pressure);
  Serial.print(", altitude (m): ");
  Serial.print(altitude);
  Serial.println("");
  delay(measurementDelay);
}
