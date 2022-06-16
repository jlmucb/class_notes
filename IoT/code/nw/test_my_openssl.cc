// Copyright 2021 John Manferdelli, All Rights Reserved.
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
// File: test_my_openssl.cc

#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <stdio.h>
#include "my_ssl_lib.h"
#include "ssl.pb.h"

DEFINE_bool(print_all, false, "Print intermediate test computations");

// sha256 tests
byte* sha256_test1_input = (byte*)"abc";
int sha256_test1_size= 3;
uint32_t sha256_test1_answer[8] = {
  0xBA7816BF, 0x8F01CFEA, 0x414140DE, 0x5DAE2223,
  0xB00361A3, 0x96177A9C, 0xB410FF61, 0xF20015AD
};

// sha384 tests
byte* sha384_test1_input = (byte*)"abc";
int sha384_test1_size= 3;
uint32_t sha384_test1_answer[12] = {
  0xcb00753f, 0x45a35e8b, 0xb5a03d69, 0x9ac65007, 0x272c32ab, 0x0eded163,
  0x1a8b605a, 0x43ff5bed, 0x8086072b, 0xa1e7cc23, 0x58baeca1, 0x34c825a7
};
bool test_digest() {
  sha256_digest hash1;
  unsigned int digest_size = 64;  // big enough for any hash
  byte digest[digest_size];
  byte ans[64];                   // big enough for any hash

  // sha256
  if (!hash1.init())
    return false;
  if (!hash1.update(sha256_test1_size, sha256_test1_input))
    return false;
  if (!hash1.finalize(digest, &digest_size))
    return false;

  for (int i = 0; i < 8; i++) {
    reverse_bytes(4, (byte*)&sha256_test1_answer[i], &ans[4 * i]);
  }

  if (FLAGS_print_all) {
    printf("sha-256 in (%2d): ", sha256_test1_size);
    print_bytes(sha256_test1_size, (byte*)sha256_test1_input);
    printf("sha-256 digest : ");
    print_bytes(32, digest);
    printf("sha-256 correct: ");
    print_bytes(32, ans);
    printf("\n");
  }

  if (memcmp(ans, digest, 32) != 0) {
    return false;
  }

  sha384_digest hash2;
  if (!hash2.init())
    return false;
  if (!hash2.update(sha384_test1_size, sha384_test1_input))
    return false;
  if (!hash2.finalize(digest, &digest_size))
    return false;

  for (int i = 0; i < 12; i++) {
    reverse_bytes(4, (byte*)&sha384_test1_answer[i], &ans[4 * i]);
  }

  if (FLAGS_print_all) {
    printf("sha-384 in (%2d): ", sha384_test1_size);
    print_bytes(sha384_test1_size, (byte*)sha384_test1_input);
    printf("sha-384 digest : ");
    print_bytes(48, digest);
    printf("sha-384 correct: ");
    print_bytes(48, ans);
    printf("\n");
  }

  if (memcmp(ans, digest, 48) != 0) {
    return false;
  }
  return true;
}
TEST (digest, test_digest) {
  EXPECT_TRUE(test_digest());
}

bool test_encrypt() {
  aes256_cbc enc;
  aes256_cbc dec;

  byte key[32];
  byte iv[16];

  int input_len = 64;
  byte input[input_len]; 
  memset(input, 0, input_len);

  int output_len = 128;
  byte output[output_len]; 
  memset(output, 0, output_len);

  int recovered_len = 128;
  byte recovered[recovered_len]; 
  memset(recovered, 0, recovered_len);

  for (int i = 0; i < 32; i++)
    key[i] = (i % 256);
  for (int i = 0; i < 16; i++)
    iv[i] = (i % 256);
  for (int i = 0; i < input_len; i++)
    input[i] = ((5 *i) % 256);

  if (FLAGS_print_all) {
    printf("aes256_cbc key: ");
    print_bytes(32, key);
    printf("aes256_cbc iv : ");
    print_bytes(16, iv);

    printf("plain     (%2d) : ", input_len);
    print_bytes(input_len, input);
  }

  if (!enc.encrypt(key, iv, input_len, input, &output_len, output))
    return false;

  if (FLAGS_print_all) {
    printf("cipher    (%2d) : ", output_len);
    print_bytes(output_len, output);
  }

  if (!dec.decrypt(key, output_len, output, &recovered_len, recovered))
    return false;

  if (FLAGS_print_all) {
    printf("recovered (%2d): ", recovered_len);
    print_bytes(recovered_len, recovered);
  }

  if (recovered_len != input_len)
    return false;
  return (memcmp(recovered, input, input_len) == 0);
}
TEST (encrypt, test_encrypt) {
  EXPECT_TRUE(test_encrypt());
}

