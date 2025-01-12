// Copyright 2025 John Manferdelli, All Rights Reserved.
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
// File: acl_rpc.cc

#include "acl_rpc.h"
#include "acl.pb.h"

acl_client_dispatch::acl_client_dispatch(int channel) {
  channel_descriptor_ = channel;
  initialized_ = true;
}

acl_client_dispatch::~acl_client_dispatch() {
}

bool acl_client_dispatch::rpc_authenticate_me(const string& principal_name,
                                              string* output) {
  return false;
}

bool acl_client_dispatch::rpc_verify_me(const string& principal_name,
                                        const string& signed_nonce) {
  return false;
}

bool acl_client_dispatch::rpc_open_resource(const string& resource_name,
                                            const string& access_right) {
  return false;
}

bool acl_client_dispatch::rpc_read_resource(const string& resource_name,
                                            int num_bytes,
                                            string* bytes_read) {
  return false;
}

bool acl_client_dispatch::rpc_write_resource(const string& resource_name,
                                             const string& bytes_to_write) {
  return false;
}

bool acl_client_dispatch::rpc_close_resource(const string& resource_name) {
  return false;
}

bool acl_client_dispatch::rpc_add_access_right(const string& resource_name,
                                               const string& delegated_principal,
                                               const string& right) {
  return false;
}


acl_server_dispatch::acl_server_dispatch(int channel) {
  channel_descriptor_ = channel;
  initialized_ = true;
}

acl_server_dispatch::~acl_server_dispatch() {
}

bool acl_server_dispatch::service_request() {
  return true;
}


