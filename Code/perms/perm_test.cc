//
// Copyright 2024 John Manferdelli, All Rights Reserved.
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

#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>

#include <gtest/gtest.h>
#include <gflags/gflags.h>
using std::string;

#include <perms.h>

std::string perm_test_ops[] = {
    "--operation=apply_perm",
    "--operation=mult_and_membership",
};

int num_perm_test_ops = sizeof(perm_test_ops) / sizeof(string);;

void print_options() {
  printf("Permitted operations:\n");
  for (int i = 0; i < num_perm_test_ops; i++) {
    printf("  perm_test.exe %s\n", perm_test_ops[i].c_str());
  }
  return;
}

DEFINE_bool(print_all, false, "Print intermediate test computations");

#include "grp_data.inc"

bool test_calc_p2() {
  printf("test_calc_p2\n"); 

  for (int i = 0; i < 8; i++) {
    if(!parse_perm_string(eight_str[i], 6, &eight[6 * i])) {
      printf("Couldn't parse eight string %d\n", i);
      return false;
    }
  }

  printf("\neight\n");
  for (int i = 0; i < 8; i++) {
    printf("    ");
    print_perm_cycles(6, &eight[6 * i]);
    printf("\n");
  }
  printf("\n");
  printf("\n");

  for (int i = 0; i < 3; i++) {
    if(!parse_perm_string(three_str[i], 6, &three[6 * i])) {
      printf("Couldn't parse three string %d\n", i);
      return false;
    }
  }

  printf("\nthree\n");
  for (int i = 0; i < 3; i++) {
    printf("    ");
    print_perm_cycles(6, &three[6 * i]);
    printf("\n");
  }
  printf("\n");
  printf("\n");

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 8; j++) {
      if (!multiply_perms(6, &three[6 * i], &eight[6 * j] ,&twenty_four[6 * (8 * i + j)]))
        return false;
    }
  }

  printf("\ntwenty four\n");
  for (int i = 0; i < 24; i++) {
    printf("    ");
    print_perm_cycles(6, &twenty_four[6 * i]);
    printf("\n");
  }
  printf("\n");
  printf("\n");

  byte t[6];
  t[4] = 5; t[5] = 6;
  t[1] =3; t[2] = 2;
  t[0] = 4; t[3] = 1;

  for (int i = 0; i < 144; i++) {
    fourty_eight[i]= twenty_four[i]; 
  }
  for (int i = 0; i < 24; i++) {
      if (!multiply_perms(6, t, &twenty_four[6 * i], &fourty_eight[6 * (i + 24)]))
        return false;
  }

  printf("\nfourty eight\n");
  for (int i = 0; i < 48; i++) {
    printf("    ");
    print_perm_cycles(6, &fourty_eight[6 * i]);
    printf("\n");
  }
  printf("\n");
  printf("\n");
  printf("Done\n");

  return true;
}


bool test_p1() {
  printf("test_p1\n"); 

  for (int i = 0; i < 48; i++) {
    if(!parse_perm_string(P1_str[i], 6, &P1_perms[6 * i])) {
      printf("Couldn't parse P1 string %d\n", i);
      return false;
    }
  }

  printf("\nP1\n");
  for (int i = 0; i < 48; i++) {
    printf("    ");
    print_perm_cycles(6, &P1_perms[6 * i]);
    printf("\n");
  }
  printf("\n");
  printf("\n");

  return true;
}

bool test_p2() {
  printf("test_p2\n"); 

  for (int i = 0; i < 48; i++) {
    if(!parse_perm_string(P2_str[i], 6, &P2_perms[6 * i])) {
      printf("Couldn't parse P2 string %d\n", i);
      return false;
    }
  }

  printf("\nP2\n");
  for (int i = 0; i < 48; i++) {
    printf("    ");
    print_perm_cycles(6, &P2_perms[6 * i]);
    printf("\n");
  }
  printf("\n");
  printf("\n");

  return true;
}

