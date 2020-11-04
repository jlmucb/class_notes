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
// File: mbm23.cc

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

int num_mbm23_ops = 13;
std::string mbm23_ops[] = {
    "--operation=inverse_table",
    "--operation=apply_transform",
    "--operation=apply_perms",
    "--operation=print_golay",
    "--operation=print_m24_generators",
    "--operation=gen_code_weights",
    "--operation=calc_golay",
    "--modulus=m",
    "--a=int",
    "--b=int",
    "--c=int",
    "--d=int",
    "--w=int"
};

void print_options() {
  printf("Permitted operations:\n");
  for (int i = 0; i < num_mbm23_ops; i++) {
    printf("  mbm23.exe %s\n", mbm23_ops[i].c_str());
  }
  return;
}

DEFINE_string(operation, "", "operations");
DEFINE_int32(modulus, 23, "modulus");
DEFINE_int32(a, 1, "a");
DEFINE_int32(b, 0, "b");
DEFINE_int32(c, 0, "c");
DEFINE_int32(d, 1, "d");
DEFINE_int32(w, 1, "weight");
DEFINE_bool(options, false, "options");


// ---------------------------------------------------------------------

const int g_infinity = 9999;
const int g_max_modulus = 128;
int g_inverse_table[g_max_modulus];

int normalize(int x, int m) {
  int y = x;

  while (y < 0)
    y += m;
  while (y >= m)
    y -= m;

  return y;
}

int inverse(int x, int m) {
  for (int j = 1; j < m; j++) {
    if (normalize(j * x, m) == 1)
      return j;
  }
  return 0;
}

void PrintNum(int x) {
  if (x == g_infinity)
    printf("inf");
  else
    printf("%03d", x);
}

void FillInverseTable(int m) {
  g_inverse_table[0] = g_infinity;
  for (int j = 1; j <= m; j++)
    g_inverse_table[j] = inverse(j, m);
  g_inverse_table[m] = 0;
}

int inv(int x, int m) {
  if (x == g_infinity)
    return g_inverse_table[m];
  return g_inverse_table[x];
}

int ApplyMobius(int a, int b, int c, int d, int m, int x) {
  if (x == g_infinity) {
    if (c == 0)
      return g_infinity; 
    return normalize(a * inv(c, m), m);
  }

  int t = normalize(c * x + d, m);
  if (t == 0)
    return g_infinity;

  return normalize((a * x + b) * inv(t,m), m);
}

bool CalcAllMobius(int a, int b, int c, int d, int m, int* out) {
  int det = normalize(a * d - b * c, m);

  if (det == 0) {
    printf("Bad determinant\n");
    return false;
  }

  for (int j = 0; j < m; j++ ) {
    out[j] = ApplyMobius(a, b, c, d, m, j);
  }
  out[m] = ApplyMobius(a, b, c, d, m, g_infinity);
  return true;
}

int Dist(int* v1, int* v2, int n) {
  int d = 0;

  for (int j = 0; j < n; j++) {
    if (v1[j] != v2[j])
      d++;
  }
  return d;
}

int Weight(int* v, int n) {
  int w = 0;

  for (int j = 0; j < n; j++) {
    w += v[j];
  }
  return w;
}

int g_Identity[24] = {
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 
  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
};
int g_alpha[24];
int g_beta[24];
int g_gamma[24];
int g_delta[24];

int g_Qr[24];

bool GenQr() {
  int k;

  g_Qr[23] = 0;
  for (int j = 0; j < 23; j++)
    g_Qr[j] = 0;
  for (int j = 0; j < 23; j++) {
    k = (j * j) % 23;
    g_Qr[k] = 1;
  }
  return true;
}

void PrintQr() {
  printf("QR's: ");
  for (int j = 0; j < 23; j++) {
    if (g_Qr[j] == 1)
      printf("%d, ", j);
  }
}

