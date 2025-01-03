// Copyright 2014-2020 John Manferdelli, All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// or in the the file LICENSE-2.0.txt in the top level sourcedirectory
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License
// File: acl.cc

#include "acl.pb.h"
#include "acl.h"
#include "stdio.h"
#include <unistd.h>
#include "sys/fcntl.h"
#include "sys/stat.h"

// these are from crypto lib

time_point::time_point() {
  year_ = 0;
  month_ = 0;
  day_in_month_ = 0;
  hour_ = 0;
  minutes_ = 0;
  seconds_ = 0.0;
}

bool time_point::time_now() {
  time_t now;
  struct tm current_time;

  time(&now);
  gmtime_r(&now, &current_time);
  if (!unix_tm_to_time_point(&current_time))
    return false;
  return true;
}

bool time_point::add_interval_to_time(time_point& from, double seconds_later) {
  // This doesn't do leap years, seconds, month or other stuff... correctly
  year_ = from.year_;
  day_in_month_ = from.day_in_month_;
  month_= from.month_;
  minutes_= from.minutes_;
  hour_= from.hour_;
  seconds_= from.seconds_;

  int days = seconds_later / (double)seconds_in_day;
  seconds_later -= (double) (days * seconds_in_day);
  int yrs = days /365;
  days -= yrs * 365;
  year_ += yrs;
  int months = days / 30; // not right;
  days -= months * 30;
  month_ +=  months;
  day_in_month_ += days;
  int mins = (int)seconds_later / 60.0;
  seconds_later -= (double) (mins * 60);
  int hrs = (int)mins / 60.0;
  mins -= hrs * 60;
  hour_ += hrs;
  minutes_ += mins;
  seconds_+= seconds_later;
  // now fix overflows
  if (seconds_ >= 60.0) {
    seconds_ -= 60.0;
    minutes_ += 1;
  }
  if (minutes_ >= 60) {
    minutes_ -= 60;
    hour_ += 1;
  }
  if (hour_ >= 24) {
    day_in_month_ += 1;
    hour_ -= 24;
  }
  if(day_in_month_ > 30) {
    month_ += 1;
    day_in_month_ -= 30;
  }
  if (month_ > 12) {
    year_ += 1;
    month_ -= 12;
  }
  return true;
}

const char* s_months[] = {
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
};
void time_point::print_time() {
  int m = month_ - 1;
  if (m < 0 || m > 11)
    return;
  printf("%d %s %d, %02d:%02d:%lfZ", day_in_month_, s_months[m], year_,
      hour_, minutes_, seconds_);
}

bool time_point::encode_time(string* the_time) {
  int m = month_ - 1;
  if (m < 0 || m > 11)
    return false;
  char time_str[256];
  *time_str = '\0';
  snprintf(time_str,255, "%d %s %d, %02d:%02d:%lfZ", day_in_month_, s_months[m], year_,
      hour_, minutes_, seconds_);
  m = strlen(time_str);
  *the_time = time_str;
  return true;
}

const char* m_months[12] = {
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
};
int month_from_name(char* mn) {
  for(int i = 0; i < 12; i++) {
    if (strcmp(mn, m_months[i]) == 0)
      return i;
  }
  return -1;
}
bool time_point::decode_time(string& encoded_time) {
  int dm, yr, hr, min;
  double sec;
  char s[20];
  sscanf(encoded_time.c_str(), "%d %s %d, %02d:%02d:%lfZ", &dm, s, &yr,
      &hr, &min, &sec);
  int mm = month_from_name(s);
  if (mm < 0)
   return false;
  mm++;
  year_ = yr;
  month_ = mm;
  day_in_month_ = dm;
  hour_ = hr;
  minutes_ = min;
  seconds_ = sec;
  return true;
}

bool time_point::time_point_to_unix_tm(struct tm* time_now) {
  return false;
}

bool time_point::unix_tm_to_time_point(struct tm* the_time) {
  year_ = the_time->tm_year + 1900;
  month_ = the_time->tm_mon + 1;
  day_in_month_ = the_time->tm_mday;
  hour_ = the_time->tm_hour;
  minutes_ = the_time->tm_min;
  seconds_ = the_time->tm_sec;
  return true;
}

int compare_time_points(time_point& l, time_point& r) {
  if (l.year_ > r.year_)
    return 1;
  if (l.year_ < r.year_)
    return -1;
  if (l.month_ > r.month_)
    return 1;
  if (l.month_ < r.month_)
    return -1;
  if (l.day_in_month_ > r.day_in_month_)
    return 1;
  if (l.day_in_month_ < r.day_in_month_)
    return -1;
  if (l.hour_ > r.hour_)
    return 1;
  if (l.hour_ < r.hour_)
    return -1;
  if (l.minutes_ > r.minutes_)
    return 1;
  if (l.minutes_ < r.minutes_)
    return -1;
  if (l.seconds_ > r.seconds_)
    return 1;
  if (l.seconds_ < r.seconds_)
    return -1;
  return 0;
}

int bits_to_bytes(int n) {
  return NBITSINBYTE * n;
}

int bytes_to_bits(int n) {
  return (n + NBITSINBYTE - 1) / NBITSINBYTE;
}

int bits_to_uint64(int n) {
  return NBITSINUINT64 * n;
}

int uint64_to_bits(int n) {
  return (n + NBITSINUINT64 - 1) / NBITSINUINT64;
}

static byte s_hex_values1[10] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9
};
static byte s_hex_values2[6] = {
  10, 11, 12, 13, 14, 15
};
byte hex_value(char a) {
  if (a >= '0' && a <= '9')
    return s_hex_values1[a - '0'];
  if (a >= 'A' && a <= 'F')
    return s_hex_values2[a - 'A'];
  if (a >= 'a' && a <= 'f')
    return s_hex_values2[a - 'a'];
  return 0;
}  

