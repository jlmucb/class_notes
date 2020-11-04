// BMP-280
// Manferdelli

#include <Wire.h>
#include "i2c.h"
#include "i2c_BMP280.h"
BMP280 bmp280;

const int measurementDelay= 500;

void setup() {
  Serial.begin(9600);

  if (!bmp280.initialize()) {
    Serial.println("Sensor missing");
  }
  bmp280.setEnabled(0);
  bmp280.triggerMeasurement();
}

void loop() {
  float temperature;
  float pascal;
  static float meters, metersold;

  bmp280.awaitMeasurement();
  bmp280.getTemperature(temperature);
  bmp280.getPressure(pascal);
  bmp280.getAltitude(meters); // , 1018.00); // Allow entry of forecast sea level pressure JFM
  metersold = (metersold * 10 + meters)/11;
  bmp280.triggerMeasurement();

  Serial.print(" HeightPT1: ");
  Serial.print(metersold);
  Serial.print(" m; Height: ");
  Serial.print(meters);
  Serial.print(" Pressure: ");
  Serial.print(pascal);
  Serial.print(" Pa; T: ");
  Serial.print(temperature);
  Serial.println(" C");

  delay(measurementDelay);
}




