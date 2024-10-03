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

#ifndef PERMS_INCLUDE
#define PERMS_INCLUDE

#ifndef byte
typedef unsigned char byte;
#endif

void print_perm(int n, byte* a);
void print_perm_cycles(int n, byte* a);

bool multiply_perms(int n, byte* a, byte* b ,byte* r);
bool inverse_perms(int n, byte* a, byte* b);
bool perms_equal(int n, byte* a, byte* b);
bool is_identity(int n, byte* a);
bool commutes(int n, byte* a, byte* b);

bool parse_perm_string(const char* s, int n, byte* a);

#endif
