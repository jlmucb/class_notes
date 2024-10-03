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

bool test_equal() {
  int n = 6;
  byte a[n];
  byte b[n];

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
  return true;
}

bool test_mult() {
  printf("num_perm_test_ops: %d\n", num_perm_test_ops);
  return true;
}

TEST (perms, test_perms) {
  EXPECT_TRUE(test_equal());
  EXPECT_TRUE(test_mult());
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

  int result = RUN_ALL_TESTS();
  return result;
}

