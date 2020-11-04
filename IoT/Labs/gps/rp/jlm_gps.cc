// GPS for raspberry pi
// Manferdelli
// Manferdelli, GPS
// This used the standard serial interface via /dev/ttyS0
// You'll need to use an fdmi connector

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#ifndef byte
typedef unsigned char byte;
#endif

// GPS Interfacing with Raspberry Pi using C (WiringPi Library)
//  Mini uart is /dev/ttyS0
//  Note: GPS transmit pin is tty receive pin and vice-versa
//  vcc to 1
//  gnd to 3
//  tx to 5
//  Raspberry Pi      GPS
//  5V  (GPIO Pin  2) VCC
//  GND (GPIO Pin  6) GND
//  TXD (GPIO Pin  8) RXD
//  RXD (GPIO Pin 10) TXD

#define RxTxDevice "/dev/ttyAM0"
#define uartDevice "/dev/ttyS0"
#define BUF_SIZE 512
const int messageDelay = 50;

void clearBuf(byte* buf, int n) {
  for (int i = 0; i < n; i++) {
    buf[i] = 0;
  }
}

void printBuf(byte* in, int n) {
  for (int i = 0; i < n; i++) {
    printf(" %02x", in[i]);
    if ((i % 16) == 15) {
      printf("\n");
    }
  }
  printf("\n");
}

const char* eol = "\r\n";
void sendCommand(int fd, const char* command) {
  for (int i = 0; i < strlen(command); i++)
    serialPutchar(fd, command[i]);
  serialPutchar(fd, eol[0]);
  serialPutchar(fd, eol[1]);
}

#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PGCMD_ANTENNA "$PGCMD,33,1*6C"
void setup_gps(int fd) {
  sendCommand(fd, PMTK_SET_NMEA_OUTPUT_RMCGGA);
  sendCommand(fd, PMTK_SET_NMEA_UPDATE_1HZ);
  sendCommand(fd, PGCMD_ANTENNA);
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

// GPS value structure
struct gpm_msg_values {
  int hour_;
  int min_;
  double seconds_;
  double degrees_lat_;
  double degrees_long_;
  int num_sats_;
};

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

int read_serial_line(int fd, byte* buf, int size) {
  int r;
  int n = 0;

  while (n < (size - 1)) {
    r = serialGetchar(fd);
    buf[n++] = r;
    if (r == '\n') {
      buf[n++] = 0;
      return n;
    }
  }
  return -1;
}

bool get_location(int fd) {
  gpm_msg_values out;
  bool got_fix = false;
  byte buf[BUF_SIZE];
  int n;

  while (!got_fix) { 

    clearBuf(buf, BUF_SIZE);
    n = read_serial_line(fd, buf, BUF_SIZE);
    if (n <  0) {
      continue;
    }

    got_fix = parseNMEAMessage((char*)buf, &out);
    if (got_fix) {
      printf("Time: %02d:%02d:%07.4f GMT, ", out.hour_, out.min_, out.seconds_);
      printf("Position: %8.5f (lat), %8.5f (long)\n", out.degrees_lat_, out.degrees_long_);
      return true;
    }

  delay(messageDelay);
  }
  return false;
}


int main(int an, char** av) {
  int baudRate = 9600;

  // Initialize wiringPi
  if (wiringPiSetup() < 0) {
    printf("wiringPiSetup failed\n");
    return 0;
  }

  // Open the serial device
  int fd = serialOpen(uartDevice, baudRate);
  if (fd < 0) {
    printf("Can't open serialDevice\n");
    return 1;
  }

  // setup gps sensor and get location
  setup_gps(fd);
  get_location(fd);

  close(fd);
  return 0;
}

