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
  string p1("john");
  string p2("paul");
  string alg("none");
  if (!add_principal_to_proto_list(p1, alg, 0, nullptr, pl)) {
    return false;
  }
  if (!add_principal_to_proto_list(p2, alg, 0, nullptr, pl)) {
    return false;
  }
  return true;
}

bool construct_sample_resources(resource_list* rl) {
  string p1("john");
  string p2("paul");
  string r1("file_1");
  string r2("file_2");
  string l1("/datica/file_1");
  string l2("/datica/file_2");
  string t;
  time_point tp;
  if (!tp.time_now())
    return false;
  if (!tp.encode_time(&t))
    return false;
  if (!add_resource_to_proto_list(r1, l1, t, t, rl)) {
    return false;
  }
  if (!add_resource_to_proto_list(r2, l1, t, t, rl)) {
    return false;
  }
  if (!add_reader_to_resource(p1, rl->mutable_resources(0)))
    return false;
  if (!add_reader_to_resource(p2, rl->mutable_resources(1)))
    return false;
  return true;
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
