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
// File: acl.h

#ifndef ACL_H__
#define ACL_H__

#include "acl.pb.h"

// these may be duplicated so add a namespace

#ifndef int32_t
typedef int int32_t;
#endif

#ifndef int64_t
#ifdef __linux__
typedef long int int64_t;
#else
typedef long long int int64_t;
#endif
#endif

#ifndef uint32_t
typedef unsigned uint32_t;
#endif

#ifndef uint64_t
#ifdef __linux__
typedef long unsigned uint64_t;
#else
typedef long long unsigned uint64_t;
#endif
#endif

#ifndef NBITSINBYTE
#define NBITSINBYTE 8
#endif
#ifndef NBITSINUINT64
#define NBITSINUINT64 64
#endif

#ifndef byte
typedef unsigned char byte;
#endif

using std::string;
using std::ofstream;

const int seconds_in_day = 86400;
const int seconds_in_minute = 60;
const int seconds_in_hour = 3600;
const double seconds_in_common_year = 365.0 * 86400;
const double seconds_in_leap_year = 366.0 * 86400;
const double seconds_in_gaussian_year = 365.2568983 * 86400;

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

class random_source {
public:
  bool initialized_;
  bool have_rd_rand_;
  int fd_;

  random_source();
  bool have_intel_rd_rand();
  bool start_random_source();
  int get_random_bytes(int n, byte* b);
  bool close_random_source();
};

void print_bytes(int n, byte* in);
void reverse_bytes(int size, byte* in, byte* out);
void reverse_bytes_in_place(int size, byte* b);
int bits_to_bytes(int n);
int bytes_to_bits(int n);
int bits_to_uint64(int n);
int uint64_to_bits(int n);
bool hex_to_bytes(string& h, string* b);
bool bytes_to_hex(string& b, string* h);
bool base64_to_bytes(string& b64, string* b);
bool bytes_to_base64(string& b, string* b64);

class file_util {
 public:
  bool initialized_;
  int fd_;
  bool write_;
  int bytes_in_file_;
  int bytes_read_;
  int bytes_written_;

  file_util();
  bool create(const char* filename);
  bool open(const char* filename);
  int bytes_in_file();
  int bytes_left_in_file();
  int bytes_written_to_file();
  void close();
  int read_a_block(int size, byte* buf);
  bool write_a_block(int size, byte* buf);
  int read_file(const char* filename, int size, byte* buf);
  bool write_file(const char* filename, int size, byte* buf);
};

class channel_guard {
public:
  channel_guard();
  ~channel_guard();

  string principal_name_;
  string authentication_algorithm_name_;
  byte* creds_;
  bool channel_principal_authenticated_;
  int  num_resources_;
  resource_message* resources_;

  void print();
  bool authenticate(string& name, principal_list& pl);
  bool load_resources(resource_list& rl);
  int can_read(string resource_name);
  int can_write(string resource_name);
  int can_delete(string resource_name);
  int can_create(string resource_name);
  int find_resource(string& name);
  bool access_check(string& resource_name, string& action);
  bool add_new_principal(principal_message& pm);
  bool add_access_rights(access_list& al);
};

key_message* make_symmetrickey(const char* alg, const char* name, int bit_size,
                               const char* purpose, const char* not_before,
                               const char* not_after, string& secret);

key_message* make_rsakey(const char* alg, const char* name, int bit_size,
    const char* purpose, const char* not_before, const char* not_after,
    string& mod, string& e, string& d, string& p, string& q, string& dp,
    string& dq, string& m_prime, string& p_prime, string& q_prime);

key_message* make_ecckey(const char* name, int bit_size, const char* purpose,
                         const char* not_before, const char* not_after,
                         string& curve_name, string& curve_p,
                         string& curve_a, string& curve_b,
                         string& curve_base_x, string& curve_base_y,
                         string& order_base_point, string& secret,
                         string& curve_public_point_x, string& curve_public_point_y);

scheme_message* make_scheme(const char* alg, const char* id_name,
      const char* mode, const char* pad, const char* purpose,
      const char* not_before, const char* not_after,
      const char* enc_alg, int size_enc_key, string& enc_key,
      const char* enc_key_name, const char* hmac_alg,
      int size_hmac_key, string& hmac_key);

certificate_body_message* make_certificate_body(string& version, string& subject_name_type,
      string& subject_name, key_message& subject_key, string& purpose,
      string& not_before, string& note_after, string& nonce, string& revocation_address,
      string& date_signed);

certificate_message* make_certificate(certificate_body_message& cbm,
      string& issuer_name_type, string& issuer_name, key_message& issuer_key,
      string& signing_algorithm, string& signature);

bool get_resources_from_file(string& file_name, resource_list* rl);
bool get_principals_from_file(string& file_name, principal_list* pl);
bool save_resources_to_file(resource_list& rl, string& file_name);
bool save_principals_to_file(principal_list& pl, string& file_name);

void print_binary_blob(binary_blob_message& m);
void print_encrypted_message(encrypted_message& m);
void print_signature_message(signature_message& m);
void print_rsa_public_parameters_message(const rsa_public_parameters_message& m);
void print_ecc_public_parameters_message(const ecc_public_parameters_message& m);
void print_rsa_private_parameters_message(const rsa_private_parameters_message& m);
void print_ecc_private_parameters_message(const ecc_private_parameters_message& m);
void print_hmac_parameters_message(const hmac_parameters_message& m);
void print_key_message(const key_message& m);
void print_scheme_message(const scheme_message& m);
void print_certificate_name_message(const certificate_name_message& m);
void print_certificate_algorithm_message(const certificate_algorithm_message& m);
void print_certificate_message(const certificate_message& m);
void print_certificate_body(const certificate_body_message& cbm);
void print_certificate(const certificate_message& cm);

int crypto_get_random_bytes(int num_bytes, byte* buf);
bool init_crypto();
void close_crypto();

void print_encryption_parameters(const scheme_message& sm);
void print_principal_info(const principal_message& pi);
void print_audit_info(const audit_info& inf);
void print_resource_message(const resource_message& rm);
void print_principal_message(const principal_message& pm);

void print_resource_list(const resource_list& rl);
void print_principal_list(const principal_list& pl);

bool get_resources_from_file(string& file_name, resource_list* rl);
bool get_principals_from_file(string& file_name, principal_list* pl);
bool save_resources_to_file(resource_list& rl, string& file_name);
bool save_principals_to_file(principal_list& pl, string& file_name);
int on_reader_list(const resource_message& r, string& name);
int on_writer_list(const resource_message& r, string& name);
int on_deleter_list(const resource_message& r, string& name);
int on_creator_list(const resource_message& r, string& name);
int on_principal_list(string& name, const principal_list& pl);
int on_resource_list(string& name, const resource_list& rl);
bool add_reader_to_resource(string& name, resource_message* r);
bool add_writer_to_resource(string& name, resource_message* r);
bool add_deleter_to_resource(string& name, resource_message* r);
bool add_creator_to_resource(string& name, resource_message* r);
bool add_principal_to_proto_list(string& name, string& alg, int num_bytes, byte* cred, principal_list* pl);
bool add_resource_to_proto_list(string& id, string& locat, string& t_created, string& t_written, resource_list* rl);

bool sign_nonce(string& nonce, key_message& k, string* signature);

#endif

