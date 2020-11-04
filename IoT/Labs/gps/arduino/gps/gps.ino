// GPS
// Manferdelli

#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>

#ifndef byte
typedef uint8_t byte;
#endif

const int gpsReceivePin= 3;
const int gpsTransmitPin= 5;
const int messageDelay= 500;

// Note: GPS transmit pin is SoftwareSerial receive pin
//   and vice-versa
SoftwareSerial serialGPS = SoftwareSerial(gpsTransmitPin, gpsReceivePin);
Adafruit_GPS GPS(&serialGPS);

void setup() {
  Serial.begin(9600);
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
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

struct gpm_msg_values {
  int hour_;
  int min_;
  double seconds_;
  double degrees_lat_;
  double degrees_long_;
  int num_sats_;
};

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

void loop() {
  gpm_msg_values out;
  bool got_fix = false;

  while (!got_fix) { 
    for(;;) {
      char c = GPS.read();     // causes the message to be read
      if (GPS.newNMEAreceived())
        break;
    }
    got_fix = parseNMEAMessage(GPS.lastNMEA(), &out);
    if (got_fix) {
      Serial.print("Time: ");
      printTwoDigitInt(out.hour_);
      Serial.print(":");
      printTwoDigitInt(out.min_);
      Serial.print(":");
      printTwoDigitDouble(out.seconds_);
      Serial.print(" GMT,  ");
      Serial.print("Position: ");
      Serial.print(out.degrees_lat_);
      Serial.print(" degrees latitude, ");
      Serial.print(out.degrees_long_);
      Serial.print(" degrees longitude");
      Serial.println();
    }
  }
  delay(messageDelay);
}