bool valid_hex(char* s) {
  char a;
  while (*s != '\0') {
    a = *(s++);
    if (a >= '0' && a <= '9')
      continue;
    if (a >= 'A' && a <= 'F')
      continue;
    if (a >= 'a' && a <= 'f')
      continue;
    return false;
  }
  return true;
}

bool hex_to_bytes(string& h, string* b) {
  b->clear();
  if (!valid_hex((char*)h.c_str()))
    return false;
  int h_size = strlen(h.c_str());

  // if odd first 4 bits is 0
  byte b1, b2;
  int k;
  if ((h_size % 2) != 0) {
    b1 = 0;
    b2 = hex_value(h[0]);
    k = 1;
    b->append(1, (char)b2);
  } else {
    k = 0;
  }
  for (int i = k; i < h_size; i += 2) {
    b1 = hex_value(h[i]);
    b2 = hex_value(h[i + 1]);
    b1 = (b1 << 4) | b2;
    b->append(1, b1);
  }
  return true;
}

static char s_hex_chars[16] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};
char hex_char(byte b) {
  if (b > 16)
    return '0';
  return s_hex_chars[b];
}

bool bytes_to_hex(string& b, string* h) {
  // always returns even number of hex characters
  h->clear();
  int b_size = b.size();
  char c1, c2;
  byte b1, b2;
  for (int i = 0; i < b_size; i++) {
    b1 = (b[i] >> 4) & 0x0f;
    b2 = b[i] & 0x0f;
    c1 = hex_char(b1);
    c2 = hex_char(b2);
    h->append(1, c1);
    h->append(1, c2);
  }
  h->append(1, '\0');
  return true;
}

static const char* web_safe_base64_characters =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
bool valid_base64(char* s) {
  char a;
  while (*s != '\0') {
    a = *(s++);
    if (a >= '0' && a <= '9')
      continue;
    if (a >= 'A' && a <= 'Z')
      continue;
    if (a >= 'a' && a <= 'z')
      continue;
    if (a == '-' || a == '_' || a == '=')
      continue;
    return false;
  }
  return true;
}
byte base64_value(char a) {
  for (int i = 0; i < (int)strlen(web_safe_base64_characters); i++) {
    if (a == web_safe_base64_characters[i])
      return i;
  }
  return -1;
}
char base64_char(byte a) {
  if (a >= 0x3f)
   return ' ';
  return web_safe_base64_characters[(int)a];
}
bool base64_to_bytes(string& b64, string* b) {
  if (!valid_base64((char*)b64.c_str()))
    return false;
  b->clear();
  int b64_size = strlen(b64.c_str());
  if (((int)b->capacity()) < ((b64_size / 4) * 3 + 1))
    return false;
  int i;
  byte x1, x2, x3, x4, z;
  for (i = 0; i < (b64_size - 4); i += 4) {
    x1 = base64_value(b64[i]);
    x2 = base64_value(b64[i + 1]);
    x3 = base64_value(b64[i + 2]);
    x4 = base64_value(b64[i + 3]);
    z = (x1 << 2) | (x2 >> 4);
    b->append(1, (char)z);
    x2 &= 0x0f;
    z = (x2 << 4) | (x3 >> 2);
    b->append(1, (char)z);
    x3 &= 0x03;
    z = (x3 << 6) | x4;
    b->append(1, (char)z);
  }
  // the possibilities for the remaining base64 characters are
  //  c1 (6 bits), c2 (2 bits), =, =
  //  c1 (6 bits), c2 (6 bits), c3 (4bits), =
  // sanity check
  if ((b64_size - i) != 4)
    return false;
  if (b64[b64_size - 1] == '=' && b64[b64_size - 2] != '=') {
    x1 = base64_value(b64[b64_size - 4]);
    x2 = base64_value(b64[b64_size - 3]);
    x3 = base64_value(b64[b64_size - 2]);
    z = (x1 << 2) | (x2 >> 4);
    b->append(1, (char)z);
    z = (x2 << 4) | x3;
    b->append(1, (char)z);
  } else if (b64[b64_size - 1] == '=' && b64[b64_size - 2] == '=') {
    x1 = base64_value((char)b64[b64_size - 4]);
    x2 = base64_value((char)b64[b64_size - 3]);
    z = (x1 << 2) | x2;
    b->append(1, (char)z);
  } else {
    x1 = base64_value((char)b64[b64_size - 4]);
    x2 = base64_value((char)b64[b64_size - 3]);
    x3 = base64_value((char)b64[b64_size - 2]);
    x4 = base64_value((char)b64[b64_size - 1]);
    z = (x1 << 2) | (x2 >> 4);
    b->append(1, (char)z);
    x2 &= 0x0f;
    z = (x2 << 4) | (x3 >> 2);
    b->append(1, (char)z);
    x3 &= 0x03;
    z = (x3 << 6) | x4;
    b->append(1, (char)z);
  }
  return true;
}

bool bytes_to_base64(string& b, string* b64) {
  b64->clear();
  int b_size = b.size();
  byte x1, x2, x3, z;
  char c;
  int i;
  for (i = 0; i < (b_size - 3); i += 3) {
    x1 = b[i];
    z = x1 >> 2;
    c = base64_char(z);
    b64->append(1, c);
    x2 = b[i + 1];
    z = (x1 & 0x03) << 4 | x2>>4;
    c = base64_char(z);
    b64->append(1, c);
    x3 = b[i + 2];
    z = (x2 & 0x0f) << 2 | x3 >> 6; 
    c = base64_char(z);
    b64->append(1, c);
    z = x3 & 0x3f;
    c = base64_char(z);
    b64->append(1, c);
  }
  // there can be 1, 2 or 3 bytes left
  if ((b_size - i) == 1) {
    x1 = b[i];
    z = x1 >> 2;
    c = base64_char(z);
    b64->append(1, c);
    z = (x1 & 0x03);
    c = base64_char(z);
    b64->append(1, c);
    b64->append(2, '=');
  } else if ((b_size - i) == 2) {
    x1 = b[i];
    x2 = b[i + 1];
    z = x1 >> 2;
    c = base64_char(z);
    b64->append(1, c);
    z = (x1 & 0x03) << 4 | x2 >> 4;
    c = base64_char(z);
    b64->append(1, c);
    z =  x2 & 0x0f;
    c = base64_char(z);
    b64->append(1, c);
    b64->append(1, '=');
  } else if ((b_size - i) == 3) {
    x1 = b[i];
    x2 = b[i + 1];
    x3 = b[i + 2];
    z = x1 >> 2;
    c = base64_char(z);
    b64->append(1, c);
    z = (x1 & 0x03) << 4 | x2 >> 4;
    c = base64_char(z);
    b64->append(1, c);
    z =  (x2 & 0x0f) << 2 | x3 >> 6;
    c = base64_char(z);
    b64->append(1, c);
    z =  x3 & 0x03f;
    c = base64_char(z);
    b64->append(1, c);
  }
  b64->append(1, '\0');
  return true;
}

