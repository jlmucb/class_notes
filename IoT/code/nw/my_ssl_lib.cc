// Copyright 2020 John Manferdelli, All Rights Reserved.
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
// File: my_ssl_lib.cc

#include "my_ssl_lib.h"
#include <time.h>

#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/hmac.h>

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

bool time_point_to_time_t(time_point& tp, time_t* tm) {
  struct tm utm;

  utm.tm_sec = tp.seconds_;
  utm.tm_min = tp.minutes_;
  utm.tm_hour = tp.hour_;
  utm.tm_mday = tp.day_in_month_;
  utm.tm_mon = tp.month_ - 1;
  utm.tm_year = tp.year_ - 1900;
  *tm = mktime (&utm);
  return true;
}

bool time_t_to_time_point(time_t& tm, time_point* tp) {
  return false;
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


sha256_digest::sha256_digest() {
  ctx_ = nullptr;
}

sha256_digest::~sha256_digest() {
  if (ctx_ != nullptr) {
    EVP_MD_CTX_free(ctx_);
    ctx_ = nullptr;
  }
}

bool sha256_digest::init() {
  if (ctx_ != nullptr)
    return true;
  ctx_ = EVP_MD_CTX_new();
  if (1 != EVP_DigestInit_ex(ctx_, EVP_sha256(), NULL))
    return false;
  return true;
}

bool sha256_digest::update(int size, byte* buf) {

  if (1 != EVP_DigestUpdate(ctx_, buf, size))
    return false;
  return true;
}

bool sha256_digest::finalize(byte* digest, unsigned int* size) {
  if (1 != EVP_DigestFinal_ex(ctx_, digest, size))
    return false;
  return true;
}


aes256_cbc::aes256_cbc() {
  ctx_ = nullptr;
  total_cipher_len_ = 0;
  total_plain_len_ = 0;
}

aes256_cbc::~aes256_cbc() {
  // clear key
  memset(key_, 0, 32);
  if (ctx_ != nullptr) {
    EVP_CIPHER_CTX_free(ctx_);
    ctx_ = nullptr;
  }
}

bool aes256_cbc::encrypt(byte* key, byte* iv, int plain_len, byte* plain, int* cipher_len, byte* cipher) {
  total_cipher_len_ = 0;
  total_plain_len_ = 0;
  int len;

  // make sure the output buf is big enough
  if (*cipher_len < (plain_len + 32))
    return false;

  if (ctx_ == nullptr)
    ctx_ = EVP_CIPHER_CTX_new();
  
  memcpy(key_, key, 32);
  if (1 != EVP_EncryptInit_ex(ctx_, EVP_aes_256_cbc(), NULL, key_, iv))
    return false;

  memcpy(cipher, iv, 16);
  cipher += 16;
  total_cipher_len_ += 16;

  if (1 != EVP_EncryptUpdate(ctx_, cipher, &len, plain, plain_len))
    return false;
  cipher += len;
  total_cipher_len_ += len;
  total_plain_len_ += plain_len;
  if (1 != EVP_EncryptFinal_ex(ctx_, cipher, &len))
    return false;
  total_cipher_len_ += len;
  *cipher_len = total_cipher_len_;
  return true;
}

bool aes256_cbc::decrypt(byte* key, int cipher_len, byte* cipher, int* plain_len, byte* plain) {
  total_cipher_len_ = 0;
  total_plain_len_ = 0;
  int len;

  // make sure the output buf is big enough
  if (*plain_len < (cipher_len - 16))
    return false;

  if (ctx_ == nullptr)
    ctx_ = EVP_CIPHER_CTX_new();

  memcpy(key_, key, 32);
  byte* iv = cipher;
  if (1 != EVP_DecryptInit_ex(ctx_, EVP_aes_256_cbc(), NULL, key, iv))
    return false;
  cipher += 16;
  cipher_len -= 16;
  total_cipher_len_ = 16;
  if (1 != EVP_DecryptUpdate(ctx_, plain, &len, cipher, cipher_len))
    return false;
  plain += len;
  total_cipher_len_ += cipher_len;
  total_plain_len_ += len;

  if (1 != EVP_DecryptFinal_ex(ctx_, plain, &len))
    return false;
  total_plain_len_ += len;
  *plain_len = total_plain_len_;

  return true;
}

bool aes256_cbc::get_key(byte* out) {
  memcpy(out, key_, 32);
  return false;
}


authenticated_aes256_cbc::authenticated_aes256_cbc() {
  total_cipher_len_ = 0;
  total_plain_len_ = 0;
  }

  authenticated_aes256_cbc::~authenticated_aes256_cbc() {
  }

  bool authenticated_aes256_cbc::authenticated_encrypt(byte* key, byte* iv,
        int plain_len, byte* plain, int* cipher_len, byte* cipher) {
  aes256_cbc enc; 

  // make sure the output buf is big enough
  if (*cipher_len < (plain_len + 64))
    return false;

  int  len = *cipher_len;
  if (!enc.encrypt(key, iv, plain_len, plain, &len, cipher))
    return false;

  unsigned int  md_len = 0;
  HMAC(EVP_sha256(), (const void*)&key[32], 32, cipher, len, cipher + len, &md_len);
  *cipher_len = len + ((int)md_len);
  return true;
  }

  bool authenticated_aes256_cbc::authenticated_decrypt(byte* key,
        int cipher_len, byte* cipher, int* plain_len, byte* plain) {
  aes256_cbc dec; 
  byte hmac_digest[32];
  memset(hmac_digest, 0, 32);

  unsigned int md_len = 32;
  HMAC(EVP_sha256(), (const void*)&key[32], 32, cipher, cipher_len - 32, hmac_digest, &md_len);
  if (memcmp(hmac_digest, cipher + cipher_len - 32, 32) != 0)
    return false;

  if (!dec.decrypt(key, cipher_len - 32, cipher, plain_len, plain))
    return false;
  return true;
}

bool authenticated_aes256_cbc::get_key(byte* out) {
  memcpy(out, key_, 64);
  return true;
}

// EM = 0x00 || 0x02 || PS || 0x00 || M to fill buffer

rsa_implement::rsa_implement() {
  key_initialized_ = false;
  int bit_size_ = 0;
  rsa_key_ = nullptr;
}

rsa_implement::~rsa_implement() {
  if (rsa_key_ != nullptr)
    RSA_free(rsa_key_);
  key_initialized_ = false;
  int bit_size_ = 0;
  rsa_key_ = nullptr;
}

bool rsa_implement::generate_key(int num_bits) {
  BIGNUM* e = BN_new();
  BN_set_word(e, 65537UL);
  rsa_key_ = RSA_new();
  if (0 == RSA_generate_key_ex(rsa_key_, num_bits, e, nullptr))
    return false;
  key_initialized_ = true;
  BN_free(e);
  int bit_size_ = num_bits;
  return true;
}

bool rsa_implement::encrypt(int padding, int plain_len, byte* plain,
        int* cipher_size, byte* cipher) {

  if (!key_initialized_)
    return false;

  // one block only for now
  if (*cipher_size <  RSA_size(rsa_key_))
    return false;

  int size_out = RSA_public_encrypt(plain_len, plain, cipher, rsa_key_, padding);
  if (size_out <= 0)
    return false;
  *cipher_size = size_out;
  return true;
}

bool rsa_implement::decrypt(int padding, int cipher_len, byte* cipher,
        int* plain_len, byte* plain) {

  if (!key_initialized_)
    return false;
  // one block only for now
  if (*plain_len <  RSA_size(rsa_key_))
    return false;

  int size_out = RSA_private_decrypt(cipher_len, cipher, plain, rsa_key_, padding);
  if (size_out <= 0)
    return false;
  *plain_len = size_out;
  return true;
}

bool rsa_implement::get_m(byte* out) {
  // void RSA_get0_key(const RSA *r, const BIGNUM **n, const BIGNUM **e, const BIGNUM **d);,

  return true;
}

bool rsa_implement::get_e(byte* out) {
  // void RSA_get0_key(const RSA *r, const BIGNUM **n, const BIGNUM **e, const BIGNUM **d);,
  return true;
}

bool rsa_implement::get_d(byte* out) {
  return true;
}

bool rsa_implement::set_key(int num_bits, byte* m, int size_e, byte* e,
      int size_d, byte* d) {
  //int RSA_set0_key(RSA *r, BIGNUM *n, BIGNUM *e, BIGNUM *d);
  return false;
}

bool rsa_implement::get_key(int* num_bits, int* size_m, byte* m, 
      int* size_e, byte* e, int* size_d, byte* d) {
  // RSA_get0_key(r, &m, &e, &d);
  return false;
}

my_x509::my_x509() {
  sn_ = 0ULL;
  issuer_key_ = nullptr;
  subject_key_ = nullptr;
  cert_ = nullptr;
}

my_x509::~my_x509() {
  if (issuer_key_ != nullptr) {
    RSA_free(issuer_key_);
    issuer_key_ = nullptr;
  }
  if (subject_key_ != nullptr) {
    RSA_free(subject_key_);
    subject_key_ = nullptr;
  }
  if (cert_ != nullptr) {
    X509_free(cert_);
    cert_ = nullptr;
  }
}

bool my_x509::init() {
  if (cert_ != nullptr)
    return true;
  cert_ = X509_new();
  return (cert_ != nullptr);
}

bool my_x509::generate_keys_for_test(int num_bits) {
  BIGNUM* e = BN_new();
  BN_set_word(e, 65537UL);

  subject_key_ = RSA_new();
  if (0 == RSA_generate_key_ex(subject_key_, num_bits, e, nullptr))
    return false;

  issuer_key_ = RSA_new();
  if (0 == RSA_generate_key_ex(issuer_key_, num_bits, e, nullptr))
    return false;

  BN_free(e);
  return true;
}

bool my_x509::sign_cert() {
  if (issuer_key_ == nullptr)
    return false;

  EVP_PKEY* k =  EVP_PKEY_new();
  if (k == nullptr)
    return false;

  if (0 == EVP_PKEY_set1_RSA(k, issuer_key_)) {
    EVP_PKEY_free(k);
    return false;
  }
  
  if (0 == X509_sign(cert_, k, EVP_sha256())) {
    EVP_PKEY_free(k);
    return false;
  }
  return true;
}

bool my_x509::verify_cert() {
  if (issuer_key_ == nullptr)
    return false;

  EVP_PKEY* k =  EVP_PKEY_new();
  if (k == nullptr)
    return false;

  if (0 == EVP_PKEY_set1_RSA(k, issuer_key_)) {
    EVP_PKEY_free(k);
    return false;
  }
  
  if (0 == X509_verify(cert_, k)) {
    EVP_PKEY_free(k);
    return false;
  }
  EVP_PKEY_free(k);
  return true;
}

bool get_m(string* m_str) {
  return false;
}

bool set_m(string& m_str) {
  return false;
}

bool my_x509::get_e(string* e_str ) {
  return false;
}

bool my_x509::set_e(string& e_str ) {
  return false;
}

bool my_x509::get_d(string* d_str) {
  return false;
}

bool my_x509::set_d(string& d_str) {
  return false;
}

bool my_x509::get_sn(uint64_t* sn) {
  return false;
}

bool my_x509::set_sn(uint64_t sn) {
  return false;
}

bool my_x509::get_not_before(string* nb) {
  return false;
}

bool my_x509::set_not_before(string& nb) {
  return false;
}

bool my_x509::get_not_after(string* na) {
  return false;
}

bool my_x509::set_not_after(string& na) {
  return false;
}

bool my_x509::get_subject_name(string* subject_name) {
  return false;
}

bool my_x509::set_subject_name(string& subject_name) {
  return false;
}

bool my_x509::get_issuer_name(string* issuer_name) {
  return false;
}

bool my_x509::set_issuer_name(string& issuer_name) {
  return false;
}

bool my_x509::get_subject_key(RSA* out) {
  return false;
}

bool my_x509::set_subject_key(RSA* in) {
  return false;
}

bool my_x509::get_issuer_key(RSA* out) {
  return false;
}

bool my_x509::set_issuer_key(RSA* in) {
  return false;
}

bool my_x509::load_cert_values() {

  if (cert_ == nullptr)
    return false;

  ASN1_INTEGER* a = ASN1_INTEGER_new();
  ASN1_INTEGER_set_uint64(a, sn_);
  X509_set_serialNumber(cert_, a);
  ASN1_INTEGER_free(a);

  X509_NAME* subject_name = X509_NAME_new();
  X509_NAME* issuer_name = X509_NAME_new();
  if (0 == X509_NAME_add_entry_by_txt(subject_name, "CN", MBSTRING_ASC, (const byte*)subject_name_.c_str(), -1, -1, 0)) {
    X509_NAME_free(subject_name);
    X509_NAME_free(issuer_name);
    return false;
  }
  if (0 == X509_NAME_add_entry_by_txt(issuer_name, "CN", MBSTRING_ASC, (const byte*) issuer_name_.c_str(), -1, -1, 0)) {
    X509_NAME_free(subject_name);
    X509_NAME_free(issuer_name);
    return false;
  }
  X509_set_subject_name(cert_, subject_name);
  X509_set_issuer_name(cert_, issuer_name);

  time_point tp_nb;
  time_point tp_na;
  tp_nb.decode_time(not_before_);
  tp_na.decode_time(not_after_);
  time_t tm_nb;
  time_t tm_na;
  if (!time_point_to_time_t(tp_nb, &tm_nb))
    return false;
  if (!time_point_to_time_t(tp_na, &tm_na))
    return false;
  ASN1_TIME* not_after_tm= ASN1_TIME_new();
  ASN1_TIME* not_before_tm= ASN1_TIME_new();
  ASN1_TIME_set(not_after_tm, tm_na);
  ASN1_TIME_set(not_before_tm, tm_nb);
  X509_set1_notBefore(cert_, (const ASN1_TIME*)not_before_tm);
  X509_set1_notAfter(cert_, (const ASN1_TIME*)not_after_tm);

  if (subject_key_ == nullptr)
    return false;
  EVP_PKEY* k =  EVP_PKEY_new();
  if (k == nullptr)
    return false;
  if (0 == EVP_PKEY_set1_RSA(k, subject_key_)) {
    EVP_PKEY_free(k);
    return false;
  }
  X509_set_pubkey(cert_, k);
  EVP_PKEY_free(k);

  return true;
}

bool my_x509::recover_cert_values() {

  if (cert_ == nullptr)
    return false;

  // void ASN1_INTEGER_free(ASN1_INTEGER *a);
  // ASN1_INTEGER_get_uint64
  // X509_get_serialNumber
  // int X509_NAME_add_entry_by_txt(X509_NAME *name, const char *field, int type, const unsigned char *bytes, int len, int loc, int set);
  // ASN1_INTEGER *X509_get_serialNumber(X509 *x);
  // const ASN1_INTEGER *X509_get0_serialNumber(const X509 *x);
  // int ASN1_INTEGER_get_uint64(uint64_t *pr, const ASN1_INTEGER *a);
  // X509_NAME *X509_get_subject_name(const X509 *x);
  // int X509_set_subject_name(X509 *x, X509_NAME *name);
  // const ASN1_TIME *X509_get0_notBefore(const X509 *x);
  // const ASN1_TIME *X509_get0_notAfter(const X509 *x);
  // EVP_PKEY *X509_get_pubkey(X509 *x);
  // EVP_PKEY *X509_get0_pubkey(const X509 *x);
  // ASN1_TIME *tm;
  // ASN1_TIME *ASN1_TIME_set(ASN1_TIME *s, time_t t);
  // ASN1_TIME *ASN1_TIME_adj(ASN1_TIME *s, time_t t, int offset_day, long offset_sec);
  return false;
}


