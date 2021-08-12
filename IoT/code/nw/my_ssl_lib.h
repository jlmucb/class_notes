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


#ifndef byte
typedef unsigned char byte;
#endif

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

#endif
