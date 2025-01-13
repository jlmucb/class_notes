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

// For testing only
#ifdef TEST_SIMULATED_CHANNEL
const int max_size_buf = 2048;
int size_buf = 0;
byte simulated_buf[max_size_buf];

int simulated_sized_buf_read(string* out) {
  out->assign((char*)simulated_buf, size_buf);
  return size_buf;
}

int simulated_buf_write(int n, byte* b) {
  if (n > max_size_buf)
    return -1;
  memcpy(simulated_buf, b, n);
  size_buf = n;
}
#endif

// Functions supported
string authenticate_me_tag("authenticate_me");
string verify_me_tag("verify_me");
string open_resource_tag("open_resource");
string close_resource_tag("close_resource");
string read_resource_tag("read_resource");
string write_resource_tag("write_resource");
string add_access_right_tag("add_access_right");


acl_client_dispatch::acl_client_dispatch(SSL* channel) {
  channel_descriptor_ = channel;
  initialized_ = true;
}

acl_client_dispatch::~acl_client_dispatch() {
}

bool acl_client_dispatch::rpc_authenticate_me(const string& principal_name,
                                              string* output) {

  string decode_parameters_str;
  string encode_parameters_str;
  rpc_call input_call_struct;
  rpc_call output_call_struct;
  int bytes_read = 0;

  // format input buffer, serialize it
  input_call_struct.set_function_name(authenticate_me_tag);
  string* in = input_call_struct.add_str_inputs();
  *in = principal_name;

  if (!input_call_struct.SerializeToString(&encode_parameters_str)) {
    printf("%s() error, line %d: Can't input\n",
           __func__, __LINE__);
    return false;
  }
#ifndef TEST_SIMULATED_CHANNEL
  if (sized_ssl_write(channel_descriptor_, encode_parameters_str.size(),
                      (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write to channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif

#ifndef TEST_SIMULATED_CHANNEL
  bytes_read= sized_ssl_read(channel_descriptor_, &decode_parameters_str);
  if (bytes_read < 0) {
    printf("%s() error, line %d: Can't read from channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_sized_buf_read(&encode_parameters) < 0) {
    printf("%s() error, line %d: Can't read from channel\n", __func__, __LINE__);
    return false;
  }
#endif

  if (!output_call_struct.ParseFromString(decode_parameters_str)) {
    printf("%s() error, line %d: Can't parse return buffer\n",
           __func__, __LINE__);
    return false;
  } 
  if (output_call_struct.function_name() != authenticate_me_tag) {
    printf("%s() error, line %d: wrong function name tag\n",
           __func__, __LINE__);
    return false;
  }
  bool ret = output_call_struct.status();
  if (!ret) {
    return false;
  }
  if (output_call_struct.buf_outputs_size() < 1) {
    printf("%s() error, line %d: missing return argument\n",
           __func__, __LINE__);
    return false;
  }
  const string& out_nonce = output_call_struct.buf_outputs(0);
  output->assign(out_nonce.data(), out_nonce.size());
  return true;
}

bool acl_client_dispatch::rpc_verify_me(const string& principal_name,
                                        const string& signed_nonce) {
  string decode_parameters_str;
  string encode_parameters_str;
  rpc_call input_call_struct;
  rpc_call output_call_struct;
  int bytes_read= 0;

  // format input buffer, serialize it
  input_call_struct.set_function_name(verify_me_tag);
  string* in = input_call_struct.add_buf_inputs();
  *in = signed_nonce;

  if (!input_call_struct.SerializeToString(&encode_parameters_str)) {
    printf("%s() error, line %d: Can't input\n", __func__, __LINE__);
    return false;
  }

#ifndef TEST_SIMULATED_CHANNEL
  if (sized_ssl_write(channel_descriptor_, encode_parameters_str.size(),
                      (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write to channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif

#ifndef TEST_SIMULATED_CHANNEL
  bytes_read = sized_ssl_read(channel_descriptor_, &decode_parameters_str);
  if (bytes_read < 0) {
    printf("%s() error, line %d: Can't read from channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_sized_buf_read(&decode_parameters) < 0) {
    printf("%s() error, line %d: Can't read from channel\n", __func__, __LINE__);
    return false;
  }
#endif

  if (!output_call_struct.ParseFromString(decode_parameters_str)) {
    printf("%s() error, line %d: Can't parse return buffer\n",
           __func__, __LINE__);
    return false;
  } 
  if (output_call_struct.function_name() != verify_me_tag) {
    printf("%s() error, line %d: wrong function name tag\n",
           __func__, __LINE__);
    return false;
  }
  bool ret = output_call_struct.status();
  if (!ret) {
    return false;
  }
  return true;
}

bool acl_client_dispatch::rpc_open_resource(const string& resource_name,
                                            const string& access_right) {
  string decode_parameters_str;
  string encode_parameters_str;
  rpc_call input_call_struct;
  rpc_call output_call_struct;
  int bytes_read= 0;

  // format input buffer, serialize it
  input_call_struct.set_function_name(open_resource_tag);
  string* pr_name = input_call_struct.add_str_inputs();
  *pr_name = resource_name;
  string* pr_access = input_call_struct.add_str_inputs();
  *pr_access = access_right;

  if (!input_call_struct.SerializeToString(&encode_parameters_str)) {
    printf("%s() error, line %d: Can't input\n",
           __func__, __LINE__);
    return false;
  }

#ifndef TEST_SIMULATED_CHANNEL
  if (sized_ssl_write(channel_descriptor_, encode_parameters_str.size(),
                      (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write to channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif

#ifndef TEST_SIMULATED_CHANNEL
  bytes_read = sized_ssl_read(channel_descriptor_, &decode_parameters_str);
  if (bytes_read < 0) {
    printf("%s() error, line %d: Can't read from channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_sized_buf_read(&decode_parameters) < 0) {
    printf("%s() error, line %d: Can't read from channel\n", __func__, __LINE__);
    return false;
  }
#endif

  if (!output_call_struct.ParseFromString(decode_parameters_str)) {
    printf("%s() error, line %d: Can't parse return buffer\n",
           __func__, __LINE__);
    return false;
  } 
  if (output_call_struct.function_name() != open_resource_tag) {
    printf("%s() error, line %d: wrong function name tag\n",
           __func__, __LINE__);
    return false;
  }
  bool ret = output_call_struct.status();
  if (!ret) {
    return false;
  }
  return true;
}

bool acl_client_dispatch::rpc_read_resource(const string& resource_name,
                                            int num_bytes,
                                            string* bytes_output) {
  string decode_parameters_str;
  string encode_parameters_str;
  rpc_call input_call_struct;
  rpc_call output_call_struct;
  int bytes_read_ret = 0;

  // format input buffer, serialize it
  input_call_struct.set_function_name(read_resource_tag);
  input_call_struct.add_int_inputs((::int32_t)num_bytes);

  if (!input_call_struct.SerializeToString(&encode_parameters_str)) {
    printf("%s() error, line %d: Can't input\n",
           __func__, __LINE__);
    return false;
  }


#ifndef TEST_SIMULATED_CHANNEL
  if (sized_ssl_write(channel_descriptor_, encode_parameters_str.size(),
                      (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write to channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif

  // the following to be replaced by sized_ssl_read(SSL *ssl, string *out);
#ifndef TEST_SIMULATED_CHANNEL
  bytes_read_ret  = sized_ssl_read(channel_descriptor_, &decode_parameters_str);
  if (bytes_read_ret  < 0) {
    printf("%s() error, line %d: Can't read from channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_sized_buf_read(&decode_parameters) < 0) {
    printf("%s() error, line %d: Can't read from channel\n", __func__, __LINE__);
    return false;
  }
#endif

  if (!output_call_struct.ParseFromString(decode_parameters_str)) {
    printf("%s() error, line %d: Can't parse return buffer\n",
           __func__, __LINE__);
    return false;
  } 
  if (output_call_struct.function_name() != read_resource_tag) {
    printf("%s() error, line %d: wrong function name tag\n",
           __func__, __LINE__);
    return false;
  }
  bool ret = output_call_struct.status();
  if (!ret) {
    return false;
  }
  if (output_call_struct.buf_outputs_size() < 1) {
    return false;
  }
  *bytes_output= output_call_struct.buf_outputs(0);
  return true;
}

bool acl_client_dispatch::rpc_write_resource(const string& resource_name,
                                             const string& bytes_to_write) {

  string decode_parameters_str;
  string encode_parameters_str;
  rpc_call input_call_struct;
  rpc_call output_call_struct;
  int bytes_read= 0;

  // format input buffer, serialize it
  input_call_struct.set_function_name(write_resource_tag);
  string* buf_to_write = input_call_struct.add_buf_inputs();
  *buf_to_write = bytes_to_write;

  if (!input_call_struct.SerializeToString(&encode_parameters_str)) {
    printf("%s() error, line %d: Can't input\n",
           __func__, __LINE__);
    return false;
  }


#ifndef TEST_SIMULATED_CHANNEL
  if (sized_ssl_write(channel_descriptor_, encode_parameters_str.size(),
                      (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write to channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif

#ifndef TEST_SIMULATED_CHANNEL
  bytes_read = sized_ssl_read(channel_descriptor_, &decode_parameters_str);
  if (bytes_read < 0) {
    printf("%s() error, line %d: Can't read from channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_sized_buf_read(&decode_parameters) < 0) {
    printf("%s() error, line %d: Can't read from channel\n", __func__, __LINE__);
    return false;
  }
#endif

  if (!output_call_struct.ParseFromString(decode_parameters_str)) {
    printf("%s() error, line %d: Can't parse return buffer\n",
           __func__, __LINE__);
    return false;
  } 
  if (output_call_struct.function_name() != write_resource_tag) {
    printf("%s() error, line %d: wrong function name tag\n",
           __func__, __LINE__);
    return false;
  }
  bool ret = output_call_struct.status();
  if (!ret) {
    return false;
  }
  return true;
}

bool acl_client_dispatch::rpc_close_resource(const string& resource_name) {
  string decode_parameters_str;
  string encode_parameters_str;
  rpc_call input_call_struct;
  rpc_call output_call_struct;
  int bytes_read= 0;

  // format input buffer, serialize it
  input_call_struct.set_function_name(close_resource_tag);
  string* in = input_call_struct.add_buf_inputs();
  *in = resource_name;

  if (!input_call_struct.SerializeToString(&encode_parameters_str)) {
    printf("%s() error, line %d: Can't input\n",
           __func__, __LINE__);
    return false;
  }

#ifndef TEST_SIMULATED_CHANNEL
  if (sized_ssl_write(channel_descriptor_, encode_parameters_str.size(),
                      (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write to channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif

#ifndef TEST_SIMULATED_CHANNEL
  bytes_read = sized_ssl_read(channel_descriptor_, &decode_parameters_str);
  if (bytes_read < 0) {
    printf("%s() error, line %d: Can't read from channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_sized_buf_read(&decode_parameters) < 0) {
    printf("%s() error, line %d: Can't read from channel\n", __func__, __LINE__);
    return false;
  }
#endif

  if (!output_call_struct.ParseFromString(decode_parameters_str)) {
    printf("%s() error, line %d: Can't parse return buffer\n",
           __func__, __LINE__);
    return false;
  } 
  if (output_call_struct.function_name() != close_resource_tag) {
    printf("%s() error, line %d: wrong function name tag\n",
           __func__, __LINE__);
    return false;
  }
  bool ret = output_call_struct.status();
  if (!ret) {
    return false;
  }
  return true;
}

bool acl_client_dispatch::rpc_add_access_right(const string& resource_name,
                                               const string& delegated_principal,
                                               const string& right) {
  return false;
}


acl_server_dispatch::acl_server_dispatch(SSL* channel) {
  channel_descriptor_ = channel;
  initialized_ = true;
}

acl_server_dispatch::~acl_server_dispatch() {
}

bool acl_server_dispatch::load_principals(principal_list& pl) {
  for (int i = 0; i < pl.principals_size(); i++) {
    principal_message* pm = principal_list_.add_principals();
    pm->CopyFrom(pl.principals(i));
  }

  return true;
}

bool acl_server_dispatch::load_resources(resource_list& rl) {
  for (int i = 0; i < rl.resources_size(); i++) {
    resource_message* rm = resource_list_.add_resources();
    rm->CopyFrom(rl.resources(i));
  }
  return true;
}

bool acl_server_dispatch::service_request() {

  string decode_parameters_str;
  string encode_parameters_str;
  rpc_call input_call_struct;
  rpc_call output_call_struct;
  int bytes_read= 0;

  if (!initialized_) {
    printf("%s() error, line %d: acl_server_dispatch not initialized\n",
           __func__, __LINE__);
    return false;
  }

  // read the buffer
  // Following line to be replaced by int sized_ssl_read(SSL *ssl, string *out);
#ifndef TEST_SIMULATED_CHANNEL
  bytes_read = sized_ssl_read(channel_descriptor_, &decode_parameters_str);
  if (bytes_read < 0) {
    return false;
  }
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif

  if (!input_call_struct.ParseFromString(decode_parameters_str)) {
    printf("%s() error, line %d: Can't parse call proto %d\n",
           __func__, __LINE__, (int)decode_parameters_str.size());
    return false;
  }

  if(input_call_struct.function_name() == authenticate_me_tag) {
    if (input_call_struct.str_inputs_size() < 1) {
      return false;
    }
    string nonce;
    if (guard_.authenticate_me(input_call_struct.str_inputs(0), principal_list_, &nonce)) {
        output_call_struct.set_status(true);
        string* nounce_out = output_call_struct.add_str_outputs();
        nounce_out->assign(nonce.data(), nonce.size());
    } else {
        output_call_struct.set_status(false);
    }
    output_call_struct.set_function_name(authenticate_me_tag);
    if (!output_call_struct.SerializeToString(&encode_parameters_str)) {
      printf("%s() error, line %d: can't encode parameters\n",
           __func__, __LINE__);
      return false;  // and the caller never knows
    }

#ifndef TEST_SIMULATED_CHANNEL
  if (sized_ssl_write(channel_descriptor_, encode_parameters_str.size(),
                      (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write to channel\n",
           __func__, __LINE__);
    return false;
  }
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif
  } else if(input_call_struct.function_name() == verify_me_tag) {
    if (input_call_struct.str_inputs_size() < 1) {
      return false;
    }
    if (input_call_struct.buf_inputs_size() < 1) {
      return false;
    }
    if (guard_.verify_me(input_call_struct.str_inputs(0),
                         input_call_struct.buf_inputs(0))) {
        output_call_struct.set_status(true);
    } else {
        output_call_struct.set_status(false);
    }
    output_call_struct.set_function_name(verify_me_tag);
    if (!output_call_struct.SerializeToString(&encode_parameters_str)) {
      printf("%s() error, line %d: can't encode parameters\n",
           __func__, __LINE__);
      return false;  // and the caller never knows
    }


#ifndef TEST_SIMULATED_CHANNEL
  if (sized_ssl_write(channel_descriptor_, encode_parameters_str.size(),
                      (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write to channel\n",
           __func__, __LINE__);
    return false;
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif
    }
    return false;
  } else if(input_call_struct.function_name() == open_resource_tag) {
    if (input_call_struct.str_inputs_size() < 2) {
      return false;
    }
    if (guard_.open_resource(input_call_struct.str_inputs(0),
                             input_call_struct.str_inputs(1))) {
        output_call_struct.set_status(true);
    } else {
        output_call_struct.set_status(false);
    }
    output_call_struct.set_function_name(open_resource_tag);
    if (!output_call_struct.SerializeToString(&encode_parameters_str)) {
      printf("%s() error, line %d: can't encode parameters\n",
           __func__, __LINE__);
      return false;  // and the caller never knows
    }
#ifndef TEST_SIMULATED_CHANNEL
    if (sized_ssl_write(channel_descriptor_, encode_parameters_str.size(),
                      (byte*)encode_parameters_str.data()) < 0) {
      printf("%s() error, line %d: Can't write to channel\n",
           __func__, __LINE__);
      return false;
    }
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif
  } else if(input_call_struct.function_name() == close_resource_tag) {
    if (input_call_struct.str_inputs_size() < 1) {
      return false;
    }
    if (guard_.close_resource(input_call_struct.str_inputs(0))) {
        output_call_struct.set_status(true);
    } else {
        output_call_struct.set_status(false);
    }
    output_call_struct.set_function_name(close_resource_tag);
    if (!output_call_struct.SerializeToString(&encode_parameters_str)) {
      printf("%s() error, line %d: can't encode parameters\n",
           __func__, __LINE__);
      return false;  // and the caller never knows
    }

#ifndef TEST_SIMULATED_CHANNEL
    if (sized_ssl_write(channel_descriptor_, encode_parameters_str.size(),
                        (byte*)encode_parameters_str.data()) < 0) {
      printf("%s() error, line %d: Can't write to channel\n",
            __func__, __LINE__);
      return false;
    }
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif
  } else if(input_call_struct.function_name() == read_resource_tag) {
    if (input_call_struct.str_inputs_size() < 1) {
      return false;
    }
    if (input_call_struct.int_inputs_size() < 1) {
      return false;
    }
    string out;
    if (guard_.read_resource(input_call_struct.str_inputs(0),
                             input_call_struct.int_inputs(0),
                             &out)) {
        output_call_struct.set_status(true);
        string* ret_out = output_call_struct.add_str_outputs();
        ret_out->assign(out.data(), out.size());
    } else {
        output_call_struct.set_status(false);
    }
    output_call_struct.set_function_name(read_resource_tag);
    if (!output_call_struct.SerializeToString(&encode_parameters_str)) {
      printf("%s() error, line %d: can't encode parameters\n",
           __func__, __LINE__);
      return false;  // and the caller never knows
    }

#ifndef TEST_SIMULATED_CHANNEL
    if (sized_ssl_write(channel_descriptor_, encode_parameters_str.size(),
                        (byte*)encode_parameters_str.data()) < 0) {
      printf("%s() error, line %d: Can't write to channel\n",
            __func__, __LINE__);
      return false;
    }
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif
    return true;
  } else if(input_call_struct.function_name() == write_resource_tag) {
    if (input_call_struct.str_inputs_size() < 1) {
      return false;
    }
    if (input_call_struct.int_inputs_size() < 1) {
      return false;
    }
    if (input_call_struct.buf_inputs_size() < 1) {
      return false;
    }
    if (guard_.write_resource(input_call_struct.str_inputs(0), 
                              input_call_struct.int_inputs(0),
                              (string&)input_call_struct.buf_inputs(0))) {
        output_call_struct.set_status(true);
    } else {
        output_call_struct.set_status(false);
    }
    output_call_struct.set_function_name(write_resource_tag);
    if (!output_call_struct.SerializeToString(&encode_parameters_str)) {
      printf("%s() error, line %d: can't encode parameters\n",
           __func__, __LINE__);
      return false;  // and the caller never knows
    }

#ifndef TEST_SIMULATED_CHANNEL
    if (sized_ssl_write(channel_descriptor_, encode_parameters_str.size(),
                        (byte*)encode_parameters_str.data()) < 0) {
      printf("%s() error, line %d: Can't write to channel\n",
            __func__, __LINE__);
      return false;
    }
#else
  if (simulated_buf_write(encode_parameters_str.size(), (byte*)encode_parameters_str.data()) < 0) {
    printf("%s() error, line %d: Can't write\n", __func__, __LINE__);
    return false;
  }
#endif
    return true;
  } else if(input_call_struct.function_name() == add_access_right_tag) {
  } else {
    printf("%s() error, line %d: unknown function\n", __func__, __LINE__);
    return false;
  }

  return true;
}


