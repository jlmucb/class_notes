// ublox GPS with dead reckoning
// Manferdelli
// You'll need to use an fdmi connector

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#ifndef byte
typedef unsigned char byte;
#endif

#define default_uartDevice "/dev/ttyUSB0"
#define BUF_SIZE 512
const int sleep_interval = 1;
const int short_wait = 200000;
bool print_message = true;

void clearBuf(byte* buf, int n) {
  for (int i = 0; i < n; i++) {
    buf[i] = 0;
  }
}

void print_bytes(byte* in, int n) {
  for (int i = 0; i < n; i++) {
    printf(" %02x", in[i]);
    if ((i % 16) == 15) {
      printf("\n");
    }
  }
  printf("\n");
}

// UBOX commands
//    Command
//      sync-1(0xb5) sync-2(0x62), class(0x06), id(0x01), len(2 bytes), payload, ck_a, ck_b
//    checksum computed from class through payload

//  UTC time: 124923.52.  hour: 12, min: 49, sec: 23, nano: 521000000
//  GPS time is referenced to 6th January 1980

//
//    Checksum
//      byte ck_a = 0;
//      byte ck_b=0;
//      for (int i= 0; i < n; i++) {
//        ck_a += buf[i]
//        ck_b += ck_a;
//      }
void compute_checksum(byte* buf, int len, byte* pa, byte* pb) {
  byte a = 0; 
  byte b = 0; 

  for (int i = 0; i < len; i++) {
    a += buf[i];
    b += a;
  }
  *pa = a;
  *pb = b;
}

//  The UBX protocol is designed so that messages can be polled by sending the message required to 
//  the receiver but without a payload (or with just a single parameter that identifies the poll request).
//  sync1 sync2 class id len-payload payload ck_a ck_b
//  ACK is 0xb5 0x062, 0x01, cls, msg, ck_a, ck_b
//  NACK is 0xb5 0x062, 0x00, cls, msg, ck_a, ck_b
//  UBX_CFG_PRT: 0xB5 0x62 0x06 0x00 20 Payload CK_A CK_B
//  Poll configuration: 0xB5 0x62 0x06 0x02 1 0 CK_A, CK_B
byte ubx_poll_cfg[9] = {
  0xb5, 0x62, 0x06, 0x02, 0x01, 0x00, 0x01, 0x00, 0x00
};
byte ubx_cfg_prt[28] = {
  0xb5, 0x62, 0x06, 0x00, 
  0x14, 0x00, 
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00
};

void format_message(byte* buf, int sz) {
  compute_checksum(&buf[2], sz - 4,
      &buf[sz - 2], &buf[sz - 1]);
}

void send_message(int fd, byte* buf, int sz) {
  for(int i = 0; i < sz; i++) {
    write(fd, &buf[i], 1);
  }
}

byte ack_msg[8] = {
  0xb5,0x62,0x05,0x01,0x02,0x00,0x06,0x02
};
bool is_ack(byte* in, int sz, byte* cls, byte* msgid) {
  if (sz != 10)
    return false;
  if (memcmp(in, ack_msg, 4) != 0)
    return false;
  *cls = in[6];
  *msgid = in[7];
  return true;
}

byte nack_msg[8] = {
  0xb5,0x62,0x05,0x00,0x02,0x00,0x06,0x02
};
bool is_nack(byte* in, int sz, byte* cls, byte* msgid) {
  if (sz != 10)
    return false;
  if (memcmp(in, nack_msg, 4) != 0)
    return false;
  *cls = in[6];
  *msgid = in[7];
  return true;
}

int recover_ublx_message(int fd, byte* out, int sz) {
  usleep(200000);
  int n = 0;
  int cur = 0;
   
  for (;;) { 
    n = read(fd, &out[0], 1); 
    if (n < 1) {
      return -1;
    }
    if (out[0] == 0xb5) {
      n = read(fd, &out[1], 1); 
      if (n < 1) {
        return -1;
      }
      if (out[1] == 0x62) {
        cur = 2;
        break;
      }
    }
  }
  // get class and id
  n = read(fd, &out[cur++], 1); 
  if (n < 1) {
    return -1;
  }
  n = read(fd, &out[cur++], 1); 
  if (n < 1) {
    return -1;
  }

  // get size
  unsigned short sm = 0;
  n = read(fd, &sm, 2); 
  if (n < 2) {
    return -1;
  }
  memcpy(&out[cur], (byte*)&sm, 2);
  cur += 2;

  int m = (int) sm + 2;
  // get payload and checksum
  n = read(fd, &out[cur], m); 
  if (n < m) {
    return -1;
  }
  cur += m;

  return cur;
}

