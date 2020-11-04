//
// Copyright 2014-2019 John Manferdelli, All Rights Reserved.
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
// File: symmetric.cc

#include <stdio.h>
#include <string.h>
#include <string>
#include "util.h"
#include "types.h"
#include "conversions.h"
#include "keys.h"
#include "aes.h"
#include "twofish.h"
#include "rc4.h"
#include "tea.h"
#include "simonspeck.h"

#include <memory>
#include <cmath>


uint64_t cycles_per_second = 10;

byte aes128_test1_plain[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                             0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
byte aes128_test1_key[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
byte aes128_test1_cipher[] = {0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
                              0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};

bool SimpleAes128Test() {
  Aes aes128_object;
  byte test_cipher_out[16];
  byte test_plain_out[16];

    printf("Aes-128 test\n");
    printf("\n");

  if (!aes128_object.Init(128, aes128_test1_key, Aes::BOTH)) {
    printf("Can't init aes object\n");
    return false;
  }
  aes128_object.EncryptBlock(aes128_test1_plain, test_cipher_out);
  aes128_object.DecryptBlock(test_cipher_out, test_plain_out);
    printf("\tKey            : ");
    PrintBytes(16, aes128_test1_key);
    printf("\n");
    printf("\tCorrect plain  : ");
    PrintBytes(16, aes128_test1_plain);
    printf("\n");
    printf("\tCorrect cipher : ");
    PrintBytes(16, aes128_test1_cipher);
    printf("\n");
    printf("\tComputed cipher: ");
    PrintBytes(16, test_cipher_out);
    printf("\n");
    printf("\tComputed plain : ");
    PrintBytes(16, test_plain_out);
    printf("\n");
    printf("\n");
  if (memcmp(aes128_test1_cipher, test_cipher_out, 16) != 0) return false;
  if (memcmp(aes128_test1_plain, test_plain_out, 16) != 0) return false;
  return true;
}

// AES-256 (Nk=8, Nr=14)
byte aes256_test1_plain[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                              0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
byte aes256_test1_key[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
byte aes256_test1_cipher[] = { 0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
                               0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89 };

bool SimpleAes256Test() {
  Aes aes256_object;
  byte test_cipher_out[16];
  byte test_plain_out[16];

    printf("Aes-256 test\n");
    printf("\n");

  if (!aes256_object.Init(256, aes256_test1_key, Aes::BOTH)) {
    printf("Can't init aes object\n");
    return false;
  }
  aes256_object.EncryptBlock(aes256_test1_plain, test_cipher_out);
  aes256_object.DecryptBlock(test_cipher_out, test_plain_out);
    printf("\tKey            : ");
    PrintBytes(32, aes256_test1_key);
    printf("\n");
    printf("\tCorrect plain  : ");
    PrintBytes(16, aes256_test1_plain);
    printf("\n");
    printf("\tCorrect cipher : ");
    PrintBytes(16, aes256_test1_cipher);
    printf("\n");
    printf("\tComputed cipher: ");
    PrintBytes(16, test_cipher_out);
    printf("\n");
    printf("\tComputed plain : ");
    PrintBytes(16, test_plain_out);
    printf("\n");
    printf("\n");
  if (memcmp(aes256_test1_cipher, test_cipher_out, 16) != 0) return false;
  if (memcmp(aes256_test1_plain, test_plain_out, 16) != 0) return false;
  return true;
}

static byte twofish_k128[] = {
    0x9F, 0x58, 0x9F, 0x5C, 0xF6, 0x12, 0x2C, 0x32,
    0xB6, 0xBF, 0xEC, 0x2F, 0x2A, 0xE8, 0xC3, 0x5A,
};
static byte twofish_p128[] = {0xD4, 0x91, 0xDB, 0x16, 0xE7, 0xB1, 0xC3, 0x9E,
                              0x86, 0xCB, 0x08, 0x6B, 0x78, 0x9F, 0x54, 0x19};
static byte twofish_c128[] = {0x01, 0x9F, 0x98, 0x09, 0xDE, 0x17, 0x11, 0x85,
                              0x8F, 0xAA, 0xC3, 0xA3, 0xBA, 0x20, 0xFB, 0xC3};

bool SimpleTwofishTest() {
  Twofish enc_obj;

    printf("Twofish test\n");
    printf("\tKey: ");
    PrintBytes(16, twofish_k128);
    printf("\n");
  if (!enc_obj.Init(128, twofish_k128, 0)) {
    printf("failed initialization\n");
    return false;
  }

  byte encrypted[16];
  byte decrypted[16];

  memset(encrypted, 0, 16);
  enc_obj.Encrypt(16, twofish_p128, encrypted);
    printf("\tplaintext: ");
    PrintBytes(16, twofish_p128);
    printf("\n");
    printf("\tencrypted: ");
    PrintBytes(16, encrypted);
    printf("\n");
    printf("\texpected : ");
    PrintBytes(16, twofish_c128);
    printf("\n");
    printf("\n");

  memset(decrypted, 0, 16);
  enc_obj.Decrypt(16, encrypted, decrypted);
    printf("\tdecrypted: ");
    PrintBytes(16, decrypted);
    printf("\n");
    printf("\n");
  if (memcmp(twofish_p128, decrypted, 16) != 0 ||
      memcmp(twofish_c128, encrypted, 16) != 0) {
    printf("Twofish: decrypted in and encrypted/decrypted text dont match\n");
    return false;
  }
  return true;
}

bool Aes128Compare(int size_key, byte* key, int size_in, byte* in) {
  Aes aes128_object;
  byte test_cipher_out[16];
  byte test_plain_out[16];

  if (!aes128_object.Init(128, key, Aes::BOTH)) {
    printf("Aes128Compare: Can't init aes object\n");
    return false;
  }
  aes128_object.EncryptBlock(in, test_cipher_out);
  aes128_object.DecryptBlock(test_cipher_out, test_plain_out);
    printf("\tKey            : ");
    PrintBytes(size_key, key);
    printf("\n");
    printf("\tPlain          : ");
    PrintBytes(16, in);
    printf("\n");
    printf("\tComputed plain : ");
    PrintBytes(16, test_plain_out);
    printf("\n");
  if (memcmp(in, test_plain_out, 16) != 0) {
    printf("AesCompare: Aes-128 test failed\n");
    return false;
  }
  return true;
}

#if 0
bool aes128_benchmark_tests(byte* key, int num_tests, bool use_aesni) {
  printf("\nAES128_TIME_TESTS\n");
  byte in[64];
  byte out[64];
  int num_tests_executed = 0;
  use_aesni &= HaveAesNi();

  uint64_t cycles_start_test;
  if (use_aesni) {
    AesNi aes;
    if (!aes.Init(128, key, Aes::ENCRYPT)) {
      cycles_start_test = 0;
      printf("AesNi failed Init()\n");
    } else {
      cycles_start_test = ReadRdtsc();
      for (num_tests_executed = 0; num_tests_executed < num_tests;
           num_tests_executed++) {
        aes.EncryptBlock(in, out);
      }
    }
  } else {
    Aes aes;
    if (!aes.Init(128, key, Aes::ENCRYPT)) {
      cycles_start_test = 0;
      printf("Aes failed Init()\n");
    } else {
      cycles_start_test = ReadRdtsc();
      for (num_tests_executed = 0; num_tests_executed < num_tests;
           num_tests_executed++) {
        aes.EncryptBlock(in, out);
      }
    }
  }
  uint64_t cycles_end_test = ReadRdtsc();
  uint64_t cycles_diff = cycles_end_test - cycles_start_test;
  if (use_aesni) {
    printf("using aesni, ");
  } else {
    printf("not using aesni, ");
  }
  printf("aes_time_test number of successful tests: %d\n", num_tests_executed);
  printf("total ellapsed time %le\n",
         ((double)cycles_diff) / ((double)cycles_per_second));
  printf("time per block %le\n",
         ((double)cycles_diff) /
             ((double)(num_tests_executed * cycles_per_second)));
  printf("END AES128_TIME_TESTS\n\n");
  return true;
}

bool aes256_benchmark_tests(byte* key, int num_tests, bool use_aesni) {
  printf("\nAES256_TIME_TESTS\n");
  byte in[64];
  byte out[64];
  int num_tests_executed = 0;
  use_aesni &= HaveAesNi();

  uint64_t cycles_start_test;
  if (use_aesni) {
    AesNi aes;
    if (!aes.Init(256, key, Aes::ENCRYPT)) {
      cycles_start_test = 0;
      printf("AesNi failed Init()\n");
    } else {
      cycles_start_test = ReadRdtsc();
      for (num_tests_executed = 0; num_tests_executed < num_tests;
           num_tests_executed++) {
        aes.EncryptBlock(in, out);
      }
    }
  } else {
    Aes aes;
    if (!aes.Init(256, key, Aes::ENCRYPT)) {
      cycles_start_test = 0;
      printf("Aes failed Init()\n");
    } else {
      cycles_start_test = ReadRdtsc();
      for (num_tests_executed = 0; num_tests_executed < num_tests;
           num_tests_executed++) {
        aes.EncryptBlock(in, out);
      }
    }
  }
  uint64_t cycles_end_test = ReadRdtsc();
  uint64_t cycles_diff = cycles_end_test - cycles_start_test;
  if (use_aesni) {
    printf("using aesni, ");
  } else {
    printf("not using aesni, ");
  }
  printf("aes_time_test number of successful tests: %d\n", num_tests_executed);
  printf("total ellapsed time %le\n",
         ((double)cycles_diff) / ((double)cycles_per_second));
  printf("time per block %le\n",
         ((double)cycles_diff) /
             ((double)(num_tests_executed * cycles_per_second)));
  printf("END AES256_TIME_TESTS\n\n");
  return true;
}

bool sha1_benchmark_tests() {
  return true;
}

bool sha256_benchmark_tests() {
  /*
   */
  return true;
}
#endif

byte rc4_test_key[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
byte rc4_test_in[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte rc4_test_out[16] = {0xb2, 0x39, 0x63, 0x05, 0xf0, 0x3d, 0xc0, 0x27,
                         0xcc, 0xc3, 0x52, 0x4a, 0x0a, 0x11, 0x18, 0xa8};
bool rc4_test1() {
  byte out[16];
  Rc4 rc4;

  if(!rc4.Init(5, rc4_test_key))
    return false;
  rc4.Encrypt(16, rc4_test_in, out);
    printf("Rc4 test\n");
    printf("\tKey            : ");
    PrintBytes(5, rc4_test_key);
    printf("\n");
    printf("\tCorrect out : ");
    PrintBytes(16, rc4_test_out);
    printf("\n");
    printf("\tout         : ");
    PrintBytes(16, out);
    printf("\n");
    printf("\n");
  return (memcmp(out, rc4_test_out, 16) == 0);
}

byte tea_test_key[16] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};
byte tea_test_in[8] = {0, 0, 0, 0, 0, 0, 0, 0};
byte tea_test_out[8] = {0x0a, 0x3a, 0xea, 0x41, 0x40, 0xa9, 0xba, 0x94};

bool tea_test1() {
  byte in[16];
  byte out[16];
  Tea tea;

    printf("Tea test\n");
  if(!tea.Init(128, tea_test_key, 0))
    return false;
  tea.Encrypt(8, tea_test_in, out);
  tea.Decrypt(8, out, in);
    printf("\tKey            : ");
    PrintBytes(16, tea_test_key);
    printf("\n");
    printf("\tin             : ");
    PrintBytes(8, tea_test_in);
    printf("\n");
    printf("\tcorrect out    : ");
    PrintBytes(8, tea_test_out);
    printf("\n");
    printf("\tEncrypted      : ");
    PrintBytes(8, out);
    printf("\n");
    printf("\tDecrypted      : ");
    PrintBytes(8, in);
    printf("\n");
  return (memcmp(out, tea_test_out, 8) == 0) && (memcmp(in, tea_test_in, 8) == 0);
}

uint64_t t_k[2] = {
    0x0706050403020100, 0x0f0e0d0c0b0a0908,
};
uint64_t t_i[2] = {
    0x6373656420737265, 0x6c6c657661727420,
};
uint64_t t_o[2] = {
    0x49681b1e1e54fe3f, 0x65aa832af84e0bbc,
};
byte* simon_test_key = (byte*)t_k;
byte* simon_test_in = (byte*)t_i;
byte* simon_test_out = (byte*)t_o;

bool simon_test1()  {
  byte out[16];
  byte in[16];
  Simon128 simon;

  if(!simon.Init(128, simon_test_key, 0))
    return false;
  uint64_t* o1 = (uint64_t*)out;
  uint64_t* o2 = (uint64_t*)(out + 8);
  uint64_t* i1 = (uint64_t*)in;
  uint64_t* i2 = (uint64_t*)(in + 8);
  simon.Encrypt(16, simon_test_in, out);
  simon.Decrypt(16, out, in);
    printf("Simon128 test\n");
    printf("\tKey         : %016llx %016llx\n", t_k[0], t_k[1]);
    printf("\tCorrect in  : %016llx %016llx\n", t_i[0], t_i[1]);
    printf("\tCorrect out : %016llx %016llx\n", t_o[0], t_o[1]);
    printf("\tin          : %016llx %016llx\n", *i1, *i2);
    printf("\tout         : %016llx %016llx\n", *o1, *o2);
    printf("\n");
  return (memcmp(out, simon_test_out, 16) == 0) && (memcmp(in, simon_test_in, 16) == 0);
}

byte test_key[] = {
    0x01, 0x02, 0x03, 0x04, 0x51, 0x52, 0x53, 0x54,
    0x91, 0x92, 0x93, 0x94, 0xe1, 0xe2, 0xe3, 0xe4,
    0x01, 0x02, 0x03, 0x04, 0x51, 0x52, 0x53, 0x54,
    0x91, 0x92, 0x93, 0x94, 0xe1, 0xe2, 0xe3, 0xe4,
};

void ReverseInPlace(int size, byte* in) {
  std::shared_ptr<byte>t(new byte[16]);
  ReverseCpy(size, in, t.get());
  memcpy((void*)in, (void*)t.get(), size);
}

int main(int an, char** av) {

  if (!InitUtilities("symmetrictest.log")) {
    printf("InitUtilities() failed\n");
    return 1;
  }
  cycles_per_second = 0; //CalibrateRdtsc();
  printf("Cycles per second on this machine: %llu\n\n", cycles_per_second);

  int num_tests = 0;
  int num_failed = 0;

  num_tests++;
  if(!SimpleAes128Test()) {
    num_failed++;
    printf(" failed\n");
  }
  num_tests++;
  if(!SimpleAes256Test()) {
    num_failed++;
    printf("SimpleAes256Test failed\n");
  }
  num_tests++;
#if 0
  if(!aes128_benchmark_tests(test_key, 10000, true)) {
    num_failed++;
    printf("aes128_benchmark_tests failed\n");
  }
  num_tests++;
  if(!aes128_benchmark_tests(test_key, 10000, false)) {
    num_failed++;
    printf("aes128_benchmark_tests failed\n");
  }
  num_tests++;
  if(!aes256_benchmark_tests(test_key, 10000, true)) {
    num_failed++;
    printf("aes256_benchmark_tests failed\n");
  }
  num_tests++;
  if(!aes256_benchmark_tests(test_key, 10000, false)) {
    num_failed++;
    printf("aes256_benchmark_tests failed\n");
  }
  num_tests++;
#endif
  if(!SimpleTwofishTest()) {
    num_failed++;
    printf("SimpleTwofishTest failed\n");
  }
  num_tests++;
  if(!rc4_test1()) {
    num_failed++;
    printf("rc4 failed\n");
  }
  num_tests++;
  if(!tea_test1()) {
    num_failed++;
    printf("tea failed\n");
  }
  num_tests++;
  if(!simon_test1()) {
    num_failed++;
    printf("simon failed\n");
  }
  num_tests++;

  CloseUtilities();
  printf("%d tests, %d failed\n", num_tests, num_failed);
  return num_failed;
}