bool test_authenticated_encrypt() {
  authenticated_aes256_cbc enc;
  authenticated_aes256_cbc dec;

  byte key[64];
  byte iv[16];

  int input_len = 64;
  byte input[input_len]; 
  memset(input, 0, input_len);

  int output_len = 256;
  byte output[output_len]; 
  memset(output, 0, output_len);

  int recovered_len = 128;
  byte recovered[recovered_len]; 
  memset(recovered, 0, recovered_len);

  for (int i = 0; i < 64; i++)
    key[i] = (i % 256);
  for (int i = 0; i < 16; i++)
    iv[i] = (i % 256);
  for (int i = 0; i < input_len; i++)
    input[i] = ((5 * i) % 256);

  if (FLAGS_print_all) {
    printf("aes256_cbc_hmac_sha256 key: ");
    print_bytes(32, key);
    printf("aes256_cbc_hmac_sha256 iv : ");
    print_bytes(16, iv);

    printf("plain     (%2d) : ", input_len);
    print_bytes(input_len, input);
  }

  if (!enc.authenticated_encrypt(key, iv, input_len, input, &output_len, output))
    return false;

  if (FLAGS_print_all) {
    printf("cipher    (%2d) : ", output_len);
    print_bytes(output_len, output);
  }

  if (!dec.authenticated_decrypt(key, output_len, output, &recovered_len, recovered))
    return false;

  return true;
}
TEST (authenticated_encrypt, test_authenticated_encrypt) {
  EXPECT_TRUE(test_authenticated_encrypt());
}

bool test_rsa() {
  rsa_implement r;

  if (!r.generate_key(2048))
    return false;
  if (FLAGS_print_all) {
    printf("key:\n");
    RSA_print_fp(stdout, r.rsa_key_, 2);
  }

  int input_len = 64;
  byte input[input_len]; 
  memset(input, 0, input_len);

  int output_len = 256;
  byte output[output_len]; 
  memset(output, 0, output_len);

  int recovered_len = 512;
  byte recovered[recovered_len]; 
  memset(recovered, 0, recovered_len);

  for (int i = 0; i < input_len; i++)
    input[i] = ((5 * i + 1) % 256);

  if (FLAGS_print_all) {
    printf("plain  (%2d):\n", input_len);
    print_bytes(input_len, input);
  }

  if (!r.encrypt(RSA_PKCS1_PADDING, input_len, input,
        &output_len, output))
    return false;

  if (FLAGS_print_all) {
    printf("cipher (%2d)\n", output_len);
    print_bytes(output_len, output);
  }

  if (!r.decrypt(RSA_PKCS1_PADDING, output_len, output,
        &recovered_len, recovered))
    return false;

  if (FLAGS_print_all) {
    printf("recovered (%2d)\n", recovered_len);
    print_bytes(recovered_len, recovered);
  }

  if (input_len != recovered_len)
    return false;
  return (memcmp(input, recovered, recovered_len) == 0);
}
TEST (rsa, test_rsa) {
  EXPECT_TRUE(test_rsa());
}

