// GPS for raspberry pi
// Manferdelli
// This used the standard serial interface via /dev/ttyS0
// You'll need to use an fdmi connector

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#ifndef byte
typedef unsigned char byte;
#endif

#define default_uartDevice "/dev/ttyUSB0"
#define BUF_SIZE 512
const int sleep_interval = 1;
const int short_wait = 300000;
bool print_message = true;

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
  write(fd, command, strlen(command));
  write(fd, eol, 2);
}

#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ  "$PMTK220,100*1F"
#define PGCMD_ANTENNA "$PGCMD,33,1*6C"
void setup_gps(int fd) {
  sendCommand(fd, PMTK_SET_NMEA_OUTPUT_RMCGGA);
  sleep(sleep_interval);
  sendCommand(fd, PMTK_SET_NMEA_UPDATE_1HZ);
  sleep(sleep_interval);
  sendCommand(fd, PGCMD_ANTENNA);
  sleep(sleep_interval);
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
  bool date_valid_;
  int year_;
  int month_;
  int day_;
  bool location_valid_;
  int hour_;
  int min_;
  double seconds_;
  double degrees_lat_;
  double degrees_long_;
  double alt_meters_;
  int num_sats_;
};

const char* mths[12] = {
  "January", "February", "March",
  "April", "May", "June",
  "July", "August", "September",
  "October", "November", "December"
};

void print_gps_data(gpm_msg_values& out) {
  if (out.date_valid_ & (out.month_ >= 1 && out.month_ <= 12)) {
    printf("  Date: %04d %s %02d, ", out.year_, mths[out.month_ - 1],
        out.day_);
  }
  if (out.location_valid_) {
    printf("Time: %02d:%02d:%.4fZ\n", out.hour_, out.min_, out.seconds_);
    printf("  Lattitude: %.5f, Longitude: %.5f, ",
        out.degrees_lat_, out.degrees_long_);
    printf("Altitude: %.4f (m), %d SVs\n", out.alt_meters_, out.num_sats_);
  }
}

// NMEA message format
//  $GNGGA,HHMMSS.SSS,DDMM.MMMM,N/S,DDDMM.MMMM,E/W,n,NS...A*20
//  $GNRMC and $GNGLL messages also give location and time.
//  Examples:
//    $GNGLL,3745.5641,N,12226.3415,W,022529.000,A,A*5E
//    $GNGGA,022529.000,3745.5641,N,12226.3415,W,1,07,1.6,34.6,M,0.0,M,,*5A
//    $GNRMC,022529.000,A,3745.5641,N,12226.3415,W,0.56,0.00,230420,,,A*6D
//    $GNZDA,210543.000,21,03,2022,00,00*4B for date

bool parseZDANMEAMessage(char* msg, struct gpm_msg_values* v) {
  char* time_string = find_string_in_msg("$GNZDA,", msg);
  if (time_string == NULL || *time_string == ',')
    return false;
  char* day_string = find_string_in_msg(",", time_string);
  if (day_string == NULL || *day_string == ',')
    return false;
  sscanf(day_string, "%02d", &(v->day_));
  char* month_string = find_string_in_msg(",", day_string);
  if (month_string == NULL || *month_string == ',')
    return false;
  sscanf(month_string, "%02d", &(v->month_));
  char* year_string = find_string_in_msg(",", month_string);
  if (year_string == NULL || *year_string == ',')
    return false;
  sscanf(year_string, "%04d", &(v->year_));
  v->date_valid_ = true;
  return true;
}

bool parseGGANMEAMessage(char* msg, struct gpm_msg_values* v) {
  char* time_string = find_string_in_msg("$GNGGA,", msg);
  if (time_string == NULL)
    return false;
  sscanf(time_string, "%02d%02d", &v->hour_, &v->min_);
  sscanf(time_string+4, "%lf", &v->seconds_);

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
  sscanf(longitude_string, "%03d%02d.%04d", &deglong, &mlong, &minlong);
  v->degrees_long_ = ((double)deglong) + (((double)mlong) + ((double)minlong)/ 10000.0) / 60.0;
  char* ew_string = find_string_in_msg(",", longitude_string);
  if (ew_string == NULL || (*ew_string != 'E' && *ew_string != 'W'))
    return false;
  if (*ew_string == 'W')
    v->degrees_long_ *= -1.0;
  char* sats_str = find_string_in_msg(",", ew_string);
  if (sats_str == NULL)
    return false;
  sats_str = find_string_in_msg(",", sats_str);
  if (sats_str == NULL)
    return false;
  sscanf(sats_str, "%02d", &(v->num_sats_));
  char* alt_str = find_string_in_msg(",", sats_str);
  if (alt_str == NULL)
    return false;
  alt_str = find_string_in_msg(",", alt_str);
  sscanf(alt_str, "%lf", &(v->alt_meters_));
  v->location_valid_ = true;
  return true;
}

bool get_date(int fd, gpm_msg_values* out) {
  bool got_date = false;
  byte buf[BUF_SIZE];
  int n = 0;
  int msg_count = 1;

  while (!got_date) { 
    usleep(short_wait);
    clearBuf(buf, BUF_SIZE);
    n = read(fd, buf, BUF_SIZE - 1);
    if (n <=  0) {
      printf("read returns %d\n", n);
      continue;
    }
    buf[n++] = 0;

    if (print_message && n > 5)
      printf("Message %2d: %s", msg_count++, (char*) buf);

    got_date = parseZDANMEAMessage((char*)buf, out);
    if (got_date) {
      return true;
    }
  }
  return false;
}


bool get_location(int fd, gpm_msg_values* out) {
  bool got_fix = false;
  byte buf[BUF_SIZE];
  int n = 0;
  int msg_count = 1;

  while (!got_fix) { 
    usleep(short_wait);
    clearBuf(buf, BUF_SIZE);
    n = read(fd, buf, BUF_SIZE - 1);
    if (n <=  0) {
      printf("read returns %d\n", n);
      continue;
    }
    buf[n++] = 0;

    if (print_message && n > 5)
      printf("Message %2d: %s", msg_count++, (char*) buf);

    got_fix = parseGGANMEAMessage((char*)buf, out);
    if (got_fix) {
      return true;
    }
  }
  return false;
}


int main(int an, char** av) {
  int baudRate = 9600;
  int num_repeat = 2;
  const char* uartDevice = default_uartDevice;
  gpm_msg_values out;

  out.date_valid_ = false;
  out.location_valid_ = false;
  for (int i = 1; i < (an - 1); i++) {
    if (strcmp(av[i], "-trials") == 0 ) {
      num_repeat = atoi(av[++i]);
    } else if (strcmp(av[i], "-device") == 0) {
      uartDevice = av[++i];
    }
  }
  int fd = open(uartDevice, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    printf("Can't open %s\n", uartDevice);
    return 1;
  }

  // setup gps sensor and get location
  setup_gps(fd);
  if (get_date(fd, &out)) {
    printf("\n");
    print_message = false;
    for (int i = 0; i < num_repeat; i++) {
      if (get_location(fd, &out)) {
        print_gps_data(out);
        printf("\n");
      }
    }
  }

  close(fd);
  return 0;
}

