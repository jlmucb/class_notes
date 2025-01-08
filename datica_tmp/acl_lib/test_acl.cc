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
// File: test_acl.cc

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
  string cred;
  if (!add_principal_to_proto_list(p1, alg, cred, pl)) {
    return false;
  }
  if (!add_principal_to_proto_list(p2, alg, cred, pl)) {
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
  if (!add_reader_to_resource_proto_list(p1, rl->mutable_resources(0)))
    return false;
  if (!add_reader_to_resource_proto_list(p2, rl->mutable_resources(1)))
    return false;
  if (!add_writer_to_resource_proto_list(p1, rl->mutable_resources(0)))
    return false;
  if (!add_writer_to_resource_proto_list(p2, rl->mutable_resources(1)))
    return false;
  if (!add_creator_to_resource_proto_list(p1, rl->mutable_resources(0)))
    return false;
  if (!add_creator_to_resource_proto_list(p2, rl->mutable_resources(1)))
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

  string p1("john");
  string p2("paul");
  string p3("tho");
  string r1("file_1");
  string r2("file_2");
  string r3("file_3");

  if (on_reader_list(rl.resources(0), p1) < 0) {
    printf("%s should be reader\n", p1.c_str());
    return false;
  }
  if (on_reader_list(rl.resources(1), p1) >= 0) {
    printf("%s should not be reader\n", p1.c_str());
    return false;
  }
  if (on_reader_list(rl.resources(1), p2) < 0) {
    printf("%s should be reader\n", p2.c_str());
    return false;
  }
  if (on_reader_list(rl.resources(0), p2) >= 0) {
    printf("%s should not be reader\n", p1.c_str());
    return false;
  }

  if (on_principal_list(p1, pl) < 0) {
    printf("%s should be on principal list\n", p1.c_str());
    return false;
  }
  if (on_resource_list(r1, rl) < 0) {
    printf("%s should be on resource list\n", r1.c_str());
    return false;
  }
  if (on_principal_list(p3, pl) >0 ) {
    printf("%s should NOT be on principal list\n", p3.c_str());
    return false;
  }
  if (on_resource_list(r3, rl) >0 ) {
    printf("%s should NOT be on resource list\n", r3.c_str());
    return false;
  }

  principal_list restored_pl;
  resource_list restored_rl;

  string prin_file("saved_principals.bin");
  if (!save_principals_to_file(pl, prin_file)) {
    printf("Can't save principals file\n");
    return false;
  }
  if (!get_principals_from_file(prin_file, &restored_pl)) {
    printf("Can't recover principals file\n");
    return false;
  }
  if (FLAGS_print_all) {
    printf("Restored principals file\n");
    print_principal_list(restored_pl);
  }

  string resource_file("saved_resource.bin");
  if (!save_resources_to_file(rl, resource_file)) {
    printf("Can't save resources file\n");
    return false;
  }
  if (!get_resources_from_file(resource_file, &restored_rl)) {
    printf("Can't recover resources file\n");
    return false;
  }
  if (FLAGS_print_all) {
    printf("Restored resources file\n");
    print_resource_list(restored_rl);
  }

  if (!init_crypto()) {
    printf("Couldn't init crypto\n");
    return false;
  }
  byte nonce[32];
  int n =  crypto_get_random_bytes(32, nonce);
  if(n < 32) {
    printf("Couldn't get nonce\n");
    return false;
  }
  printf("Nonce: ");
  print_bytes(n, nonce);
  printf("\n");
  close_crypto();
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

  channel_guard guard;

  string channel_prin("john");
  if (!guard.authenticate(channel_prin, pl)) {
    printf("Cant authenticate %s\n", channel_prin.c_str());
    return false;
  }
  printf("Channel principal set\n");

  if (!guard.load_resources(rl)) {
    printf("Cant load resource list\n");
    return false;
  }
  printf("resource list loaded\n");

  string res1("file_1");
  string acc1("read");
  if (!guard.open_resource(res1, acc1)) {
    printf("open_resource failed\n");
    return false;
  }
#if 0
  bool channel_guard::create_resource(string& name)
  bool channel_guard::add_access_rights(string& right, access_list& al)
  bool channel_guard::read_resource(string& resource_name)
  bool channel_guard::write_resource(string& resource_name) {
  bool channel_guard::close_resource(string& resource_name)
  channel_guard::close_resource(string& resource_name)
  bool channel_guard::save_principals(string& master_principal_list)
  bool channel_guard::save_resources(string& master_resource_list)
#endif

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