int available(int fd) {
  int n = 0;
  ioctl(fd, FIONREAD, &n);
  return n;
}

// Add
//    UBX-CFG-MSG (0x06 0x01)
//    UBX-CFG-NAV5
//    UBX-CFG-NMEA (0x06 0x17)
//    UBX-HNR (0x28)
//    UBX-HNR-PVT (0x28 0x00)
//    UBX-NAV (0x01)
//    UBX-RXM (0x02)
//    UBX-CFG-RATE
//    UBX-CFG-ITFM
//    UBX-ESF-INS (dead rekoning)


//  UBX-NAV-TIMEGPS
//    HDR (0xb5 0x62), class (0x01), id (0x20), length (16) PL chk_a chk_b
//      Byte offset type    item
//      00          U4      time-of-week (ms)
//      04
//      08          I2      week
//      10          I1      leasS
//      11          X1      valid
//      12          U4      accuracy
//

//  UBX-NAV-TIMEUTC
//    HDR (0xb5 0x62), class (0x01), id (0x21), length (20) PL chk_a chk_b
//      Byte offset type    item
//      00          U4
//      04          U4
//      08          I4
//      12          U2      year (1999-2099)
//      14          U1      month (1-12)
//      15          U1      day (1-31)
//      16          U1      hour (0-23)
//      17          U1      min (0-59)
//      18          U1      sec
//      19          X1


bool test_ublox_cmds(int fd) {
  byte ack_buf[256];
  byte out_buf[256];

  // clear buf
  int n = 0;
  while (available(fd) > 0)
    n = read(fd, out_buf, 256);  // clear buffer

  format_message(ubx_poll_cfg, sizeof(ubx_poll_cfg));
  send_message(fd, ubx_poll_cfg, sizeof(ubx_poll_cfg));
#if 0
  printf("Sent: ");
  print_bytes(ubx_poll_cfg, sizeof(ubx_poll_cfg));
#endif

  n = recover_ublx_message(fd, out_buf, 256);
  if (n < 0)
    return false;

#if 0
  printf("Recovered UBLX: ");
  print_bytes(out_buf, n);
#endif

  int m = recover_ublx_message(fd, ack_buf, 256);
  byte cls = 0;
  byte msgid = 0;
  if (m < 0)
    return false;

  if (is_ack(ack_buf, m, &cls, &msgid)) {
    return true;
  }

  return false;
}