void PrintComponents(int* set, int n) {
  for (int j = 0; j <= n; j++) {
      if (set[j] == 1) {
        PrintNum(j);
        printf(" ");
      }
  }
  printf("\n");
}

void PrintShortSet(int* set, int n) {
  for (int j = 0; j <= n; j++) {
      printf("%1d", set[j]);
  }
  // printf("\n");
}

void PrintSet(int* set, int n) {
  for (int j = 0; j <= n; j++) {
      printf("%02d ", j);
  }
  printf("\n");

  for (int j = 0; j <= n; j++) {
      PrintNum(set[j]);
      printf(" ");
  }
  printf("\n");
}

void PrintMatrix(int* matrix, int rows, int cols, bool withWeight= false) {
  int i;

  for (i = 0; i < rows; i++) {
    printf("Row %2d: ", i + 1);
    PrintShortSet(&matrix[i * cols], cols - 1);
    if (withWeight)
      printf(", wt: %d", Weight(&matrix[i * cols], cols));
    printf("\n");
  }
}

void PrintPerm(int* p, int n) {
  for (int j = 0; j < n; j++) {
      printf("%03d ", j);
  }
  printf("inf\n");

  for (int j = 0; j < n; j++) {
      PrintNum(p[j]);
      printf(" ");
  }
  PrintNum(p[n]);
  printf("\n");
}

void PrintCycles(int* p, int n) {
}

// x --> x+1
bool GenAlpha() {
  g_alpha[23] = g_infinity;
  for (int j = 0; j < 23; j++) {
    g_alpha[j] = (j + 1) % 23;
  }
  return true;
}

// x --> 2x
bool GenBeta() {
  g_beta[23] = g_infinity;
  for (int j = 0; j < 23; j++) {
    g_beta[j] = (2 * j) % 23;
  }
  return true;
}

// x --> -1/x
bool GenGamma() {
  g_gamma[23] = 0;
  g_gamma[0] = g_infinity;
  for (int j = 1; j < 23; j++) {
    g_gamma[j] = (23 - inv(j, 23)) % 23;
  }
  return true;
}

// If x is a QR x --> x*x*x/9, else, x --> 9 x*x*x
bool GenDelta() {
  g_delta[23] = g_infinity;
  for (int j = 0; j < 23; j++) {
    if (g_Qr[j] ==1) {
      g_delta[j] = (23 - (((j * j * j) * inv(9, 23)) % 23)) % 23;
    } else {
      g_delta[j] = (23 - ((9 * (j * j * j)) % 23)) % 23;
    }
  }
  return true;
}

void ApplyProjPerm(int* set, int* perm, int n, int* out) {
  for (int j = 0; j <= n; j++) {
    if (perm[j] == g_infinity || perm[j] == n) {
      out[n] = set[j];
    } else {
      out[perm[j]] = set[j];
    }
  }
}

int g_GolayGen[288]= {
#if 0
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23  
   1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, //  0
   0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, //  1
   0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, //  2
   0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, //  3
   0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, //  4
   0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, //  5
   0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, //  6
   0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, //  7
   0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, //  8
   0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, //  9
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, // 10
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // 11
#else
// 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23  
   1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, //  0
   0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, //  2
   0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, //  3
   0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, //  4
   0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, //  8

   0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, //  6

   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // 11
   0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, //  9
   0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, //  7
   0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, //  5
   0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, //  1
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, // 10
#endif
};

void SwitchGolayCols(int c1, int c2) {
  int t;
  int *p;

  for (int j = 0; j < 12; j++) {
    p = &g_GolayGen[j * 24];
    t= p[c1];
    p[c1] = p[c2];
    p[c2] = t;
  }
}

void SwitchGolayRows(int c1, int c2) {
  int t;
  int *p = &g_GolayGen[c1 * 24];
  int *q = &g_GolayGen[c2 * 24];

  for (int j = 0; j < 24; j++) {
    t= p[j];
    p[j] = q[j];
    q[j] = t;
  }
}