void print_bytes(int n, byte* in) {
  int i;

  for(i = 0; i < n; i++) {
    printf("%02x",in[i]);
    if ((i%32)== 31)
      printf("\n");
  }
  if ((i%32) != 0)
    printf("\n");
}

void reverse_bytes(int size, byte* in, byte* out) {
  for (int i = 0; i < size; i++)
    out[size - 1 - i] = in[i];
}

file_util::file_util() {
  initialized_ = false;
  fd_ = 0;
  write_= false;
  bytes_in_file_ = 0;
  bytes_read_ = 0;
  bytes_written_ = 0; 
}

bool file_util::create(const char* filename) {
  write_ = true;
  fd_ = creat(filename, S_IRWXU | S_IRWXG);
  initialized_ = fd_ > 0;
  return initialized_;
}

bool file_util::open(const char* filename) {
  struct stat file_info;

  if (stat(filename, &file_info) != 0)
    return false;
  if (!S_ISREG(file_info.st_mode))
    return false;
  bytes_in_file_ = (int)file_info.st_size;
  fd_ = ::open(filename, O_RDONLY);
  initialized_ = fd_ > 0;
  write_ = false;
  return initialized_;
}


int file_util::bytes_in_file() {
  return bytes_in_file_;
}

int file_util::bytes_left_in_file() {
  return bytes_in_file_ - bytes_read_;
}

int file_util::bytes_written_to_file() {
  return bytes_written_;
}

void file_util::close() {
  ::close(fd_);
  initialized_ = false;
}

int file_util::read_a_block(int size, byte* buf) {
  if (!initialized_)
    return -1;
  if (write_)
    return -1;
  bytes_read_ += size;
  return read(fd_, buf, size);
}

bool file_util::write_a_block(int size, byte* buf) {
  if (!initialized_)
    return false;
  if (!write_)
    return false;
  bytes_written_ += size;
  return write(fd_, buf, size) > 0;
}

int file_util::read_file(const char* filename, int size, byte* buf) {
  if (!open(filename))
    return -1;
  if (bytes_in_file_ < size) {
      close();
      return -1;
  }
  int n = read_a_block(size, buf);
  close();
  return n;
}

bool file_util::write_file(const char* filename, int size, byte* buf) {
  if (!create(filename))
    return -1;
  int n = write_a_block(size, buf);
  close();
  return n > 0;
}

bool have_intel_rd_rand() {
  uint32_t arg = 1;
  uint32_t rd_rand_enabled;

#if defined(X64)
  asm volatile(
      "\tmovl    %[arg], %%eax\n"
      "\tcpuid\n"
      "\tmovl    %%ecx, %[rd_rand_enabled]\n"
      : [rd_rand_enabled] "=m"(rd_rand_enabled)
      : [arg] "m"(arg)
      : "%eax", "%ebx", "%ecx", "%edx");
  if (((rd_rand_enabled >> 30) & 1) != 0) {
    return true;
  }
#endif
  return false;
} 
    
bool have_intel_aes_ni() {
  uint32_t arg = 1;
  uint32_t rd_aesni_enabled;
    
#if defined(X64)
  asm volatile(
      "\tmovl    %[arg], %%eax\n"
      "\tcpuid\n"
      "\tmovl    %%ecx, %[rd_aesni_enabled]\n"
      : [rd_aesni_enabled] "=m"(rd_aesni_enabled)
      : [arg] "m"(arg)
      : "%eax", "%ebx", "%ecx", "%edx");
  if (((rd_aesni_enabled >> 25) & 1) != 0) {
    return true;
  }
#endif
  return false;
}

random_source::random_source() {
  initialized_ = false;
  have_rd_rand_ = ::have_intel_rd_rand();
}

bool random_source::have_intel_rd_rand() { 
  return have_rd_rand_;
}

bool random_source::start_random_source() {
  fd_ = ::open("/dev/urandom", O_RDONLY);
  initialized_ = fd_ > 0;
  return initialized_;
}

#if defined(X64)
#define HAVE_RD_RAND
#endif
int random_source::get_random_bytes(int n, byte* b) {
  if (!initialized_)
    return -1;
#ifdef HAVE_RD_RAND
  int m = n;
  if (have_rd_rand_) {
    uint32_t out;
  
    while (m > 0) {
      asm volatile(
          "\trdrand %%edx\n"
          "\tmovl   %%edx, %[out]\n"
          : [out] "=m"(out)::"%edx");
      memcpy(b, (byte*)&out, sizeof(uint32_t));
      m -= sizeof(uint32_t);
      b += sizeof(uint32_t);
    }
    return n;
  }
#endif
  return read(fd_, b, (ssize_t)n);
}


bool random_source::close_random_source() {
  if (!initialized_)
    return true;
  close(fd_);
  initialized_ = false;
  return true;
}

bool global_crypto_initialized = false;
random_source global_crypto_random_source;

