// Copyright 2014-2025 John Manferdelli, All Rights Reserved.
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
// File: test_crypto_support.cc

#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <stdio.h>
#include "acl.pb.h"
#include "acl.h"

DEFINE_bool(print_all, false, "Print intermediate test computations");


bool construct_sample_principals(principal_list* pl) {
  return false;
}

bool construct_sample_resources(resource_list* rl) {
  return false;
}

bool test_basic() {
  principal_list pl;
  resource_list rl;

  if (!construct_sample_principals(&pl)) {
    printf("Cant construct principals\n");
    return false;
  }
  if (!construct_sample_resources(&rl)) {
    printf("Cant construct resources\n");
    return false;
  }
  print_principal_list(pl);
  print_resource_list(rl);
  return true;
}

bool test_access() {
  principal_list pl;
  resource_list rl;

  if (!construct_sample_principals(&pl)) {
    printf("Cant construct principals\n");
    return false;
  }
  if (!construct_sample_resources(&rl)) {
    printf("Cant construct resources\n");
    return false;
  }
  return true;
}

TEST (basic, test_basic) {
  EXPECT_TRUE(test_basic());
  EXPECT_TRUE(test_access());
}

int main(int an, char** av) {
  gflags::ParseCommandLineFlags(&an, &av, true);
  an = 1;
  ::testing::InitGoogleTest(&an, av);

  printf("Starting\n");

  int result = RUN_ALL_TESTS();

  return result;
}
