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
  int cipher_len;
  int plain_len;
  byte iv_[16];
  byte key_[32];

  aes256_cbc();
  ~aes256_cbc();

  bool encrypt_init(int key_size, byte* key, byte* iv);
  bool decrypt_init(int key_size, byte* key, byte* iv);
  bool encrypt_update(byte* cipher, int* cipher_len,
        byte* plaintext, int plaintext_len);
  bool encrypt_finalize(byte* cipher, int* size);
  bool decrypt_update(byte* cipher, int* cipher_len,
        byte* plaintext, int plaintext_len);
  bool decrypt_finalize(byte* plain, int* size);
  int get_key_size() {return 32;};
  int get_block_size() {return 16;};
  bool get_key(byte* out);
};


#endif