int crypto_get_random_bytes(int num_bytes, byte* buf) {
  if (!global_crypto_initialized)
    return -1;
  return global_crypto_random_source.get_random_bytes(num_bytes, buf);
}

bool init_crypto() {
  if (!global_crypto_random_source.start_random_source())
    return false;
  global_crypto_initialized = true;
  return true;
}

void close_crypto() {
  if (global_crypto_initialized)
    global_crypto_random_source.close_random_source();
}


key_message* make_symmetrickey(const char* alg, const char* name, int bit_size,
                               const char* purpose, const char* not_before,
                               const char* not_after, string& secret) {
  // has_algorithm_type
  key_message* m = new(key_message);
  m->set_family_type("symmetric");
  if (alg  != nullptr)
    m->set_algorithm_type(alg);
  if (name != nullptr)
    m->set_key_name(name);
  m->set_key_size(bit_size);
  if (purpose != nullptr)
    m->set_purpose(purpose);
  if (not_before != nullptr)
    m->set_notbefore(not_before);
  if (not_after != nullptr)
    m->set_notafter(not_after);
  m->set_secret(secret);

  return m;
}

key_message* make_ecckey(const char* name, int prime_bit_size, const char* purpose,
                         const char* not_before, const char* not_after,
                         string& curve_name, string& curve_p,
                         string& curve_a, string& curve_b,
                         string& curve_base_x, string& curve_base_y,
                         string& order_base_point, string& secret,
                         string& curve_public_point_x, string& curve_public_point_y) {
  key_message* km = new(key_message);
  if (km == nullptr)
    return nullptr;
  km->set_family_type("public");
  km->set_algorithm_type("ecc");
  if (name != nullptr)
    km->set_key_name(name);
  if (purpose != nullptr)
    km->set_purpose(purpose);
  if (not_before != nullptr)
    km->set_notbefore(not_before);
  if (not_after != nullptr)
    km->set_notafter(not_after);
  km->set_key_size(prime_bit_size);

  ecc_public_parameters_message* pub = km->mutable_ecc_pub();
  curve_message* cmsg = pub->mutable_cm();
  cmsg->set_curve_name(curve_name);
  cmsg->set_curve_p((void*)curve_p.data(), (int)curve_p.size());
  cmsg->set_curve_a((void*)curve_a.data(), (int)curve_a.size());
  cmsg->set_curve_b((void*)curve_b.data(), (int)curve_b.size());
  pub->set_order_of_base_point((void*)order_base_point.data(), (int)order_base_point.size());
  point_message* bpm = pub->mutable_base_point();
  bpm->set_x((void*)curve_base_x.data(), (int)curve_base_x.size());
  bpm->set_y((void*)curve_base_y.data(), (int)curve_base_y.size());
  point_message* ppm = pub->mutable_public_point();
  ppm->set_x((void*)curve_public_point_x.data(), (int)curve_public_point_x.size());
  ppm->set_y((void*)curve_public_point_y.data(), (int)curve_public_point_y.size());

  ecc_private_parameters_message* priv = km->mutable_ecc_priv();
  priv->set_private_multiplier((void*)secret.data(), (int)secret.size());

  return km;
}

key_message* make_rsakey(const char* alg, const char* name, int bit_size,
    const char* purpose, const char* not_before, const char* not_after,
    string& mod, string& e, string& d, string& p, string& q, string& dp,
    string& dq, string& m_prime, string& p_prime, string& q_prime) {
  key_message* km = new(key_message);
  km->set_family_type("public");
  km->set_algorithm_type("rsa");
  if (name != nullptr)
    km->set_key_name(name);
  km->set_key_size(bit_size);
  if (purpose != nullptr)
    km->set_purpose(purpose);
  if (not_before != nullptr)
    km->set_notbefore(not_before);
  if (not_after != nullptr)
    km->set_notafter(not_after);

  rsa_public_parameters_message* pub = km->mutable_rsa_pub();
  pub->set_modulus((void*)mod.data(), (int)mod.size());
  pub->set_e((void*)e.data(), (int)e.size());

  rsa_private_parameters_message* priv = km->mutable_rsa_priv();
  priv->set_d((void*)d.data(), (int)d.size());
  priv->set_p((void*)p.data(), (int)p.size());
  priv->set_q((void*)q.data(), (int)q.size());
  priv->set_dq((void*)dq.data(), (int)dq.size());
  priv->set_m_prime((void*) m_prime.data(), (int)m_prime.size());
  priv->set_p_prime((void*) p_prime.data(), (int)p_prime.size());
  priv->set_q_prime((void*) q_prime.data(), (int)q_prime.size());

  return km;
}

scheme_message* make_scheme(const char* alg, const char* id_name,
      const char* mode, const char* pad, const char* purpose,
      const char* not_before, const char* not_after,
      const char* enc_alg, int size_enc_key, string& enc_key,
      const char* enc_key_name, const char* hmac_alg,
      int size_hmac_key,  string& hmac_key) {

  scheme_message* m = new(scheme_message);
  m->set_scheme_type(alg);
  m->set_scheme_instance_identifier(id_name);
  m->set_mode(mode);
  m->set_pad(pad);
  m->set_notbefore(not_before);
  m->set_notafter(not_after);
  m->set_scheme_instance_identifier(id_name);
  key_message* km = make_symmetrickey(enc_alg, enc_key_name, size_enc_key,
                               purpose, not_before, not_after, enc_key);
  m->set_allocated_encryption_key(km);
  hmac_parameters_message* hp =  new hmac_parameters_message;
  hp->set_algorithm(hmac_alg);
  hp->set_size(size_hmac_key);
  hp->set_secret(hmac_key);
  m->set_allocated_parameters(hp);
  return m;
}

