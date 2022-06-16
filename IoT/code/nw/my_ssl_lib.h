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
// File: my_ssl_lib.h

#ifndef _MY_OPENSSL_LIB___
#define _MY_OPENSSL_LIB___

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cmath>

#include <stdio.h>
#include <string.h>
#include <string>

#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/hmac.h>
#include <openssl/ecdsa.h>

#ifndef byte
typedef unsigned char byte;
#endif

using std::string;

const int seconds_in_day = 60 * 60 * 24;

class time_point {
 public:
  int year_;
  int month_;  // 1= January
  int day_in_month_;
  int hour_;
  int minutes_;
  double seconds_;

  time_point();
  bool time_now();
  bool add_interval_to_time(time_point& from, double seconds_later);
  void print_time();
  bool encode_time(string* the_time);
  bool decode_time(string& encoded_time);
  bool time_point_to_unix_tm(struct tm* time_now);
  bool unix_tm_to_time_point(struct tm* time_now);
};
int compare_time_points(time_point& l, time_point& r);
bool time_point_to_time_t(time_point& tp, time_t* tm);
bool time_t_to_time_point(time_t& tm, time_point* tp);

void print_bytes(int n, byte* in);
void reverse_bytes(int size, byte* in, byte* out);

class sha256_digest {
public:
  EVP_MD_CTX* ctx_;

  sha256_digest();
  ~sha256_digest();

  bool init();
  bool update(int size, byte* buf);
  bool finalize(byte* digest, unsigned int* size);
  int get_digest_size() {return 32;};
};


class sha384_digest {
public:
  EVP_MD_CTX* ctx_;

  sha384_digest();
  ~sha384_digest();

  bool init();
  bool update(int size, byte* buf);
  bool finalize(byte* digest, unsigned int* size);
  int get_digest_size() {return 48;};
};

class aes256_cbc {
public:
  EVP_CIPHER_CTX* ctx_;
  int total_cipher_len_;
  int total_plain_len_;
  byte key_[32];

  aes256_cbc();
  ~aes256_cbc();

  bool encrypt(byte* key, byte* iv, int plain_len, byte* plain,
        int* cipher_size, byte* cipher);
  bool decrypt(byte* key, int cipher_len, byte* cipher,
        int* plain_len, byte* plain);
  int get_key_size() {return 32;};
  int get_block_size() {return 16;};
  bool get_key(byte* out);
};

class authenticated_aes256_cbc {
public:
  EVP_CIPHER_CTX* ctx_;
  int total_cipher_len_;
  int total_plain_len_;
  byte key_[64];

  authenticated_aes256_cbc();
  ~authenticated_aes256_cbc();

  bool authenticated_encrypt(byte* key, byte* iv,
        int plain_len, byte* plain, int* cipher_size, byte* cipher);
  bool authenticated_decrypt(byte* key,
        int cipher_len, byte* cipher, int* plain_len, byte* plain);
  int get_key_size() {return 64;};
  int get_block_size() {return 16;};
  bool get_key(byte* out);
};

class rsa_implement {
public:
  bool key_initialized_;
  int bit_size_ = 0;
  RSA* rsa_key_;
  string m_;
  string e_;
  string d_;

  rsa_implement();
  ~rsa_implement();

  bool generate_key(int num_bits);
  bool set_key_from_parameters(int num_bits);
  bool get_key_from_parameters();

  bool encrypt(int padding, int plain_len, byte* plain,
        int* cipher_size, byte* cipher);
  bool decrypt(int padding, int cipher_len, byte* cipher,
        int* plain_len, byte* plain);
  int get_key_size() {return (bit_size_ + 7) / 8;};
  int get_block_size() {return (bit_size_ + 7) / 8;};
  bool get_m(byte* out);
  bool get_e(byte* out);
  bool get_d(byte* out);
};

class ecc_implement {
public:
  bool key_initialized_;
  int bit_size_ = 0;
  EC_KEY* ecc_key_;
  string m_;
  string e_;
  string d_;

  ecc_implement();
  ~ecc_implement();

  bool generate_key(int num_bits);
  bool set_key_from_parameters(int num_bits);
  bool get_key_from_parameters();

  bool encrypt(int padding, int plain_len, byte* plain,
        int* cipher_size, byte* cipher);
  bool decrypt(int padding, int cipher_len, byte* cipher,
        int* plain_len, byte* plain);
  int get_key_size() {return (bit_size_ + 7) / 8;};
  int get_block_size() {return (bit_size_ + 7) / 8;};
  bool get_m(byte* out);
  bool get_e(byte* out);
  bool get_d(byte* out);
  void print_key();
};

class my_x509 {
public:
  uint64_t  sn_;
  string not_before_;
  string not_after_;
  string subject_name_;
  string issuer_name_;

  RSA* issuer_key_;
  string issuer_m_;
  string issuer_e_;
  string issuer_d_;
  RSA* subject_key_;
  string subject_m_;
  string subject_e_;
  string subject_d_;

  X509* cert_;

  my_x509();
  ~my_x509();

  bool generate_keys_for_test(int num_bits);

  bool init();
  bool get_m(string* m_str);
  bool set_m(string& m_str);
  bool get_e(string* e_str );
  bool set_e(string& e_str );
  bool get_d(string* d_str);
  bool set_d(string& d_str);
  bool get_sn(uint64_t* sn);
  bool set_sn(uint64_t sn);
  bool get_not_before(string* nb);
  bool set_not_before(string& nb);
  bool get_not_after(string* na);
  bool set_not_after(string& na);
  bool get_subject_name(string* subject_name);
  bool set_subject_name(string& subject_name);
  bool get_issuer_name(string* issuer_name);
  bool set_issuer_name(string& issuer_name);

  bool get_subject_parameters_from_key();
  bool set_subject_key_from_parameters();
  bool get_issuer_parameters_from_key();
  bool set_issuer_key_from_parameters();

  bool load_cert_values();
  bool recover_cert_values();

  bool sign_cert();
  bool verify_cert();
};


bool rsa_sign_msg(RSA* r, const byte* to_sign, int to_sign_size,
         byte* sig, int* sig_size);
bool rsa_verify_sig(RSA* r, byte* hash, int hash_size,
         const byte* sig, int sig_size);

bool asn1_to_x509(string& in, X509 *x);
bool x509_to_asn1(X509 *x, string* out);

void bn_print(const BIGNUM* n);
#endif
