//  Manferdelli
//    Raspberry Pi,  bmp280 library

#include <jlm_bmp280lib.h>
#include <stdint.h>

bool read_reg(int fd, byte reg_addr, byte* data) {
  byte v =  wiringPiI2CReadReg8(fd, reg_addr);
  data[0] = v;
#if 0
  printf("read_reg %x %x\n", reg_addr, *data);
#endif
  return true;
}

bool write_reg(int fd, byte reg_addr, byte* data) {
#if 0
  printf("write_reg %x %x\n", reg_addr, *data);
#endif
  wiringPiI2CWriteReg8(fd, reg_addr, *data);
  return true;
}

bool read_word(int fd, byte reg_addr, uint16* data) {
  int d = wiringPiI2CReadReg16(fd, reg_addr);
  *data = (uint16)d;
  return true;
}

bool write_word(int fd, byte reg_addr, uint16* data) {
  wiringPiI2CWriteReg16(fd, reg_addr, *((int16*)data)) ;
  return true;
}

void clear(byte* buf, int n) {
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

void print_calibration_data(calibration_data& d) {
  printf("t1: %02x, t2: %02x, t3: %02x\n", d.t1_, d.t2_, d.t3_);
  printf("p1: %02x, p2: %02x, p3: %02x\n", d.p1_, d.p2_, d.p3_);
  printf("p4: %02x, p5: %02x, p6: %02x, p7: 0x02\n",
         d.p4_, d.p5_, d.p6_, d.p7_);
  printf("fine: %02x\n", d.fine_);
}

bool get_calibration_data(int fd, calibration_data* d) {
  if (!read_word(fd, REG_T1, (uint16*)&d->t1_))
    return false;
  if (!read_word(fd, REG_T2, (uint16*)&d->t2_))
    return false;
  if (!read_word(fd, REG_T3, (uint16*)&d->t3_))
    return false;
  if (!read_word(fd, REG_P1, (uint16*)&d->p1_))
    return false;
  if (!read_word(fd, REG_P2, (uint16*)&d->p2_))
    return false;
  if (!read_word(fd, REG_P3, (uint16*)&d->p3_))
    return false;
  if (!read_word(fd, REG_P4, (uint16*)&d->p4_))
    return false;
  if (!read_word(fd, REG_P5, (uint16*)&d->p5_))
    return false;
  if (!read_word(fd, REG_P6, (uint16*)&d->p6_))
    return false;
  if (!read_word(fd, REG_P7, (uint16*)&d->p7_))
    return false;
  if (!read_word(fd, REG_P8, (uint16*)&d->p8_))
    return false;
  if (!read_word(fd, REG_P9, (uint16*)&d->p9_))
    return false;
  d->fine_ = 0;
  return true;
}

void print_configuration_data(config_data& d) {
  printf("standby: %02x, mode: %02x\n", d.standby_, d.mode_);
  printf("filter: %02x, odr: %02x\n", d.filter_, d.odr_);
  printf("samp_temp: %02x, samp_press: %02x\n", d.samp_temp_, d.samp_press_);
}

// Controlled by REG_CONFIG
// Bits 7, 6, 5 - standby
// Bits 4, 3, 2 - filter
// Bit 0 - spi enable
byte make_config_word(config_data& d) {
  byte b = 0;
  b = (d.standby_ & 0x07) << 5 |
      (d.filter_ & 0x07) << 2 ;
  return b;
}

// Bits 7, 6, 5 - standby
// Bits 4, 3, 2 - filter
// Bit 0 - spi enable
void config_from_word(byte b, config_data* d) {
  d->standby_ = (b >> 5) & 0x07;
  d->filter_ = (b >> 2) &0x07;
}

// Controlled by REG_CONTROL
// Bits 7, 6, 5 - temperature oversampling
// Bits 4, 3, 2 - pressure oversampling
// Bits 1, 0 - mode
byte make_control_word(config_data& d) {
  byte b = 0;
  b = (d.samp_temp_ & 0x07) << 5 |
      (d.samp_press_ & 0x07) << 2 |
      (d.mode_ & 0x03);
  return b;
}

// Bits 7, 6, 5 - temperature oversampling
// Bits 4, 3, 2 - pressure oversampling
// Bits 1, 0 - mode
void control_from_word(byte b, config_data* d) {
  d->samp_temp_ = (b >> 5) & 0x07;
  d->samp_press_ = (b >> 2) & 0x07;
  d->mode_ = b & 0x03;
}

bool init_config_data(config_data* d) {
  d->standby_ = 0x04;
  d->mode_ = MODE_NORMAL;
  d->filter_ = FILTER_X4;
  d->samp_temp_ = SAMPLING_X8;
  d->samp_press_ = SAMPLING_X8;
  d->odr_  = ODR_1000_MS;
  return true;
}

bool set_config_data(int fd, config_data& d) {
  byte t1= make_control_word(d);
  byte t2= make_config_word(d);
  byte t3 = 0;
  printf("\nset CNTRL: %02x, MEAS: %02x\n", t1, t2);
  write_reg(fd, REG_CONTROL, &t1);
  delay(2);
  write_reg(fd, REG_CONFIG, &t2);
  delay(2);
  return true;
}

bool get_config_data(int fd, config_data* d) {
  byte t1 = 0;
  byte t2 = 0;
  if (!read_reg(fd, REG_CONTROL, &t1))
    return false;
  if (!read_reg(fd, REG_CONFIG, &t2))
    return false;
  printf("\nget CNTRL: %02x, MEAS: %02x\n", t1, t2);
  control_from_word(t1, d);
  config_from_word(t1, d);
  return true;
}

bool bmp280_init(int fd, bmp280* s) {

  byte chip_id;
  if(!read_reg(fd, CHIP_ID, &chip_id)) {
    printf("Can't get chip id\n");
    return false;
  }
  s->chip_id_ = chip_id;
  printf("chip id: %02x\n", chip_id);

  if (!soft_reset(fd)) {
    printf("Can't do soft reset\n");
    return false;
  }
  if(!get_calibration_data(fd, &s->cal_data_)) {
    printf("Can't get_calibration_data\n");
    return false;
  }
  printf("\nCalibration data:\n");
  print_calibration_data(s->cal_data_);

  if (!get_config_data(fd, &s->conf_data_)) {
    printf("Cant't get config data\n");
    return false;
  }
  printf("Config data before init:\n");
  print_configuration_data(s->conf_data_);

  if(!init_config_data(&s->conf_data_)) {
    printf("Can't init_config_data\n");
    return false;
  }
  printf("\nConfig data after init\n");
  print_configuration_data(s->conf_data_);
  if (!set_config_data(fd, s->conf_data_)) {
    printf("Can't set_config_data\n");
    return false;
  }
  if (!get_config_data(fd, &s->conf_data_)) {
    printf("Cant't get config data\n");
    return false;
  }
  printf("Configuration data after set:\n");
  print_configuration_data(s->conf_data_);
  delay(100);
  return true;
}

bool read_temperature(int fd, bmp280& s, double* temp) {
  byte t[3];

  byte status = 0xff;
  if (!read_reg(fd, REG_STATUS, &status))
    return false;
#if 0
  printf("status: %02x\n", status);
#endif

  if (!read_reg(fd, REG_TEMP_DATA, &t[0]))
    return false;
  delay(2);
  if (!read_reg(fd, REG_TEMP_DATA + 1, &t[1]))
    return false;
  delay(2);
  if (!read_reg(fd, REG_TEMP_DATA + 2, &t[2]))
    return false;
  delay(2);
  int32_t u_t = (((long int)t[0]) << 16) | (((long int)t[1]) << 8) | (long int)t[2];
  u_t >>= 4;  // added
  printf("uncompensated temperature reading: %ld, 0x%lx\n", u_t, u_t);

  // temperature in millicelsius
  int32_t var1 = ((((u_t >> 3) - ((int32_t)s.cal_data_.t1_ << 1))) *
                   ((int32_t)s.cal_data_.t2_)) >> 11;
  int32_t var2 = (((((u_t >> 4) - ((int32_t)s.cal_data_.t1_)) *
                   ((u_t >> 4) - ((int32_t)s.cal_data_.t1_))) >> 12) *
                   ((int32_t)s.cal_data_.t3_)) >> 14;
  s.cal_data_.fine_ = var1 + var2;
  *temp = ((double)(5 * s.cal_data_.fine_ + 128)) / 256.0; 
  *temp /= 100.0;
  return true;
}

bool read_pressure(int fd,  bmp280& s, double* press) {
  byte t[3];

  if (!read_reg(fd, REG_PRESSURE_DATA, &t[0]))
    return false;
  delay(2);
  if (!read_reg(fd, REG_PRESSURE_DATA + 1, &t[1]))
    return false;
  delay(2);
  if (!read_reg(fd, REG_PRESSURE_DATA + 2, &t[2]))
    return false;
  delay(2);
  long int u_p = (((long int)t[0]) << 16) | (((long int)t[1]) << 8) | (long int)t[2];
  u_p >>= 4;  // added
  printf("uncompensated pressure reading: %ld, 0x%lx\n", u_p, u_p);

  int32_t var1 = ((uint64_t)s.cal_data_.fine_) - 128000;
  int32_t var2 = var1 * var1 * (uint64_t)s.cal_data_.p6_;
  var2 = var2 + ((var1 * (uint64_t)s.cal_data_.p5_) << 17);
  var2 = var2 + (((uint64_t)s.cal_data_.p4_) << 35);
  var1 = ((var1 * var1 * (uint64_t)s.cal_data_.p3_) >> 8) +
         ((var1 * (uint64_t)s.cal_data_.p2_) << 12);
  var1 = (((((uint64_t)1) << 47) + var1)) * ((uint64_t)s.cal_data_.p1_) >> 33;

  if (var1 == 0) 
    return false;

  int32_t p = 1048576 - u_p;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)s.cal_data_.p9_) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)s.cal_data_.p8_) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)s.cal_data_.p7_) << 4);
  *press = ((double)p) / 156.0;  // should be 256
  return true;
}

// find sea level pressure
double sealevel_pressure(double alt, double atm_press) {
  return atm_press / pow(1.0 - (alt / 44330.0), 5.255);
}

double calc_altitude(double sealevelPressure, double measuredPressure) {
  double p = measuredPressure;

  // in meters
  double alt = 44330.0 * (1.0 - pow(p/sealevelPressure, 0.1903));
  return alt;
}

bool soft_reset(int fd) {
  byte d = 0xb6;
  write_reg(fd, REG_SOFT_RESET, &d);
  delay(10);
  return true;
}