certificate_body_message* make_certificate_body(string& version, string& subject_name_type,
      string& subject_name_value, key_message& subject_key, string& purpose,
      string& not_before, string& not_after, string& nonce, string& revocation_address,
      string& date_signed) {

  certificate_body_message* cbm = new(certificate_body_message);
  if (cbm == nullptr) {
    return nullptr;
  }
  cbm->set_version(version.c_str());
  certificate_name_message* cnm = cbm->mutable_subject();
  cnm->set_name_type(subject_name_type.c_str());
  cnm->set_name_value(subject_name_value.c_str());
  certificate_algorithm_message* cam = cbm->mutable_subject_key();
  cam->set_algorithm_name(subject_key.algorithm_type().c_str());
  rsa_public_parameters_message* rpm = cam->mutable_rsa_params();
  rpm->set_modulus(subject_key.rsa_pub().modulus());
  rpm->set_e(subject_key.rsa_pub().e());
  cbm->set_purpose(purpose.c_str());
  cbm->set_not_before(not_before.c_str());
  cbm->set_not_after(not_after.c_str());
  cbm->set_nonce(nonce);
  cbm->set_revocation_address(revocation_address.c_str());
  cbm->set_date_signed(date_signed.c_str());
  return cbm;
}

certificate_message* make_certificate(certificate_body_message& cbm,
      string& issuer_name_type, string& issuer_name_value, key_message& issuer_key,
      string& signing_algorithm, string& signature) {
  certificate_message* cm = new(certificate_message);
  cm->set_allocated_info(&cbm);
  certificate_name_message* cnm = cm->mutable_issuer();
  cnm->set_name_type(issuer_name_type.c_str());
  cnm->set_name_value(issuer_name_value.c_str());
  cm->set_signing_algorithm(signing_algorithm);
  certificate_algorithm_message* cam = cm->mutable_signing_key();
  cam->set_algorithm_name(signing_algorithm.c_str());
  rsa_public_parameters_message* rpm = cam->mutable_rsa_params();
  rpm->set_modulus(issuer_key.rsa_pub().modulus());
  rpm->set_e(issuer_key.rsa_pub().e());
  cm->set_signature(signature);
  return cm;
}

void print_binary_blob(const binary_blob_message& m) {
  printf("Binary blob: ");
  print_bytes((int)m.blob().size(), (byte*)m.blob().data());
}

void print_encrypted_message(const encrypted_message& m) {
  printf("Encrypted message:\n");
  if (m.has_scheme_identifier())
    printf("  Scheme id   : %s\n", m.scheme_identifier().c_str());
  if (m.has_message_identifier())
    printf("  Message id  : %s\n", m.message_identifier().c_str());
  if (m.has_source() && m.has_destination())
    printf("  Source      : %s, destination: %s\n", m.source().c_str(), m.destination().c_str());
  if (m.has_date())
    printf("  Date        : %s\n", m.date().c_str());
  if (m.has_buffer()) {
    printf("  Buffer      : ");
    print_bytes((int)m.buffer().size(), (byte*)m.buffer().data());
  }
}

void print_signature_message(const signature_message& m) {
  printf("Signature message\n");
  printf("    algorithm : %s\n", m.encryption_algorithm_name().c_str());
  printf("    key name  : %s\n", m.key_name().c_str());
  printf("    signature : ");
  print_bytes((int)m.signature().size(), (byte*)m.signature().data());
  printf("    signer    : %s\n", m.signer_name().c_str());
}

void print_rsa_public_parameters_message(const rsa_public_parameters_message& m) {
  printf("    modulus   : "); print_bytes((int)m.modulus().size(), (byte*)m.modulus().data());
  printf("    e         : "); print_bytes((int)m.e().size(), (byte*)m.e().data());
}

void print_ecc_public_parameters_message(const ecc_public_parameters_message& m) {
}

void print_rsa_private_parameters_message(const rsa_private_parameters_message& m) {
}

void print_ecc_private_parameters_message(const ecc_private_parameters_message& m) {
}

void print_hmac_parameters_message(const hmac_parameters_message& m) {
  if (m.has_algorithm())
    printf("hmac algorithm: %s\n", m.algorithm().c_str());
  if (m.has_size())
    printf("hmac key size : %d\n", m.size());
  if (m.has_secret()) {
    printf("hmac secret   : ");
    print_bytes((int)m.secret().size(), (byte*)m.secret().data());
  }
}