void Rotate(int n) {
  for (int i = 0; i < n; i++) {
    SwitchGolayCols(0, 1);
    SwitchGolayCols(1, 2);
    SwitchGolayCols(2, 3);
    SwitchGolayCols(3, 4);
    SwitchGolayCols(4, 5);
    SwitchGolayCols(5, 6);
    SwitchGolayCols(6, 7);
    SwitchGolayCols(7, 8);
    SwitchGolayCols(8, 9);
    SwitchGolayCols(9, 10);
    SwitchGolayCols(10, 11);
    SwitchGolayCols(11, 12);
    SwitchGolayCols(12, 13);
    SwitchGolayCols(13, 14);
    SwitchGolayCols(14, 15);
    SwitchGolayCols(15, 16);
    SwitchGolayCols(16, 17);
    SwitchGolayCols(17, 18);
    SwitchGolayCols(18, 19);
    SwitchGolayCols(19, 20);
    SwitchGolayCols(20, 21);
    SwitchGolayCols(21, 22);
    SwitchGolayCols(22, 23);
  }
}

void MultOnLeft(int* basisVectors, int numRows, int numCols, int* coeff, int* result) {
  for(int j = 0; j < numCols; j++)
    result[j] = 0;

   // printf("\tcoeff: ");PrintShortSet(coeff, 11);printf("\n");

  for(int i = 0; i < numRows; i++) {
    if (coeff[i] != 0) {
   // printf("\tMultBy %d: ", i); PrintShortSet(&basisVectors[numCols * i], numCols-1);printf("\n");
      for(int j = 0; j < numCols; j++) {
        result[j] = result[j] ^ basisVectors[numCols * i + j];
      }
    }
  }
   // printf("\tresult: ");PrintShortSet(result, 23); printf("\n");
}

int g_GolayCode[24 * 4096];

void ToCoeff(int mask, int numBasis, int* coeff) {
  int t = 1;

  for (int j = 0; j < numBasis; j++) {
    if (t&mask)
      coeff[j] = 1;
    else
      coeff[j] = 0;
    t <<= 1;
  }
}

void swapRow(int* matrix, int r1, int r2) {
  const int N = 24;
  int t;

  for (int i = 0; i < N; i++) {
    t = matrix[N * r1 + i];
    matrix[N * r1 + i] = matrix[N * r2 + i];
    matrix[N * r2 + i] =  t;
  }
}

// Add row r1 to row r2.
void addToRow(int* matrix, int r1, int r2) {
  const int N = 24;

  for (int i = 0; i < N; i++)
    matrix[N * r2 + i] ^= matrix[N * r1 + i];
}


// returns simplified generators.
int SimplifyGolayGens(int* oldgens, int* newgens) {
  const int N = 24;
  int row;
  int i, j;

  for (row = 0; row < 12; row++) {
    // Find a row from this row until the end with 1 in column row.
    for (i = row; i < N; i++) {
      if (oldgens[i * N + row] != 0)
        break;
    }
    if (i >= N)
      return -1;
    if (i != row)
      swapRow(oldgens, row, i);
    for (j = (row + 1); j < N; j++) {
      if (oldgens[N * j + row] != 0)
        addToRow(oldgens, row, j);
    }
  }

  for (i = 0; i < 12; i++) {
    for (j = (i + 1); j < 12; j++) {
      if (oldgens[N * i + j] != 0)
        addToRow(oldgens, j, i);
    }
  }

  for (i = 0; i < 12; i++) {
    for (j = 0; j < N; j++) {
      newgens[N * i + j] = oldgens[N * i +j];
    }
  }
  return 12;
}