void setup_gps(int fd) {
  if (test_ublox_cmds(fd))
    printf("UBLOX test succeeded\n\n");
  else
    printf("UBLOX test failed\n\n");
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

struct sat_data {
  int sv_id_;
  int elev_;
  int az_;
  int cn0_;
};

// GPS value structure
#define MAX_SATS 32
struct gpm_msg_values {
  bool date_valid_;
  int fix_type_;
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
  double geod_sep_meters_;
  int num_sats_;
  int num_sat_data_vals_;
  sat_data sd_[MAX_SATS];
};

void reset_gpm_location_data(gpm_msg_values* v) {
  v->location_valid_ = false;
  v->hour_ = 0;
  v->min_ = 0;
  v->seconds_ = 0;
  v->degrees_lat_ = 0;
  v->degrees_long_ = 0;
  v->alt_meters_ = 0;
  v->geod_sep_meters_ = 0;
  v->num_sats_ = 0;
  v->num_sat_data_vals_ = 0;
}

void reset_gpm_all_data(gpm_msg_values* v) {
  v->date_valid_ = false;
  v->year_ = 0;
  v->month_ = 0;
  v->day_ = 0;
  reset_gpm_location_data(v);
}

const char* mths[12] = {
  "January", "February", "March",
  "April", "May", "June",
  "July", "August", "September",
  "October", "November", "December"
};

bool system_date(gpm_msg_values* out) {
  time_t t = time(NULL);
  struct tm tm = *gmtime(&t);
  out->year_ = tm.tm_year + 1900;
  out->month_ = tm.tm_mon + 1;
  out->day_ = tm.tm_mday;
  out->date_valid_ = true;
  return true;
}

void fix_quality(int k) {
  switch(k) {
    default:
      return;
    case 0:
      printf("No fix, ");
      return;
    case 1:
      printf("Autonomous fix, ");
      return;
    case 2:
      printf("Differential fix, ");
      return;
    case 4:
    case 5:
      printf("RTK fix, ");
      return;
    case 6:
      printf("Dead reckoning fix, ");
      return;
  }
}

void print_gps_data(gpm_msg_values& out) {
  printf("\n  ");
  fix_quality(out.fix_type_);
  printf(" %d SVs\n", out.num_sats_);
  if (out.date_valid_ & (out.month_ >= 1 && out.month_ <= 12)) {
    printf("    Date: %04d %s %02d, ", out.year_, mths[out.month_ - 1],
        out.day_);
  }
  if (out.location_valid_) {
    printf("Time: %02d:%02d:%.4fZ\n", out.hour_, out.min_, out.seconds_);

    printf("    Lattitude: %.5f, Longitude: %.5f\n",
        out.degrees_lat_, out.degrees_long_);
    printf("    Mean altitude: %.4f (m), Geoid offset: %.4f (m), Altitude: %.4f (m)\n",
        out.alt_meters_, out.geod_sep_meters_,  out.alt_meters_ + out.geod_sep_meters_);
  }
  if (out.num_sat_data_vals_ > 0) {
    printf("    Satelittes:\n");
    for (int i = 0; i < out.num_sat_data_vals_; i++) {
      printf("      SV-ID: %d, Elevation: %d, Azimuth: %d, Signal quality: %d\n",
        out.sd_[i].sv_id_, out.sd_[i].elev_, out.sd_[i].az_, out.sd_[i].cn0_);
    }
  }
  printf("\n");
}

// NMEA message format
//  $GNGGA,HHMMSS.SSS,DDMM.MMMM,N/S,DDDMM.MMMM,E/W,n,NS...A*20
//  $GNRMC and $GNGLL messages also give location and time.
//  Examples:
//    $GNGLL,3745.5641,N,12226.3415,W,022529.000,A,A*5E
//    $GNGGA,022529.000,3745.5641,N,12226.3415,W,1,07,1.6,34.6,M,0.0,M,,*5A
//    $GNRMC,022529.000,A,3745.5641,N,12226.3415,W,0.56,0.00,230420,,,A*6D
//    $GNZDA,210543.000,21,03,2022,00,00*4B for date
//  or perhaps (if gps only)
//    $GPGLL,3745.5641,N,12226.3415,W,022529.000,A,A*5E
//    $GPGGA,022529.000,3745.5641,N,12226.3415,W,1,07,1.6,34.6,M,0.0,M,,*5A
//    $GPRMC,022529.000,A,3745.5641,N,12226.3415,W,0.56,0.00,230420,,,A*6D
//    $GPZDA,210543.000,21,03,2022,00,00*4B for date
//  GP is gps, GL is glonass, GA is galeleo, GB is Beidou, GN is anybody

const int gga_msg = 1;
const int zda_msg = 2;
const int gsv_msg = 3;
const int rmc_msg = 4;

int parseZDANMEAMessage(char* msg, struct gpm_msg_values* v) {
  //  $xxZDA,time,day,month,year,ltzh,ltzn*cs<CR><LF>
  char* time_string = find_string_in_msg("$GNZDA,", msg);
  if (time_string == NULL || *time_string == ',')
    return -1;
  char* day_string = find_string_in_msg(",", time_string);
  if (day_string == NULL || *day_string == ',')
    return -1;
  sscanf(day_string, "%02d", &(v->day_));
  char* month_string = find_string_in_msg(",", day_string);
  if (month_string == NULL || *month_string == ',')
    return -1;
  sscanf(month_string, "%02d", &(v->month_));
  char* year_string = find_string_in_msg(",", month_string);
  if (year_string == NULL || *year_string == ',')
    return -1;
  sscanf(year_string, "%04d", &(v->year_));
  v->date_valid_ = true;
  return zda_msg;
}

int parseGGANMEAMessage(char* msg, struct gpm_msg_values* v) {
  // $xxGGA,time,lat,NS,lon,EW,quality,numSV,HDOP,alt,altUnit,sep,sepUnit,diffAge,diffStation*cs<CR><LF>
  //    quality: 1, 2d; 2, 3d.
  //    sep: Geoid separation
  char* time_string = find_string_in_msg("$GNGGA,", msg);
  if (time_string == NULL)
    return -1;
  if (*time_string == ',')
    return -1;
  sscanf(time_string, "%02d%02d", &v->hour_, &v->min_);
  sscanf(time_string+4, "%lf", &v->seconds_);

  char* latitude_string = find_string_in_msg(",", time_string);
  if (latitude_string == NULL)
    return -1;
  if (*latitude_string == ',')
    return -1;
  int deglat;
  int mlat;
  int minlat;
  sscanf(latitude_string, "%02d%02d.%04d", &deglat, &mlat, &minlat);
  v->degrees_lat_ = ((double)deglat) + (((double)mlat) + ((double)minlat) / 10000.0) / 60.0;
  char* ns_string = find_string_in_msg(",", latitude_string);
  if (*ns_string == ',')
    return -1;
  if (ns_string == NULL || (*ns_string != 'N' && *ns_string != 'S'))
    return -1;
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
    return -1;
  if (*ew_string == ',')
    return -1;
  if (*ew_string == 'W')
    v->degrees_long_ *= -1.0;
  char* qual_str = find_string_in_msg(",", ew_string);
  if (qual_str == NULL)
    return -1;
  if (*qual_str == ',')
    return -1;
  sscanf(qual_str, "%d", &(v->fix_type_));

  char* sats_str = find_string_in_msg(",", qual_str);
  if (sats_str == NULL)
    return -1;
  if (*sats_str == ',')
    return -1;
  sscanf(sats_str, "%02d", &(v->num_sats_));

  char* alt_str = find_string_in_msg(",", sats_str);
  if (alt_str == NULL)
    return -1;
  if (*alt_str == ',')
    return -1;
  alt_str = find_string_in_msg(",", alt_str);
  sscanf(alt_str, "%lf", &(v->alt_meters_));

  char* alt_unit_str = find_string_in_msg(",", alt_str);
  if (alt_unit_str == NULL)
    return -1;
  if (*alt_unit_str != 'M')
    return -1;

  char* sep_str = find_string_in_msg(",", ++alt_unit_str);
  sscanf(sep_str, "%lf", &(v->geod_sep_meters_));

  v->location_valid_ = true;
  return gga_msg;
}

int parseGLLNMEAMessage(char* msg, struct gpm_msg_values* v) {
  //  $xxGLL,lat,NS,lon,EW,time,status,posMode*cs<CR><LF>
  return 0;
}

int parseRMCNMEAMessage(char* msg, struct gpm_msg_values* v) {
  //  $xxRMC,time,status,lat,NS,lon,EW,spd,cog,date,mv,mvEW,posMode,navStatus*cs<CR><LF>
  //  date is DDMMYY
  char* time_string = find_string_in_msg("$GNRMC,", msg);
  if (time_string == NULL)
    return 0;
  for (int i = 0; i < 8; i++) {
    time_string = find_string_in_msg(",", time_string);
    if (time_string == NULL)
      return 0;
  }
  int d = 0;
  int m = 0;
  int y = 0;
  sscanf(time_string, "%02d%02d%02d", &d, &m, &y);
  v->date_valid_ = true;
  v->year_ = y + 2000;
  v->month_ = m;
  v->day_ = d;
  return rmc_msg;
}

bool parseGSVNMEAMessage(char* msg, struct gpm_msg_values* v) {
  // $xxGSV,numMsg,msgNum,numSV{,svid,elv,az,cno},signalId*cs<CR><LF>
  // svid is in the range of 1 to 32 for GPS satellites, and
  // 33 to 64 for SBAS 
  // $GPGSV,2,1,08,05,29,046,32,13,,,32,15,35,118,40,18,64,322,40*4E
  //   Sigid: gps (1-32), SBAS (33-64), Galileo (211-246), Beidou (159-163), GLONASS (65-96)
  // For GLONASS: GLGSV
  return 0;
}

int parseGNSNMEAMessage(char* msg, struct gpm_msg_values* v) {
  //  $xxGNS,time,lat,NS,lon,EW,posMode,numSV,HDOP,alt,sep,diffAge,diffStation,navStatus*cs<CR><LF>
  // Posmode: Positioning mode (A-C/A, E-Dead reckoning, N- not valid, F, R - RTK, A/D-3,3d
  //   First character for GPS, second character for GLONASS
  //   Third character for Galileo, Fourth character for BeiDou
  return 0;
}

int parseNMEAMessage(char* msg, struct gpm_msg_values* v, char* mtype) {

  if (mtype != NULL) {
    if(strcmp("ZDA", mtype) ==0) {
    } else if(strcmp("ZDA", mtype) == 0) {
      return parseZDANMEAMessage(msg, v);
    } else if(strcmp("GGA", mtype) == 0) {
      return parseGGANMEAMessage(msg, v);
    } else if(strcmp("GLL", mtype) == 0) {
      return parseGLLNMEAMessage(msg, v);
    } else if(strcmp("RMC", mtype) == 0) {
      return parseRMCNMEAMessage(msg, v);
    } else if(strcmp("GSV", mtype) == 0) {
      return parseGSVNMEAMessage(msg, v);
    } else {
      return false;
    }
  }

  if (find_string_in_msg("$GNZDA,", msg) != NULL)
      return parseZDANMEAMessage(msg, v);
  if (find_string_in_msg("$GNGGA,", msg) != NULL)
      return parseGGANMEAMessage(msg, v);
  if (find_string_in_msg("$GPGSV,", msg) != NULL)
      return parseGSVNMEAMessage(msg, v);
#if 0
  if (find_string_in_msg("$GNGLL,", msg) != NULL)
      return parseGLLNMEAMessage(msg, v);
#endif
  if (find_string_in_msg("$GNRMC,", msg) != NULL)
      return parseRMCNMEAMessage(msg, v);

  return -1;
}

int read_line(int fd, byte* buf, int size) {
  clearBuf(buf, size);
  int n = 0;
  int k;

  while(n < (size-1)) {
    k = read(fd, &buf[n], 1);
    if (k < 0)
      return -1;
    if (buf[n++] == '\n')
      return n;
  }
  return -1;
}

bool get_date(int fd, gpm_msg_values* out) {
  byte buf[BUF_SIZE];
  int n = 0;
  int msg_count = 1;
  int trys = 0;

  while (1) { 
    usleep(short_wait);
    n = read_line(fd, buf, BUF_SIZE - 1);
    if (n <=  0) {
      printf("read returns %d\n", n);
      continue;
    }
    buf[n++] = 0;

    if (print_message && n > 5)
      printf("Message %2d: %s", msg_count++, (char*) buf);

    if (++trys > 100) {
        if (system_date(out))
          return true;
    }

   if (zda_msg == parseZDANMEAMessage((char*)buf, out))
      return true;
   if (rmc_msg == parseRMCNMEAMessage((char*)buf, out))
      return true;
  }
  return false;
}


bool get_location(int fd, gpm_msg_values* out) {
  int msg_type = 0;
  byte buf[BUF_SIZE];
  int n = 0;
  int msg_count = 1;

  while (1) { 
    usleep(short_wait);
    n = read_line(fd, buf, BUF_SIZE - 1);
    if (n <=  0) {
      printf("read returns %d\n", n);
      continue;
    }
    buf[n++] = 0;

    if (print_message && n > 5)
      printf("Message %2d: %s", msg_count++, (char*) buf);

#if 0
#endif

    // if (gga_msg == parseGGANMEAMessage((char*)buf, out))
    if (gga_msg == parseNMEAMessage((char*)buf, out, NULL))
      return true;
  }
  return false;
}


int main(int an, char** av) {
  int baudRate = 9600;
  int num_repeat = 2;
  const char* uartDevice = default_uartDevice;
  gpm_msg_values out;

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

  struct termios options;
  tcgetattr(fd, &options);
  options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd, TCSANOW, &options);

  // setup gps sensor and get location
  reset_gpm_all_data(&out);
  setup_gps(fd);
  if (get_date(fd, &out)) {
    printf("\n");
    // print_message = false;  // change this later
    for (int i = 0; i < num_repeat; i++) {
      if (get_location(fd, &out)) {
        print_gps_data(out);
        reset_gpm_location_data(&out);
      }
    }
  }

  close(fd);
  return 0;
}

