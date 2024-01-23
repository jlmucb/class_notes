//
// Copyright 2014 John Manferdelli, All Rights Reserved.
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
// File: mog.cc

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <string>
#include <iostream>
#include <fstream>
#include <memory>

#include <gtest/gtest.h>
#include <gflags/gflags.h>
using std::string;

int num_mog_ops = 3;
std::string mog_ops[] = {
    "--operation=list_golay",
    "--operation=list_hexacode",
    "--operation=transform_points",
};

DEFINE_string(operation, "", "operations");
DEFINE_int32(pt_num, 0, "point to transform");
DEFINE_bool(options, false, "options");

void print_options() {
  printf("Permitted operations:\n");
  for (int i = 0; i < num_mog_ops; i++) {
    printf("  mog.exe %s\n", mog_ops[i].c_str());
  }
  return;
}

int plus[16] = {
  0, 1, 2, 3,
  1, 0, 3, 2,
  2, 3, 0, 1,
  3, 2, 1, 0,
};

int times[16] = {
  0, 0, 0, 0,
  0, 1, 2, 3,
  0, 2, 3, 1,
  0, 3, 1, 2,
};

int add(int a, int b) {
  return plus[a * 4 + b];
}

int multiply(int a, int b) {
  return times[a * 4 + b];
}

class hxvec {
public:
  int x_[6];
};

hxvec Hexacodebasis[3] = {
  {{3, 2, 3, 2, 2, 3}},
  {{2, 3, 3, 2, 3, 2}},
  {{3, 2, 2, 3, 3, 2}},
};

hxvec Hexacode[64];

void addvec(hxvec& a, hxvec& b, hxvec& c) {
  for (int i = 0; i < 6; i++)
    c.x_[i] = add(a.x_[i], b.x_[i]);
}

void multvec(int a, hxvec& b, hxvec& c) {
  for (int i = 0; i < 6; i++)
    c.x_[i] = multiply(a, b.x_[i]);
}

class point {
public:
  int x_, y_;
};

void actonPoint(point& a, hxvec& b, point& c) {
  c.x_ = a.x_;
  c.y_ = add(b.x_[a.x_], a.y_);
}

class set {
public:
  point pt_[24];
};

void actonSet(hxvec& a, set& b, set& c) {
  for (int i = 0; i < 24; i++)
    actonPoint(b.pt_[i], a, c.pt_[i]);
}

void PrintHex(hxvec& a) {
  printf("(");
  for (int i = 0; i < 5; i++)
    printf("%d, ", a.x_[i]);
  printf("%d)", a.x_[5]);
}

void PrintPoint(point a) {
  printf("(%d, %d)", a.x_, a.y_);
}

void PrintSet(set& a) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
    PrintPoint(a.pt_[6 * i + j]);
    printf(", ");
    }
  printf("\n");
  }
}

set Golay;

void init_golay() {
  int n = 0;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 6; j++) {
      Golay.pt_[n].x_ = j;
      Golay.pt_[n].y_ = i;
      n++;
    }
  }
}

void init_hexacode() {
  int n = 0;
  for (int i1 = 0; i1 < 4; i1++) {
    for (int i2 = 0; i2 < 4; i2++) {
      for (int i3 = 0; i3 < 4; i3++) {
        hxvec a, b, c, t;
        multvec(i1, Hexacodebasis[0], a);
        multvec(i2, Hexacodebasis[1], b);
        multvec(i3, Hexacodebasis[2], c);

        addvec(a, b, t);
        addvec(t, c, Hexacode[n]);
        n++;
      }
    }
  }
}

void transform_point(hxvec& a, point& b, point& c) {
}

// ---------------------------------------------------------------------


int main(int an, char** av) {
#ifdef __linux__
  gflags::ParseCommandLineFlags(&an, &av, true);
#else
  google::ParseCommandLineFlags(&an, &av, true);
#endif

  if (FLAGS_options) {
    printf("Options:\n");
    for (int j = 0; j< num_mog_ops; j++) {
      printf("\t%s \n", mog_ops[j].c_str());
    }
    return 0;
  }

  if (FLAGS_operation == "") {
    std::cout << "No operation specified.\n\n";
    print_options();
    return 1;
  }
  printf("operation flag: %s\n", FLAGS_operation.c_str());

  if (FLAGS_operation == "list_golay") {
    init_golay();
    PrintSet(Golay);
  } else if (FLAGS_operation == "list_hexacode") {
    init_hexacode();
    for (int i = 0; i < 64; i++) {
      PrintHex(Hexacode[i]);
      printf("\n");
    }
  } else if (FLAGS_operation == "transform_points") {
    init_golay();
    init_hexacode();
    for (int j = 0; j < 64; j++) {
      point t;
      PrintHex(Hexacode[j]);
      printf("[");
      PrintPoint(Golay.pt_[FLAGS_pt_num]);
      printf("]= ");
      actonPoint(Golay.pt_[FLAGS_pt_num], Hexacode[j], t);
      PrintPoint(t);
      printf("\n");
    }
    printf("\n");
  } else {
    printf("Unimplemented option\n");
  return 1;
  }
  return 0;
}

