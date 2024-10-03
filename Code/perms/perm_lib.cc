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

#include <perms.h>

void print_perm(int n, byte* a) {
  printf("[ ");
  for (int i = 0; i < n; i++)
    printf("%02d ", a[i]);
  printf("] ");
}

void print_perm_cycles(int n, byte* a) {
  bool something_printed = false;
  bool seen[n];

  for (int i = 0; i < n; i++)
    seen[i] = false;

  for (int i = 0; i < n; i++) {
    if (seen[i])
      continue;

    int last = i + 1;
    seen[i] = true;
    if (a[i] == last)
      continue;
    something_printed = true;

    printf("(%2d ", last);
    for (;;) {
      int k = a[last - 1];
      int index = k - 1;
      last = k;
      if (seen[index]) {
        printf(")");
        break;
      }
      printf("%2d ", k);
      seen[index] = true;
    }
  }

  if(!something_printed)
    printf("1");
}

bool multiply_perms(int n, byte* a, byte* b, byte* r) {
  for (int i = 0; i < n; i++) {
    r[i] = b[a[i]-1];
  }
  return true;
}

bool inverse_perms(int n, byte* a, byte* b) {
  for (int i = 0; i < n; i++) {
    b[a[i] - 1] = i + 1;
  }
  return true;
}

bool perms_equal(int n, byte* a, byte* b) {
  for (int i = 0; i < n; i++) {
    if (a[i] != b[i])
      return false;
  }
  return true;
}

bool is_identity(int n, byte* a) {
  for (int i = 0; i < n; i++) {
    if (a[i] != (i + 1))
      return false;
  }
  return true;
}

bool parse_perm_string(const char* s, int n, byte* a) {
  if(strlen(s) != n)
    return false;

  for (int i = 0; i < n; i++) {
    int j = (int)s[i] - (int)'0';
     if (j < 0 || j > 9)
      return false;
    a[i] = j;
  } 

  return true;
}

bool commutes(int n, byte* a, byte* b) {
  byte r1[n];
  byte r2[n];

  if (!multiply_perms(n, a, b, r1))
    return false;
  if (!multiply_perms(n, b, a, r2))
    return false;
  return perms_equal(n, r1, r2);
}
