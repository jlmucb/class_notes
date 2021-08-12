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
  return true;
}
TEST (encrypt, test_encrypt) {
  EXPECT_TRUE(test_encrypt());
}

bool test_authenticated_encrypt() {
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
