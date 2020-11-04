// Read accelerometer (MPU-6050)
//   Manferelli

#include <Wire.h>

#ifndef byte
typedef uint8_t byte;
#endif

const byte i2c_address= 0x68;
const byte sleep_mgmt= 0x6b;
const byte acc_x_out= 0x3b;

const int zero_pt= -512 - (340 * 35);
const double acc_normalizer= 16384.0;
const double gyro_normalizer= 131.0;
const double temp_normalizer= 340.0;

const int measurementDelay= 750;

// Wiring
//   SDL --> A5
//   SDA --> A4
//   int --> D2

typedef uint8_t byte;
struct pdu {
  int16_t x_acc;
  int16_t y_acc;
  int16_t z_acc;
  int16_t temp;
  int16_t x_gyro;
  int16_t y_gyro;
  int16_t z_gyro;
};

void setup() {
  Serial.begin(9600);
  Wire.begin();
  write_i2c(sleep_mgmt, 0x00);
}

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

void write_i2c(byte r, byte d) {
  Wire.beginTransmission(i2c_address);
  Wire.write(r);
  Wire.write(d);
  Wire.endTransmission(true);
}

void loop() {
  pdu p_out;
  read_i2c(acc_x_out, (byte*)&p_out, sizeof(pdu));
  p_out.x_acc= swap_bytes(p_out.x_acc);
  p_out.y_acc= swap_bytes(p_out.y_acc);
  p_out.z_acc= swap_bytes(p_out.z_acc);
  p_out.temp= swap_bytes(p_out.temp);
  p_out.x_gyro= swap_bytes(p_out.x_gyro);
  p_out.y_gyro= swap_bytes(p_out.x_gyro);
  p_out.z_gyro= swap_bytes(p_out.x_gyro);

  double acc_x= ((double)p_out.x_acc) / acc_normalizer;
  double acc_y= ((double)p_out.y_acc) / acc_normalizer;
  double acc_z= ((double)p_out.z_acc) / acc_normalizer;

  double t= (p_out.temp - zero_pt) / temp_normalizer;

  double gyro_x= ((double)p_out.x_gyro) / gyro_normalizer;
  double gyro_y= ((double)p_out.y_gyro) / gyro_normalizer;
  double gyro_z= ((double)p_out.z_gyro) / gyro_normalizer;
  
  Serial.println("");
  Serial.println("-----------------------------");
  Serial.print("Acceleration (g): (");
  Serial.print(acc_x);
  Serial.print(", ");
  Serial.print(acc_y);
  Serial.print(", ");
  Serial.print(acc_z);
  Serial.println(")");
  Serial.print("Temperature (C): ");
  Serial.print(t);
  Serial.println("");
  Serial.print("Gyro (deg/sec): (");
  Serial.print(gyro_x);
  Serial.print(", ");
  Serial.print(gyro_y);
  Serial.print(", ");
  Serial.print(gyro_z);
  Serial.println(")");
  Serial.println("-----------------------------");
  Serial.println("");
  delay(measurementDelay);
}
