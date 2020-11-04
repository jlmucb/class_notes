//
// Copyright 2014-2019, John Manferdelli, All Rights Reserved.
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
// Project: Arm64 Crypto
// File: arm64_test.cc

#include <stdio.h>
#include <sys/types.h>
#include <bignum.h>

void print_compare_val(int k) {
  switch(k) {
  case 1:
    printf(" > ");
    break;
  case 0:
    printf(" = ");
    break;
  case -1:
    printf(" < ");
    break;
  }
}

int main(int an, char** av) {

#if 0
  extern void InstructionTest(uint64_t a, uint64_t b, uint64_t* c, uint64_t* d);
  uint64_t a, b, c, d;

  a = 10;
  b = 0;
  c = 0;
  d = 0;

  InstructionTest(a, b, &c, &d);
  printf("%lu --> %lu\n", a, d);
#else
  uint64_t a, b, carry_in, carry, result;
  carry_in = 1;
  carry = carry_in;
  result = 0;
  a = 0xffffffffffffffffULL;
  b = 2;
  Uint64AddStep(a, b, &result, &carry);
  printf("\nUint64AddStep, ");
  printf("%lx + %lx  + %lx = %lx, carry_out: %lx\n", a, b, carry_in, result, carry);

  uint64_t r1, r2;
  a = 0xffffffffffffffffULL;
  b = 0x100;
  r1 = 0;
  r2 = 0;
  printf("\nUint64MultStep, ");
  Uint64MultStep(a, b, &r1, &r2);
  printf("%lx *  %lx =  %016lx %016lx\n", a, b, r1, r2);

  // Compare tests

  BigNum A(10, 0xfffffffffffffff8ULL);
  BigNum B(10, 0x100ULL);
  BigNum C(10);
  C.ZeroNum();

  printf("\nBigCompare\n");
  int k = BigCompare(A, B);
  PrintBig(A);
  print_compare_val(k);
  PrintBig(B);
  printf("\n");
  k = BigCompare(A, A);
  PrintBig(A);
  print_compare_val(k);
  PrintBig(A);
  printf("\n");
  k = BigCompare(B, A);
  PrintBig(B);
  print_compare_val(k);
  PrintBig(A);
  printf("\n");
  

  printf("\nBignum add\n");
  PrintBig(A);
  printf(" + ");
  PrintBig(B);
  printf(" = ");
  if (BigAdd(A, B, C)) {
    PrintBig(C);
    printf("\n");
  } else {
    printf(" --- ADD failed\n");
  }

  printf("\nBignum sub\n");
  PrintBig(A);
  printf(" - ");
  PrintBig(B);
  printf(" = ");
  if (BigSub(A, B, C)) {
    PrintBig(C);
    printf("\n");
  } else {
    printf(" --- Sub failed\n");
  }

  printf("\nBignum mult\n");
  PrintBig(A);
  printf(" * ");
  PrintBig(B);
  printf(" = ");
  if (BigMult(A, B, C)) {
    PrintBig(C);
    printf("\n");
  } else {
    printf(" --- Mult failed\n");
  }

#if 1
  printf("\nBignum div\n");
  BigNum A1(10, 0xfffffffffffffff8ULL);
  uint64_t* a1p = A1.ValuePtr();
  a1p[1]=  0xffff;
  A1.Normalize();
  BigNum B1(10, 0x100ULL);
  BigNum Q(10);
  BigNum R(10);

  PrintBig(A1);
  printf(" / ");
  PrintBig(B1);
  printf(" = ");
  if (BigDiv(A1, B1, Q, R)) {
    PrintBig(Q);
    printf(" rem ");
    PrintBig(R);
    printf("\n");
  } else {
    printf(" --- Div failed\n");
  }
#endif

  A.ZeroNum();
  uint64_t* ap = A.ValuePtr();
  ap[0] = 0xffff000011110000ULL;
  ap[1] = 0xcccc000055550000ULL;
  ap[2] = 0x0000bbbb00006666ULL;
  A.Normalize();

  printf("\nBignum shift\n");
  if (BigDigitShift(2, A, C)) {
    PrintBig(A);
    printf(" << 2w = ");
    PrintBig(C);
    printf("\n");
  } else {
    printf("BigDigitShift error\n");
  }
  if (BigDigitShift(-2, A, C)) {
    PrintBig(A);
    printf(" >> 2w = ");
    PrintBig(C);
    printf("\n");
  } else {
    printf("BigDigitShift error\n");
  }
  if (BigBitShift(6, A, C)) {
    PrintBig(A);
    printf(" << 6b = ");
    PrintBig(C);
    printf("\n");
  } else {
    printf("BigBitShift error\n");
  }
  if (BigBitShift(-6, A, C)) {
    PrintBig(A);
    printf(" >> 6b = ");
    PrintBig(C);
    printf("\n");
  } else {
    printf("BigBitShift error\n");
  }
  if (BigBitShift(67, A, C)) {
    PrintBig(A);
    printf(" << 67b = ");
    PrintBig(C);
    printf("\n");
  } else {
    printf("BigBitShift error\n");
  }
  if (BigBitShift(-67, A, C)) {
    PrintBig(A);
    printf(" >> 67b = ");
    PrintBig(C);
    printf("\n");
  } else {
    printf("BigBitShift error\n");
  }

  BigNum a1(5, 46ULL);
  BigNum b1(5, 7ULL);
  BigNum e(5, 40ULL);
  BigNum m(5, 41ULL);
  BigNum q(5);
  BigNum r(5);
  BigNum x(5);
  BigNum y(5);
  BigNum g(5);

  printf("\nMod tests:\n");
  if (BigMod(a1, m, r))  {
    a1.Print();
    printf(" BigMod ");
    m.Print();
    printf(" is ");
    r.Print();
    printf("\n");
  } else {
    printf("BigMod error\n");
  } 

  r.ZeroNum();
  if (BigModAdd(a1, b1, m, r)) {
    a1.Print();
    printf(" + ");
    b1.Print();
    printf(" (mod ");
    m.Print();
    printf(") is ");
    r.Print();
    printf("\n");
  } else {
    printf("BigModAdd error\n");
  } 

  r.ZeroNum();
  if (BigModSub(a1, b1, m, r)) {
    a1.Print();
    printf(" - ");
    b1.Print();
    printf(" (mod ");
    m.Print();
    printf(") is ");
    r.Print();
    printf("\n");
  } else {
    printf("BigModSub error\n");
  } 

  r.ZeroNum();
  if (BigSquare(a1, r)) {
    a1.Print();
    printf(" * ");
    a1.Print();
    printf(" = ");
    r.Print();
    printf("\n");
  } else {
    printf("BigSquare error\n");
  } 

  r.ZeroNum();
  if (BigMod(a1, m, r)) {
    a1.Print();
    printf(" (mod  ");
    m.Print();
    printf(") = ");
    r.Print();
    printf("\n");
  } else {
    printf("BigMod error\n");
  } 

  r.ZeroNum();
  if (BigModNeg(a1, m, r)) {
    printf(" - ");
    a1.Print();
    printf(" = ");
    r.Print();
    printf("\n");
  } else {
    printf("BigModNeg error\n");
  } 

  r.ZeroNum();
  if (BigModMult(a1, b1, m, r)) {
    a1.Print();
    printf(" * ");
    b1.Print();
    printf(" mod( ");
    m.Print();
    printf(") = ");
    r.Print();
    printf("\n");
  } else {
    printf("BigModMult error\n");
  } 

  r.ZeroNum();
  if (BigModSquare(a1, m, r)) {
    a1.Print();
    printf(" * ");
    a1.Print();
    printf(" mod( ");
    m.Print();
    printf(") = ");
    r.Print();
    printf("\n");
  } else {
    printf("BigModSquare error\n");
  } 

  q.ZeroNum();
  r.ZeroNum();
  if (BigUnsignedEuclid(a1, b1, q, r)) {
    a1.Print();
    printf(" = ");
    b1.Print();
    printf(" * ");
    q.Print();
    printf(" + ");
    r.Print();
    printf("\n");
  } else {
    printf("BigUnsignedEuclid error\n");
  }

  g.ZeroNum();
  x.ZeroNum();
  y.ZeroNum();
  if (BigExtendedGCD(a1, b1, x, y, g)) {
    a1.Print();
    printf(" * ");
    x.Print();
    printf(" + ");
    b1.Print();
    printf(" * ");
    y.Print();
    printf(" = ");
    g.Print();
    printf("\n");
  } else {
    printf("BigExtendedGCD error\n");
  } 

  r.ZeroNum();
  if (BigModInv(a1, m, r)) {
    printf("(");
    a1.Print();
    printf(")**-1 (mod ");
    m.Print();
    printf(") = ");
    r.Print();
    printf("\n");
  } else {
    printf("BigModInv error\n");
  } 

  r.ZeroNum();
  if (BigModDiv(a1, b1, m, r)) {
    a1.Print();
    printf(" / ");
    b1.Print();
    printf(" (mod ");
    m.Print();
    printf(") = ");
    r.Print();
    printf("\n");
  } else {
    printf("BigModDiv error\n");
  } 

  r.ZeroNum();
  if (BigModExp(b1, e, m, r)) {
    b1.Print();
    printf(" ** ");
    e.Print();
    printf(" (mod ");
    m.Print();
    printf(") = ");
    r.Print();
    printf("\n");
  } else {
    printf("BigModExp error\n");
  } 

  BigNum m1(5, 37ULL);
  BigNum m2(5, 41ULL);
  BigNum s1(5, 7ULL);
  BigNum s2(5, 1ULL);
  r.ZeroNum();
  if(BigCRT(s1, s2, m1, m2, r)) {
    s1.Print();
    printf(" (mod ");
    m1.Print();
    printf("),  ");
    s2.Print();
    printf(" (mod ");
    m2.Print();
    printf("), common soln is ");
    r.Print();
    printf("\n");
  } else {
    printf("BigCRT error\n");
  } 

  // Divide by digit test
  m1.ZeroNum();
  uint64_t* pm = m1.ValuePtr();
  uint64_t d = 0ULL;
  b = 10ULL;
  pm[0] = 4;
  pm[1] = 6000;
  pm[2] = 200;
  pm[3] = 2929;
  m1.Normalize();

  printf("\nDivideArrayByDigit test\n");
  m2.ZeroNum();
  if (DivideArrayByDigit(m1.Size(), m1.ValuePtr(), b, m2.Size(), m2.ValuePtr(), &d)) {
    m1.Print();
    printf(" / %ld remainder %ld, quotient is ", b, d);
    m2.Normalize();
    m2.Print();
    printf("\n");
  } else {
    printf("DivideArrayByDigit error\n");
  }

#endif

  printf("done\n");
  return 0;
}