bool test_x509() {
  my_x509 x;

  if (!x.init())
    return false;

  uint64_t sn = 1ULL;
  string nb("13 August 2021, 21:50:47.000000Z");
  string na("13 August 2023, 21:50:47.000000Z");
  string subject_name("John");
  string issuer_name("JohnRoot");

  if (!x.generate_keys_for_test(2048))
    return false;
  if (!x.set_sn(sn))
    return false;
  if (!x.set_not_before(nb))
    return false;
  if (!x.set_not_after(na))
    return false;
  if (!x.set_subject_name(subject_name))
    return false;
  if (!x.set_issuer_name(issuer_name))
    return false;

  EVP_PKEY* k =  EVP_PKEY_new();
  if (k == nullptr)
    return false;
  if (0 == EVP_PKEY_set1_RSA(k, x.subject_key_)) {
    EVP_PKEY_free(k);
    return false;
  }
  if (0 == X509_verify(x.cert_, k)) {
    EVP_PKEY_free(k);
    return false;
  }
  X509_set_pubkey(x.cert_, k);
  EVP_PKEY_free(k);

  if (!x.load_cert_values())
    return false;

  if (!x.sign_cert())
    return false;

  if (FLAGS_print_all) {
    X509_print_fp(stdout, x.cert_);
  }

  if (!x.verify_cert())
    return false;

  if (FLAGS_print_all) {
    string asn1;
    x509_to_asn1(x.cert_, &asn1);

    printf("\nder:\n");
    print_bytes(asn1.size(), (byte*) asn1.data());
    printf("\n");

    X509* y = X509_new();
    if (!asn1_to_x509(asn1, y))
      return false;
    printf("\nnew x509:\n");
    X509_print_fp(stdout, y);
    X509_free(y);
  }

  return true;
}
TEST (x509, test_x509) {
  EXPECT_TRUE(test_x509());
}

bool test_sig_special() {

  rsa_implement rsa_obj;

  if (!rsa_obj.generate_key(2048))
    return false;
  if (FLAGS_print_all) {
    printf("key:\n");
    RSA_print_fp(stdout, rsa_obj.rsa_key_, 2);
  }

  int to_sign_size = 64;
  byte to_sign[to_sign_size];
  memset(to_sign, 0, to_sign_size);
  for (int i = 0; i < to_sign_size; i++)
    to_sign[i] = (byte)i;

  int sig_size = 256;
  byte sig[sig_size];
  memset(sig, 0, sig_size);
  byte decrypted[sig_size];
  memset(decrypted, 0, sig_size);

  int hash_size = 64;
  byte hash[hash_size];
  memset(hash, 0, hash_size);

  sha256_digest dg;
  dg.init();
  dg.update(to_sign_size, to_sign);
  dg.finalize(hash, (unsigned int*)&hash_size);
  printf("To hash: ");
  print_bytes(to_sign_size, to_sign);
  printf("Hashed : ");
  print_bytes(hash_size, hash);

  if (!rsa_sign_msg(rsa_obj.rsa_key_, to_sign, to_sign_size,
         sig, &sig_size)) {
    printf("Can't sign msg\n");
    return false;
  }

  printf("Sig    :\n");
  print_bytes(sig_size, sig);

  int n = RSA_public_encrypt(sig_size, sig, decrypted, rsa_obj.rsa_key_, RSA_NO_PADDING);
  printf("Decrypted:\n");
  print_bytes(sig_size, decrypted);

  if (!rsa_verify_sig(rsa_obj.rsa_key_, hash, 32, sig, sig_size)) {
    printf("Can't verify msg\n");
    return false;
  }

  return true;
}
TEST (test_sig_special, test_sig_special) {
  EXPECT_TRUE(test_sig_special());
}

bool test_ecc() {
  ecc_implement ec;

  if (!ec.generate_key(384))
    return false;
  ec.print_key();

  sha384_digest sha384_dgst;
  unsigned int dgst_len = 64;
  byte dgst[dgst_len];
  unsigned int sig_len = 64;
  byte sig[sig_len];
  const char* to_hash = "abc";

  if (!sha384_dgst.init())
    return false;
  if (!sha384_dgst.update(strlen(to_hash), (byte*)to_hash))
    return false;
  if (!sha384_dgst.finalize(dgst, &dgst_len))
    return false;

  if (!ec.sign(dgst_len, dgst, &sig_len, sig))
    return false;
  if (!ec.verify(dgst_len, dgst, (int)sig_len, sig))
    return false;
  return true;
}
TEST (test_ecc, test_ecc) {
  EXPECT_TRUE(test_ecc());
}

int main(int an, char** av) {
  gflags::ParseCommandLineFlags(&an, &av, true);
  an = 1;
  ::testing::InitGoogleTest(&an, av);

  // open /dev/random?

  int result = RUN_ALL_TESTS();

  return result;
}