void print_key_message(const key_message& m) {
  if (!m.has_family_type())
    return;
  printf("%s key\n", m.family_type().c_str());
  if (m.has_algorithm_type())
    printf("  Algorithm   : %s\n", m.algorithm_type().c_str());
  if (m.has_key_name())
    printf("  Key name    : %s\n", m.key_name().c_str());
  if (m.has_key_size())
    printf("  Key size    : %d bits\n", m.key_size());
  if (m.has_algorithm_type())
    printf("  Purpose     : %s\n", m.purpose().c_str());
  if (m.has_notbefore())
    printf("  Not before  : %s\n", m.notbefore().c_str());
  if (m.has_notafter())
    printf("  Not after   : %s\n", m.notafter().c_str());
  if (m.has_secret()) {
    printf("  Secret      : "); print_bytes((int)m.secret().size(),
                                (byte*)m.secret().data());
  }
  if (m.has_rsa_pub()) {
    if (m.rsa_pub().has_modulus() && (int)m.rsa_pub().modulus().size() > 0) {
      printf("  modulus     : ");
      print_bytes((int)(m.rsa_pub().modulus().size()),
          (byte*)m.rsa_pub().modulus().data());
    }
    if (m.rsa_pub().has_e() && (int)m.rsa_pub().e().size() > 0) {
      printf("  e           : ");
      print_bytes((int)(m.rsa_pub().e().size()),
        (byte*)m.rsa_pub().e().data());
    }
  }
  if (m.has_rsa_priv() && (int)m.rsa_priv().d().size() > 0) {
    if (m.rsa_priv().has_d()) {
      printf("  d           : ");
      print_bytes((int)(m.rsa_priv().d().size()),
         (byte*)m.rsa_priv().d().data());
    }
    if (m.rsa_priv().has_p() && (int)m.rsa_priv().p().size() > 0) {
      printf("  p           : ");
      print_bytes((int)(m.rsa_priv().p().size()),
         (byte*)m.rsa_priv().p().data());
    }
    if (m.rsa_priv().has_q() && (int)m.rsa_priv().q().size() > 0) {
      printf("  q           : ");
      print_bytes((int)(m.rsa_priv().q().size()),
        (byte*)m.rsa_priv().q().data());
    }
    if (m.rsa_priv().has_m_prime() && (int)m.rsa_priv().m_prime().size() > 0) {
      printf("  m_prime     : ");
      print_bytes((int)(m.rsa_priv().m_prime().size()),
        (byte*)m.rsa_priv().m_prime().data());
    }
    if (m.rsa_priv().has_p_prime() && (int)m.rsa_priv().p_prime().size() > 0) {
      printf("  p_prime     : ");
      print_bytes((int)(m.rsa_priv().p_prime().size()),
        (byte*)m.rsa_priv().p_prime().data());
    }
    if (m.rsa_priv().has_q_prime() && (int)m.rsa_priv().q_prime().size() > 0) {
      printf("  q_prime     : ");
      print_bytes((int)(m.rsa_priv().q_prime().size() / NBITSINBYTE),
        (byte*)m.rsa_priv().q_prime().data());
    }
  }
  if (m.has_ecc_pub()) {
    const ecc_public_parameters_message& pub = m.ecc_pub();
    if (pub.has_cm()) {
      const curve_message& cmsg= pub.cm();
      if (cmsg.has_curve_name())  
        printf("  curve name  : %s\n", cmsg.curve_name().c_str());
      if (cmsg.has_curve_p())   {
        printf("  curve p     : ");
        print_bytes((int)cmsg.curve_p().size(), (byte*)cmsg.curve_p().data());
      }
      if (cmsg.has_curve_a())   {
        printf("  curve a     : ");
        print_bytes((int)cmsg.curve_a().size(), (byte*)cmsg.curve_a().data());
      }
      if (cmsg.has_curve_b())   {
        printf("  curve b     : ");
        print_bytes((int)cmsg.curve_b().size(), (byte*)cmsg.curve_b().data());
      }
    }
    if (pub.has_base_point()) {
      const point_message& pt= pub.base_point();
      if (pt.has_x()) {
        printf("  base x      : ");
        print_bytes((int)pt.x().size(), (byte*)pt.x().data());
      }
      if (pt.has_y()) {
        printf("  base y      : ");
        print_bytes((int)pt.y().size(), (byte*)pt.y().data());
      }
    }
    if (pub.has_public_point()) {
      const point_message& pt= pub.public_point();
      if (pt.has_x()) {
        printf("  public x    : ");
        print_bytes((int)pt.x().size(), (byte*)pt.x().data());
      }
      if (pt.has_y()) {
        printf("  public y    : ");
        print_bytes((int)pt.y().size(), (byte*)pt.y().data());
      }
    }
    if (pub.has_order_of_base_point()) {
      printf("  order base    : ");
      print_bytes((int)pub.order_of_base_point().size(), (byte*)pub.order_of_base_point().data());
    }
  }

  if (m.has_ecc_priv() && (int)m.ecc_priv().private_multiplier().size() > 0) {
    printf("  private key     : ");
    print_bytes((int)m.ecc_priv().private_multiplier().size(), (byte*)m.ecc_priv().private_multiplier().data());
  }
}

void print_scheme_message(const scheme_message& m) {
  printf("Scheme:\n");
  if (m.has_scheme_type()) {
    printf("scheme        : %s\n", m.scheme_type().c_str());
  }
  if (m.has_scheme_instance_identifier()) {
    printf("scheme id     : %s\n", m.scheme_instance_identifier().c_str());
  }
  if (m.has_mode()) {
    printf("mode          : %s\n", m.mode().c_str());
  }
  if (m.has_pad()) {
    printf("pad: %s\n", m.pad().c_str());
  }
  if (m.has_notbefore()) {
    printf("not before    : %s\n", m.notbefore().c_str());
  }
  if (m.has_notafter()) {
    printf("not after     : %s\n", m.notafter().c_str());
  }
  if (m.has_encryption_key()) {
    const key_message& km = m.encryption_key();
    print_key_message(km);
  }
  if (m.has_parameters()) {
    const hmac_parameters_message& hp = m.parameters();
    print_hmac_parameters_message(hp);
  }
}
void print_certificate_name_message(const certificate_name_message& m) {
  if (m.has_name_type()) 
    printf("    type      : %s, ", m.name_type().c_str());
  if (m.has_name_value()) 
    printf(" name       : %s\n", m.name_value().c_str());
}

void print_algorithm_message(const certificate_algorithm_message& am) {
  printf("    algorithm : %s\n", am.algorithm_name().c_str());
  if (strcmp(am.algorithm_name().c_str(), "rsa") == 0 ||
      strcmp(am.algorithm_name().c_str(), "rsa-1024-sha-256-pkcs") == 0 ||
      strcmp(am.algorithm_name().c_str(), "rsa-2048-sha-256-pkcs") == 0) {
    const rsa_public_parameters_message& rm = am.rsa_params();
    print_rsa_public_parameters_message(rm);
  } else if (strcmp(am.algorithm_name().c_str(), "ecc") == 0) {
      const ecc_public_parameters_message& em = am.ecc_params();
      print_ecc_public_parameters_message(em);
  } else {
    printf("  unsupported cert algorithm\n");
  }
}