bool CalcGolayGens(int* gens) {
  int i;
  const int M = 23;
  const int N = 24;

  if (!GenQr())
    return false;
  if (!GenAlpha())
    return false;

  ApplyProjPerm(g_Qr, g_Identity, M, gens);
  for (i = 1; i < M; i++) {
    ApplyProjPerm(&gens[N * (i - 1)], g_alpha, M, &gens[N * i]);
  }
  int* pX = &gens[N * M];
  for (i = 0; i < N; i++) {
    pX[i] = g_Qr[i] ^ 1; 
  }
  return true;
}

bool GenGolay() {
  int coeff[12];

  for (int mask = 0; mask < 4096; mask++) {
    ToCoeff(mask, 12, coeff);
    MultOnLeft(g_GolayGen, 12, 24, coeff, &g_GolayCode[24 * mask]);
  }
  return true;
}

bool FullGenGolay() {
    int gens[24 * 24];
    if (!CalcGolayGens(gens)) {
      printf("Can't calculate original golay generators\n");
      return 1;
    }
#ifdef DEBUG
  printf("\nGenerators:\n");
  PrintMatrix(gens, 24, 24, true);
  printf("\n");
#endif
    if (SimplifyGolayGens(gens, g_GolayGen) < 0) {
      printf("Can't simplify golay generators\n");
      return 1;
    }

#ifdef DEBUG
    printf("Golay generators used:\n");
    PrintMatrix(g_GolayGen, 12, 24, true);
#endif

    GenGolay();
  return true;
}

void PrintGolay() {
  for (int i = 0; i < 4096; i++) {
    int *p = &g_GolayCode[24 * i];
    for (int j = 0; j< 24; j++)
      printf("%01d", p[j]);
    printf("\n");
  }
}

bool SameVector(int *v1, int* v2, int n) {
  for (int j = 0; j <= n; j++) {
    if (v1[j] != v2[j])
      return false;
  }
  return true;
}

void MissedDistFromGolay(int* set) {
  int min_dist = 24;
  int k;
  int pos = 0;
  int* p;

  for (int i = 0; i < 4096; i++) {
    p = &g_GolayCode[24 * i];
    k = Dist(p, set, 24); 
    if (k < min_dist) {
      min_dist = k;
      pos = i;
    }
  }
  p = &g_GolayCode[24 * pos];
  printf("missed at ");
  for (int j = 0; j < 24; j++) {
    if (p[j] != set[j])
      printf("%d ", j);
  }
  printf(" Vectors: ");
  PrintShortSet(set, 23);
  printf(", ");
  PrintShortSet(p, 23);
  printf("\n");
}

int DistFromGolay(int* set) {
  int min_dist = 24;
  int k;

  for (int i = 0; i < 4096; i++) {
    int* p = &g_GolayCode[24 * i];
    k = Dist(p, set, 24); 
    if (k < min_dist)
      min_dist = k;
  }
  return min_dist;
}

bool SetInGolay(int* set) {
  for (int i = 0; i < 4096; i++) {
    int* p = &g_GolayCode[24 * i];
    if (SameVector(set, p, 23))
      return true;
  }
  return false;
}

