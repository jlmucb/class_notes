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

#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/hmac.h>


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
}

aes256_cbc::~aes256_cbc() {
  // clear key and iv
  if (ctx_ != nullptr) {
    EVP_CIPHER_CTX_free(ctx_);
    ctx_ = nullptr;
  }
}

bool aes256_cbc::encrypt(byte* key, byte* iv, int plain_len, byte* plain, int* cipher_size, byte* cipher) {
  total_cipher_len_ = 0;
  total_plain_len_ = 0;

  if (ctx_ == nullptr)
    ctx_ = EVP_CIPHER_CTX_new();
  
  memcpy(key_, key, 32);
  memcpy(iv_, iv, 32);
  if (1 != EVP_EncryptInit_ex(ctx_, EVP_aes_256_cbc(), NULL, key_, iv_))
    return false;
  // if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
  // ciphertext_len = len;
  // if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
  // ciphertext_len += len;
  // if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
  // ciphertext_len += len;
  return true;
}

bool aes256_cbc::decrypt(byte* key, int cipher_len, byte* cipher, int* plain_len, byte* plain) {
   // if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
  // if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
  // plaintext_len = len;
   // if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
   // plaintext_len += len;
  return true;
}

bool aes256_cbc::get_key(byte* out) {
  memcpy(out, key_, 32);
  return false;
}


#if 0
// EM = 0x00 || 0x02 || PS || 0x00 || M to fill buffer
void RSA_free(RSA *rsa);
int RSA_private_encrypt(int flen, unsigned char *from, unsigned char *to, RSA *rsa, int padding);
int RSA_public_decrypt(int flen, unsigned char *from, unsigned char *to, RSA *rsa, int padding);
int RSA_size(const RSA *rsa);
RSA_public_encrypt(size_sig, sig, decrypted, key, RSA_NO_PADDING)
RSA_private_decrypt(size_buf, buf, out, key, RSA_NO_PADDING)
RSA_PKCS1_PADDING
RSA_bits(r);
BIGNUM* m = BN_new();
BN_set_word(bne, e)
RSA_generate_key_ex(r, num_bits, bne, NULL)
BN_bin2bn
RSA_set0_key(r, n, e, d)
BN_bin2bn
RSA_get0_key(r, &m, &e, &d);
size = BN_num_bytes(m)
BIGNUM* d = nullptr;
RSA_bits(r);
RSA_get0_key(r, &m, &e, &d);
HMAC(EVP_sha256(), hmackey, mac_size, out, cipher_size, out , &hmac_size)
EVP_PKEY_free(verify_pkey);
size = BN_num_bytes(m)
BN_bn2bin(BIGNUM, byte buf)
rsa_sha256_verify
X509_verify(&cert, verify_pkey)
#endif