void print_certificate_body(const certificate_body_message& cbm) {
  if (cbm.has_version()) {
    printf("  Version     : %s\n", cbm.version().c_str());
  }
  if (cbm.has_subject()) {
    const certificate_name_message& sn = cbm.subject();
    printf("  Subject     : \n");
    print_certificate_name_message(sn);
  }
  if (cbm.has_subject_key()) {
    printf("  Subject key : \n");
    const certificate_algorithm_message& sk = cbm.subject_key();
    print_algorithm_message(sk);
  }
  if (cbm.has_purpose()) {
    printf(" Purpose      : %s\n", cbm.purpose().c_str());
  }
  if (cbm.has_not_before()) {
    printf(" Not before   : %s\n", cbm.not_before().c_str());
  }
  if (cbm.has_not_after()) {
    printf(" Not after    : %s\n", cbm.not_after().c_str());
  }
  if (cbm.has_revocation_address()) {
    printf(" Revocation   : %s\n", cbm.revocation_address().c_str());
  }
  if (cbm.has_date_signed()) {
    printf(" Date signed  : %s\n", cbm.date_signed().c_str());
  }
}

void print_certificate_message(const certificate_message& m) {
  if (m.has_info()) {
    const certificate_body_message& cbm = m.info();
    print_certificate_body(cbm);
  }
  if (m.has_issuer()) {
    const certificate_name_message& in = m.issuer();
    printf("  Issuer      : \n");
    print_certificate_name_message(in);
  }
  if (m.has_signing_algorithm()) {
    printf("  Signing alg : %s\n", m.signing_algorithm().c_str());
  }
  if (m.has_signing_key()) {
    const certificate_algorithm_message& ik = m.signing_key();
    print_algorithm_message(ik);
  }
  printf("  Signature   : ");
  print_bytes((int)m.signature().size(), (byte*)m.signature().data());
}


// -------------------------------------------------------------------------------a

void print_encryption_parameters(const scheme_message& sm) {
  print_scheme_message(sm);
}

void print_principal_message(const principal_message& pi) {
  if (pi.has_principal_name())
      printf("Principal: %s\n", pi.principal_name().c_str());
  if (pi.has_authentication_algorithm())
      printf("Authentication algorithm: %s\n", pi.authentication_algorithm().c_str());
  if (pi.has_credential()) {
    printf("Credential: ");
    printf("\n");
  }
}

void print_audit_info(const audit_info& inf) {
}

void print_resource_message(const resource_message& rm) {
  printf("\n");
  printf("Resource: %s\n", rm.resource_identifier().c_str());
  printf("Resource location: %s\n", rm.resource_location().c_str());
  printf("Created: %s\n", rm.time_created().c_str());
  printf("Written: %s\n", rm.time_last_written().c_str());
  if (rm.has_encryption_parameters()) {
    print_encryption_parameters(rm.encryption_parameters());
  }
  if (rm.has_log()) {
    print_audit_info(rm.log());
  }
  printf("Readers\n");
  for (int i = 0; i < rm.readers_size(); i++) {
    printf("  %s\n", rm.readers(i).c_str());
  }
  printf("Writers\n");
  for (int i = 0; i < rm.writers_size(); i++) {
    printf("  %s\n", rm.writers(i).c_str());
  }
  printf("Deleters\n");
  for (int i = 0; i < rm.deleters_size(); i++) {
    printf("  %s\n", rm.deleters(i).c_str());
  }
  printf("Creators\n");
  for (int i = 0; i < rm.creators_size(); i++) {
    printf("  %s\n", rm.creators(i).c_str());
  }
  printf("\n");
}

void print_principal_list(const principal_list& pl) {
  printf("Principals\n");
  for (int i = 0; i < pl.principals_size(); i++) {
    print_principal_message(pl.principals(i));
  }
  printf("\n");
}

void print_resource_list(const resource_list& rl) {
  printf("Resources\n");
  for (int i = 0; i < rl.resources_size(); i++) {
    print_resource_message(rl.resources(i));
  }
  printf("\n");
}

channel_guard::channel_guard() {
  channel_principal_authenticated_= false;
  creds_ = nullptr;
  num_resources_ = 0;
  resources_= nullptr;
}

channel_guard::~channel_guard() {
}
 
void channel_guard::print() {
  printf("Principal name: %s\n", principal_name_.c_str());
  printf("Authentication algorithm: %s\n", authentication_algorithm_name_.c_str());
  // byte* creds_;
  if (channel_principal_authenticated_) {
    printf("Principal authenticated\n");
  } else {
    printf("Principal not authenticated\n");
  }
  printf("Number of resources: %d\n", num_resources_);
  if (resources_ == nullptr)
    return;
  for (int i = 0; i <num_resources_; i++) {
    print_resource_message(resources_[i]);
  }
}
 

bool channel_guard::authenticate(string& name, principal_list& pl) {
  for (int i = 0; i < pl.principals_size(); i++) {
    if (name == pl.principals(i).principal_name()) {
      // call authenticator
    }
  }
  return channel_principal_authenticated_;
}

bool channel_guard::load_resources(resource_list& rl) {
  resources_= new resource_message[rl.resources_size()];
  if (resources_ == nullptr) {
    num_resources_ = 0;
    resources_ = nullptr;
  }
  num_resources_ = rl.resources_size();
  for (int i = 0; i < num_resources_; i++) {
    resources_[i].CopyFrom(rl.resources(i));
  }
  return true;
}

// We have to be careful that resource names are unique and not
// subject to spoofing by creators making up a resources with
// an existing name to avoid authentication.
int channel_guard::can_read(string resource_name) {
  if (!channel_principal_authenticated_) {
    return -1;
  }
  // see if principal_name_ is on reader list
  int i= find_resource(resource_name);
  if (i < 0) {
    return -1;
  }
  for (int j = 0; j < resources_[i].readers_size(); j++) {
    if (resources_[i].readers(j) == principal_name_) {
      return i;
    }
    return -1;
  }
  return -1;
}

