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
bool test_digest() {
  sha256_digest hash;
  unsigned int digest_size = 32;
  byte digest[digest_size];

  if (!hash.init())
    return false;
  if (!hash.update(sha256_test1_size, sha256_test1_input))
    return false;
  if (!hash.finalize(digest, &digest_size))
    return false;

  byte ans[32];
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

  return (memcmp(ans, digest, 32) == 0);
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
printf("back\n");

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
  // authenticated_aes256_cbc enc;
  // authenticated_aes256_cbc dec;

  return true;
}
TEST (authenticated_encrypt, test_authenticated_encrypt) {
  EXPECT_TRUE(test_authenticated_encrypt());
}

bool test_rsa() {
  return true;
}
TEST (rsa, test_rsa) {
  EXPECT_TRUE(test_rsa());
}


int main(int an, char** av) {
  gflags::ParseCommandLineFlags(&an, &av, true);
  an = 1;
  ::testing::InitGoogleTest(&an, av);

  // init?
  // open /dev/random

  int result = RUN_ALL_TESTS();

  return result;
}