int main(int an, char** av) {
#ifdef __linux__
  gflags::ParseCommandLineFlags(&an, &av, true);
#else
  google::ParseCommandLineFlags(&an, &av, true);
#endif

  if (FLAGS_options) {
    printf("Options:\n");
    for (int j = 0; j< num_mbm23_ops; j++) {
      printf("\t%s \n", mbm23_ops[j].c_str());
    }
    return 0;
  }

  if (FLAGS_operation == "") {
    std::cout << "No operation specified.\n\n";
    print_options();
    return 1;
  }
  printf("operation flag: %s\n", FLAGS_operation.c_str());

  // Inverse table
  int m = FLAGS_modulus;
  if (m > g_max_modulus) {
    printf("Modulus too big\n");
  }
  FillInverseTable(m);

  // Generate M(24)
  GenQr();
  GenAlpha();
  GenBeta();
  GenGamma();
  GenDelta();

  // Generate Golay Code
  FullGenGolay();

  if (FLAGS_operation == "inverse_table") {
    for (int j = 0; j < m; j++) {
      printf("1/");
      PrintNum(j);
      printf(" = ");
      PrintNum(g_inverse_table[j]);
      printf(" (mod %d)\n", m);
    }
    printf("1/inf");
    printf(" = ");
    PrintNum(g_inverse_table[m]);
    printf(" (mod %d)\n", m);
    return 0;
  } else if (FLAGS_operation == "gen_code_weights") {
    printf("Weight %d codewords\n", FLAGS_w);
    int n = 0;
    for (int j = 0; j < 4096; j++) {
      int *p = &g_GolayCode[24 * j];
      if (Weight(p, 24) == FLAGS_w) {
        PrintShortSet(p, 23); printf("       ");
        PrintComponents(p, 23);
        n++;
      }
    }
    printf("%d code words\n", n);
    return 0;
  } else if (FLAGS_operation == "print_m24_generators") {
    PrintQr();
    printf("\n");
    printf("alpha: \n");
    PrintPerm(g_alpha, 23);
    printf("\n");
    printf("beta: \n");
    PrintPerm(g_beta, 23);
    printf("\n");
    printf("gamma: \n");
    PrintPerm(g_gamma, 23);
    printf("\n");
    printf("delta: \n");
    PrintPerm(g_delta, 23);
    printf("\n");
    return 0;
  } else if (FLAGS_operation == "print_golay") {
    printf("Golay:\n");
    PrintGolay();
    printf("\n");
    return 0;
  } else if (FLAGS_operation == "calc_golay") {
    int gens[24 * 24];
    if (!CalcGolayGens(gens)) {
      printf("Can't calculate original golay generators\n");
      return 1;
    }
    if (SimplifyGolayGens(gens, g_GolayGen) < 0) {
      printf("Can't simplify golay generators\n");
      return 1;
    }
    return 0;
  } else if (FLAGS_operation == "apply_perms") {
#if 0
    int gens[24 * 24];
    if (!CalcGolayGens(gens)) {
      printf("Can't calculate original golay generators\n");
      return 1;
    }
#ifdef DEBUG
  printf("\nGenerators:\n");
  PrintMatrix(gens, 24, 24, true);
  printf("\n");
#endif
    if (SimplifyGolayGens(gens, g_GolayGen) < 0) {
      printf("Can't simplify golay generators\n");
      return 1;
    }

    printf("Golay generators used:\n");
    PrintMatrix(g_GolayGen, 12, 24, true);

    GenGolay();
#endif

    int out[24];

    int matched = 0;
    int d;
    for (int i = 0; i < 4096; i++) {
      memset(out, 0, 96);
      ApplyProjPerm(&g_GolayCode[24 * i], g_alpha, 23, out);
      PrintShortSet(&g_GolayCode[24 * i], 23);
      printf("\n");
      PrintShortSet(out, 23);
      printf("\n");
      d = DistFromGolay(out);
      if (d == 0) {
        matched++;
        printf("permuted set %d in\n", i);
      } else {
        printf("permuted set %d out, min dist %d, ", i, d);
        MissedDistFromGolay(out);
      }
    }
    printf("matched: %d\n", matched);
    return 0;
  } else if (FLAGS_operation == "apply_transform") {
    int a = FLAGS_a;
    int b = FLAGS_b;
    int c = FLAGS_c;
    int d = FLAGS_d;

    int* out = new int[m + 1];
    CalcAllMobius(a, b, c, d, m, out);
    printf("Mobius [%d %d] [%d %d]\n", a, b, c, d);
    for (int j = 0; j < m; j++) {
      if (out[j] == g_infinity) {
        printf("%d --> inf\n", j);
      } else {
        printf("%d --> %d\n", j, out[j]);
      }
    }
    printf("inf --> ");
    PrintNum(out[m]);
    printf("\n");
    delete []out;
    return 0;
  } else {
    printf("Unimplemented option\n");
  return 1;
  }
  return 0;
}