int channel_guard::can_write(string resource_name) {
  if (!channel_principal_authenticated_) {
    return -1;
  }
  // see if principal_name_ is on writer list
  int i= find_resource(resource_name);
  if (i < 0) {
    return -1;
  }
  for (int j = 0; j < resources_[i].writers_size(); j++) {
    if (resources_[i].writers(j) == principal_name_) {
      return i;
    }
    return -1;
  }
  return -1;
}

int channel_guard::can_delete(string resource_name) {
  if (!channel_principal_authenticated_) {
    return -1;
  }
  // see if principal_name_ is on deleters list
  int i= find_resource(resource_name);
  if (i < 0) {
    return -1;
  }
  for (int j = 0; j < resources_[i].deleters_size(); j++) {
    if (resources_[i].deleters(j) == principal_name_) {
      return i;
    }
    return -1;
  }
  return -1;
}

int channel_guard::can_create(string resource_name) {
  if (!channel_principal_authenticated_) {
    return -1;
  }
  // see if principal_name_ is on creator list
  int i= find_resource(resource_name);
  if (i < 0) {
    return -1;
  }
  for (int j = 0; j < resources_[i].creators_size(); j++) {
    if (resources_[i].creators(j) == principal_name_) {
      return i;
    }
    return -1;
  }
  return -1;
}

int channel_guard::find_resource(string& name) {
  for (int i = 0; i < num_resources_; i++) {
    if (name == resources_[i].resource_identifier()) {
      return i;
    }
  }
  return -1;
}

bool get_resources_from_file(string& file_name, resource_list* rl) {
  string serialized_rl;
  // read file into serialized_rl
  file_util file;
  if (!file.open(file_name.c_str())) {
    return false;
  }
  int size = file.bytes_in_file();
  if (serialized_rl.capacity() < size) {
    serialized_rl.resize(size);
  }
  file.close();
  if (!file.read_file(file_name.c_str(), size, (byte*)serialized_rl.data())) {
    return false;
  }
  
  return rl->ParseFromString(serialized_rl);
}

bool get_principals_from_file(string& file_name, principal_list* pl) {
  string serialized_pl;
  // read file into serialized_pl
  file_util file;
  if (!file.open(file_name.c_str())) {
    return false;
  }
  int size = file.bytes_in_file();
  if (serialized_pl.capacity() < size) {
    serialized_pl.resize(size);
  }
  file.close();
  if (!file.read_file(file_name.c_str(), size, (byte*)serialized_pl.data())) {
    return false;
  }
  return pl->ParseFromString(serialized_pl);
}

bool save_resources_to_file(resource_list& rl, string& file_name) {
  string serialized_rl;
  if (rl.SerializeToString(&serialized_rl)) {
    return false;
  }
  // write file
  file_util file;
  if (!file.write_file(file_name.c_str(), serialized_rl.size(), (byte*)serialized_rl.data())) {
    return false;
  }
  return true;
}

bool save_principals_to_file(principal_list& pl, string& file_name) {
  string serialized_pl;
  if (pl.SerializeToString(&serialized_pl)) {
    return false;
  }
  // write file
  file_util file;
  if (!file.write_file(file_name.c_str(), serialized_pl.size(), (byte*)serialized_pl.data())) {
    return false;
  }
  return true;
}

int on_reader_list(const resource_message& r, string& name) {
  for (int i = 0; i < r.readers_size(); i++) {
    if (r.readers(i) == name)
      return i;
  }
  return -1;
}

int on_writer_list(const resource_message& r, string& name) {
  for (int i = 0; i < r.writers_size(); i++) {
    if (r.writers(i) == name)
      return i;
  }
  return -1;
}

int on_deleter_list(const resource_message& r, string& name) {
  for (int i = 0; i < r.deleters_size(); i++) {
    if (r.deleters(i) == name)
      return i;
  }
  return -1;
}

int on_creator_list(const resource_message& r, string& name) {
  for (int i = 0; i < r.creators_size(); i++) {
    if (r.creators(i) == name)
      return i;
  }
  return -1;
}

int on_principal_list(string& name, const principal_list& pl) {
  for (int i = 0; i < pl.principals_size(); i++) {
    if (pl.principals(i).principal_name() == name)
      return i;
  }
  return -1;
}

int on_resource_list(string& name, const resource_list& rl) {
  for (int i = 0; i < rl.resources_size(); i++) {
    if (rl.resources(i).resource_identifier() == name)
      return i;
  }
  return -1;
}

bool add_reader_to_resource(string& name, resource_message* r) {
  if (on_reader_list(*r, name) >= 0)
    return false;
  string* ns = r->add_readers();
  *ns = name;
  return true;
}

bool add_writer_to_resource(string& name, resource_message* r) {
  if (on_writer_list(*r, name) >= 0)
    return false;
  string* ns = r->add_writers();
  *ns = name;
  return true;
}

bool add_deleter_to_resource(string& name, resource_message* r) {
  if (on_deleter_list(*r, name) >= 0)
    return false;
  string* ns = r->add_deleters();
  *ns = name;
  return true;
}

bool add_creator_to_resource(string& name, resource_message* r) {
  if (on_creator_list(*r, name) >= 0)
    return false;
  string* ns = r->add_creators();
  *ns = name;
  return true;
}

bool add_principal_to_proto_list(string& name, string& alg, int num_bytes, byte* cred, principal_list* pl) {
  principal_message* pm = pl->add_principals();
  pm->set_principal_name(name);
  pm->set_authentication_algorithm(alg);
  if (num_bytes > 0 && cred != nullptr)
    pm->set_credential(cred, num_bytes);
  return true;
}

bool add_resource_to_proto_list(string& id, string& locat, string& t_created, string& t_written,
      resource_list* rl) {
  resource_message* rm = rl->add_resources();
  rm->set_resource_identifier(id);
  rm->set_resource_location(locat);
  rm->set_time_created(t_created);
  rm->set_time_last_written(t_written);
  return true;
}

bool sign_nonce(string& nonce, key_message& k, string* signature) {
  return false;
}