bool test_equal() {
  int n = 6;
  byte a[n];
  byte b[n];

  printf("test_equal\n"); 
  for(int i = 0; i < n; i++) {
    a[i]= i + 1;
    b[i]= i + 1;
  }
  b[0]= 2;
  b[1]= 1;

  if (!perms_equal(n, a, a))
    return false;
  if (perms_equal(n, a, b))
    return false;

  if(FLAGS_print_all) {
    print_perm(n, a);
    printf(" ");
    print_perm(n, b);
    printf("\n");

    print_perm_cycles(n, a);
    printf("\n");
    print_perm_cycles(n, b);
    printf("\n");
    byte c[n];
    c[0]= 2; c[1]=3; c[2] = 1; c[3] = 5; c[4]= 6; c[5]= 4;
    print_perm_cycles(n, c);
    printf("\n");
  }
  return true;
}

bool test_mult() {
  int n = 6;
  byte a[n];
  byte b[n];
  byte c[n];
  a[0]= 2; a[1]=3; a[2] = 1; a[3] = 5; a[4]= 6; a[5]= 4;

  printf("test_mult\n");
  if (!multiply_perms(n, a, a ,c))
    return false;

  if(FLAGS_print_all) {
    print_perm_cycles(n, a);
    printf(" ");
    print_perm_cycles(n, a);
    printf(" = ");
    print_perm_cycles(n, c);
    printf("\n");
  }

  b[0]= 2; b[1]= 1; b[2] = 3; b[3] = 4; b[4]= 5; b[5]= 6;
  if (!multiply_perms(n, a, b ,c))
    return false;
  if(FLAGS_print_all) {
    print_perm_cycles(n, a);
    printf(" ");
    print_perm_cycles(n, b);
    printf(" = ");
    print_perm_cycles(n, c);
    printf("\n");

    print_perm_cycles(n, a);
    printf("  and ");
    print_perm_cycles(n, a);
  }
  if (commutes(n, a, a)) {
    if(FLAGS_print_all) {
        printf("  commute\n");
    }
  } else {
    if(FLAGS_print_all) {
      printf("  do not comute\n");
    }
    return false;
  }

  if(FLAGS_print_all) {
    print_perm_cycles(n, a);
    printf("  and ");
    print_perm_cycles(n, b);
  }
  if (!commutes(n, a, b)) {
    if(FLAGS_print_all) {
      printf("  do not commute\n");
    }
  } else {
    if(FLAGS_print_all) {
      printf("  commute\n");
    }
      return false;
  }
   
  byte d[n];
  if (!inverse_perms(n, a, d))
    return false;
  if(FLAGS_print_all) {
    printf("inverse of ");
    print_perm_cycles(n, a);
    printf(" = ");
    print_perm_cycles(n, d);
    printf("\n");
  }

  byte e[n];
  if (!multiply_perms(n, a, d ,e))
    return false;
  if (!is_identity(n, e))
    return false;

  return true;
}

bool test_parse() {
  int n = 6;
  byte a[n];

  printf("test_parse\n");
  const char* s = "123456";
  if (!parse_perm_string(s, n, a))
    return false;

  if(FLAGS_print_all) {
    printf("%s -->", s);
    print_perm_cycles(n, a);
    printf("\n");
  }

  s = "234561";
  if (!parse_perm_string(s, n, a))
    return false;

  if(FLAGS_print_all) {
    printf("%s -->", s);
    print_perm_cycles(n, a);
    printf("\n");
  }
  return true;
}

TEST (perms, test_perms) {
  EXPECT_TRUE(test_parse());
  EXPECT_TRUE(test_equal());
  EXPECT_TRUE(test_mult());
  EXPECT_TRUE(test_p1());
  EXPECT_TRUE(test_p2());
  EXPECT_TRUE(test_calc_p2());
}

int main(int an, char** av) {
#ifdef __linux__
  gflags::ParseCommandLineFlags(&an, &av, true);
#else
  google::ParseCommandLineFlags(&an, &av, true);
#endif

 gflags::ParseCommandLineFlags(&an, &av, true);
  an = 1;
  ::testing::InitGoogleTest(&an, av);

  if(!parse_perm_string(a_involution_str, 6, a_involution)) {
    printf("Couldn't parse a_involution\n");
    return 1;
  }
  if(!parse_perm_string(b_involution_str, 6, b_involution)) {
    printf("Couldn't parse b_involution\n");
    return 1;
  }

  printf("\na_involution: ");
  print_perm_cycles(6, a_involution);
  printf("\n");
  printf("b_involution: ");
  print_perm_cycles(6, b_involution);
  printf("\n");
  printf("\n");

  int result = RUN_ALL_TESTS();
  return result;
}

