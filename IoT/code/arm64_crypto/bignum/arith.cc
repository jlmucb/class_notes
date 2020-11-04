//
// Copyright 2014-2019 John Manferdelli, All Rights Reserved.
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
// File: arith.cc

#include <stdio.h>
#include <sys/types.h>
#include <bignum.h>
//#include <string.h>


// asm(code : output operand list : input operand list : clobber list);
//  MRS  x1, NZCV
//  MSR  NZCV, x1          ; copy x1 back into NZCV register to update the condition flags


//#define TEST
#if 1
void InstructionTest(uint64_t a, uint64_t b, uint64_t* c, uint64_t* d) {

  *d = 0ULL;

  // a is top of look
  asm volatile (
    "mov    x9, %[c2]\n\t"    // address of output
    "mov    x10, %[a1]\n\t"   // a
    "mov    x11, 0\n\t"       // holds result
    ".1:\n\t"
    "add    x11, x11, x10\n\t"
    "subs   x10, x10, 1\n\t"
    "bne    .1\n\t"
    "str    x11, [X9]\n\t"
    :: [a1] "r" (a), [a2] "r" (b), [c1] "r" (c), [c2] "r" (d) : );
}
#endif


void Uint64AddStep(uint64_t a, uint64_t b, uint64_t* result, uint64_t* carry) {

  *carry = (*carry) << 29;

  asm volatile (
    "mov    x10, %[a1]\n\t"   // a
    "mov    x11, %[a2]\n\t"   // b
    "mov    x8, %[c1]\n\t"    // &result
    "mov    X9, %[c2]\n\t"    // &carry
    "mrs    x13, NZCV\n\t"
    "mov    x14, 0xffffffff0fffffff\n\t"
    "and    x13, x13, x14\n\t"
    "ldr    x12, [x9]\n\t"
    "orr    x13, x13, x12\n\t"
    "msr    NZCV, x13\n\t"
    "adcs   x12, x10, x11\n\t"
    "mov    x13, 0\n\t"
    "cset   x13, CS\n\t"
    "str    x12, [x8]\n\t"
    "str    x13, [x9]\n\t"
    :: [c1] "r" (result), [c2] "r" (carry), [a1] "r" (a), [a2] "r" (b) : );
}

// r1 is the high order digit, r2 is low order
void Uint64MultStep(uint64_t a, uint64_t b, uint64_t* r1, uint64_t* r2) {
  asm volatile (
    "mov    x10, %[a1]\n\t"   // a
    "mov    x11, %[a2]\n\t"   // b
    "mov    x8, %[r1]\n\t"    // &r1
    "mov    x9, %[r2]\n\t"    // &r2
    "mul    x13, x10, x11\n\t"
    "umulh  x12, x10, x11\n\t"
    "str    x12, [x8]\n\t"
    "str    x13, [x9]\n\t"
    :: [r1] "r" (r1), [r2] "r" (r2), [a1] "r" (a), [a2] "r" (b) : );
}

// 1 means lowest bit is 1
int high_bit_in_unsigned(uint64_t x) {
  uint64_t mask = 1ULL << (num_bits_in_uint64_t - 1);
  for (int i = num_bits_in_uint64_t; i > 0; i--) {
    if ((x & mask) != 0ULL)
      return i;
    mask >>= 1;
  }
  return 0;
}

// Constraint: size_a >= size_b
void u_add(int size_a, int size_b, uint64_t* a, uint64_t* b, int size_c, uint64_t* c) {
  asm volatile (
    // clear carry
    "mrs    x9, NZCV\n\t"
    "mov    x10, 0xffffffff0fffffff\n\t"
    "and    x9, x9, x10\n\t"
    "msr    NZCV, x9\n\t"

    // load parameters
    "mov    x8, 0\n\t"       // indexing
    "mov    x9, %[c]\n\t"    // address of output
    "mov    x10, %[a]\n\t"   // address of input 1
    "mov    x11, %[b]\n\t"   // address of input 2
    "mov    x12, %[sa]\n\t"  // size of first array
    "mov    x13, %[sb]\n\t"  // size of second array

    // do carry addition
    "1:\n\t"

    // load first operand
    "ldr    x15, [x10, x8, lsl 3]\n\t"

    // load second operand or 0 depending on sb
    "mov    x14, 0\n\t"
    "cbz    x13, 2f\n\t"
    "sub    x13, x13, 1\n\t"
    "ldr    x14, [x11, x8, lsl 3]\n\t"

    "2:\n\t"
    "adcs   x16, x15, x14\n\t"
    "str    x16, [x9, x8, lsl 3]\n\t"
    "add    x8, x8, 1\n\t"
    "sub    x12, x12, 1\n\t"
    "cbnz   x12, 1b\n\t"

    // propagate carry
    "mov    x15, 0\n\t"
    "adc    x16, x15, x15\n\t"
    "str    x16, [x9, x8, lsl 3]\n\t"
    :: [sa] "r" (size_a), [a] "r" (a), [sb] "r" (size_b), [b] "r" (b),
       [sc] "r" (size_c), [c] "r" (c): );
}

// Constraint: size_a >= size_b
void u_sub(int size_a, int size_b, uint64_t* a, uint64_t* b, int size_c, uint64_t* c) {

  asm volatile (
    // set carry for sbcs
    "mrs    x9, NZCV\n\t"
    "mov    x10, 0x20000000\n\t"
    "orr    x9, x9, x10\n\t"
    "msr    NZCV, x9\n\t"

    // load parameters
    "mov    x8, 0\n\t"       // indexing
    "mov    x9, %[c]\n\t"    // address of output
    "mov    x10, %[a]\n\t"   // address of input 1
    "mov    x11, %[b]\n\t"   // address of input 2
    "mov    x12, %[sa]\n\t"  // size of first array
    "mov    x13, %[sb]\n\t"  // size of second array

    // do borrow subtract
    "1:\n\t"

    // load first operand
    "ldr    x15, [x10, x8, lsl 3]\n\t"

    // load second operand or 0 depending on sb
    "mov    x14, 0\n\t"
    "cbz    x13, 2f\n\t"
    "sub    x13, x13, 1\n\t"
    "ldr    x14, [x11, x8, lsl 3]\n\t"

    "2:\n\t"
    "sbcs   x16, x15, x14\n\t"
    "str    x16, [x9, x8, lsl 3]\n\t"
    "add    x8, x8, 1\n\t"
    "sub    x12, x12, 1\n\t"
    "cbnz   x12, 1b\n\t"
    :: [sa] "r" (size_a), [a] "r" (a), [sb] "r" (size_b), [b] "r" (b),
       [sc] "r" (size_c), [c] "r" (c): );
}

void u_mult(int size_a, int size_b, uint64_t* a, uint64_t* b, int size_c, uint64_t* c) {
  int size_temp = size_a + size_b + 1;
#ifdef TEST
  if (size_temp > size_c)
    printf("*************u_mult error\n");
#endif
  uint64_t* temp1= new uint64_t[size_temp];
  uint64_t* temp2= new uint64_t[size_temp];
  uint64_t* temp3= new uint64_t[size_temp];
  uint64_t* temp4= new uint64_t[size_temp];
  uint64_t t1 = 0ULL;
  uint64_t s1 = 0ULL;
  uint64_t t2 = 0ULL;
  uint64_t s2 = 0ULL;

  for (int i = 0; i < size_b; i++) {
    ZeroArray(size_temp, temp1);
    ZeroArray(size_temp, temp2);
    s1 = b[i];
    for (int j = 0; j < size_a; j++) {
      s2 = a[j];
      // ?replace by: Uint64MultStep(s1, s2, &t1, &t2);
      asm volatile (
        "mov    x10, %[s1]\n\t"   // first operand
        "mov    x11, %[s2]\n\t"   // second operand
        "mul    x8, x10, x11\n\t"
        "umulh  x9, x10, x11\n\t"
        "mov    %[t1], x8\n\t"    // first output
        "mov    %[t2], x9\n\t"    // second output
      : [t1] "=rm" (t1), [t2] "=rm" (t2)
      : [s1] "r" (s1), [s2] "r" (s2)
      : "x8", "x9", "x10", "x11");
      temp1[i + j] = t1;
      temp2[i + j + 1] = t2;
    }
    ZeroArray(size_temp, temp3);
    ZeroArray(size_temp, temp4);
    u_add(size_temp - 1, size_temp - 1, temp1, temp2, size_temp, temp3);
    CopyArray(size_temp, c, temp4);
    ZeroArray(size_c, c);
    u_add(size_temp - 1, size_temp - 1, temp3, temp4, size_c, c);
  }

  delete []temp1;
  delete []temp2;
  delete []temp3;
  delete []temp4;
}

bool DivideArrayByDigit(int size_a, uint64_t* a, uint64_t b, int size_q,
			uint64_t* q, uint64_t* r) {

  if (b == 0ULL)
    return false;

  uint64_t q1 = 0ULL;
  uint64_t current_product[2];
  uint64_t estimated_product[3];
  uint64_t difference[3];
  uint64_t t_carry, t_digit;

  // carry < b
  t_carry = 0ULL;
  ZeroArray(size_q, q);
#if 0
  printf("\n    a: "); PrintArray(size_a, a); printf("\n");
#endif
  for (int i = (size_a - 1); i >= 0; i--) {

    t_digit = a[i];
    if (t_carry == 0ULL && t_digit < b)
      continue;

#if 0
    printf("    Divide, divisor: %lu, carry: %lu, digit: %lu\n", b, t_carry, t_digit);
#endif
    // estimate quotient
    if (t_carry == 0ULL) {
      q1 = t_digit / b;
    } else {
      estimate_quotient(t_carry, t_digit, b, &q1);
    }

    // quotient is q1 or less
    current_product[1] = t_carry;
    current_product[0] = t_digit;

    for(;;) {
#if 0
      printf("    Estimate: %lu, ", q1);
#endif
      if (q1 == 0ULL)
        break;
      ZeroArray(3, estimated_product);
      Uint64MultStep(q1, b, &estimated_product[1], &estimated_product[0]);
#if 0
      printf("current product: %lu  %lu, ", current_product[1], current_product[0]);
      printf("estimated product: %lu  %lu\n", estimated_product[1], estimated_product[0]);
#endif
      if (CompareArray(SizeArray(3, estimated_product), estimated_product,
                       SizeArray(2, current_product), current_product) <= 0) {
        break;
      }
      q1--;
    }
  q[i] = q1;
  ZeroArray(3, difference);
  u_sub(2, 2, current_product, estimated_product, 3, difference);
#if 0
    printf("    Quotient: %lu, difference: %lu  %lu\n\n", q1, difference[1], difference[0]);
#endif
  t_carry= difference[0];
  }

  *r = t_carry;
  return true;
}

void balence_remainders(uint64_t d, uint64_t* a, uint64_t* b) {
  const uint64_t a0 = 0xffffffffffffffff;
  uint64_t t = a0 - *a;
  if (*b < t)
    t -= *b;
  *a += t;
  *b -= t;
}

uint64_t denominator_normalize_factor(uint64_t d) {
  const uint64_t a0 = 0xffffffffffffffff;
  return a0 / (d + 1);
}

// (x[1]*2^64+x[0])/y, x[1]<y
// 2^63 <= y <= 2^64-1
// In general, this may be an overestimate by 1 or 2
uint64_t div2by1(uint64_t* x, uint64_t y) {
#ifdef TEST
  printf("\ndiv2by1, [%016lx %016lx] / %016lx", x[1], x[0], y);
#endif
  const uint64_t a0 = 0xffffffffffffffff;
  uint64_t current_x[4] = {0ULL, 0ULL, 0ULL, 0ULL};
  uint64_t temp1[4] = {0ULL, 0ULL, 0ULL, 0ULL};
  uint64_t temp2[4] = {0ULL, 0ULL, 0ULL, 0ULL};
  uint64_t a1, a2, a3, a4, r1, r2, r3, r, t;
  uint64_t current_q = 0ULL;

  CopyArray(2, x, current_x);
  for(;;) {
    a1 = a0 / y;
    r1 = a0 - (a1 * y);
#ifdef TEST
    printf("\nLoop, a1: %016lx, r1: %016lx\n", a1, r1);
    printf("current_x: ");
    PrintArray(2, current_x);
    printf("\n");
#endif
    a1 *= current_x[1];
    a2 = current_x[0] / y;
    a3 = current_x[1] / y;
    r2 = current_x[0] - (a2 * y);
    r3 = current_x[1] - (a3 * y);

#ifdef TEST
    printf("a1: %016lx, ", a1);
    printf("a2: %016lx, ", a2);
    printf("a3: %016lx\n", a3);
    printf("r1: %016lx, ", r1);
    printf("r2: %016lx, ", r2);
    printf("r3: %016lx\n", r3);
#endif

    // In general, r1 * x[1] is bigger than 64 bits
    if ((high_bit_in_unsigned(current_x[1]) + high_bit_in_unsigned(r1)) <= num_bits_in_uint64_t) {
      r = r1 * current_x[1];
    } else {
      r = r1;
    }
    balence_remainders(y, &r, &r2);
    balence_remainders(y, &r, &r3);
    a4 = r / y;
    t = a1 + a2 + a3 + a4;
#ifdef TEST
    printf("a1: %016lx, ", a1);
    printf("a2: %016lx, ", a2);
    printf("a3: %016lx\n", a3);
    printf("(1) r: %016lx, a4: %016lx, t: %016lx, current_q: %016lx\n", r, a4, t, current_q);
#endif
    if (t > (a0 - current_q))
      return a0;
    current_q += t;
#ifdef TEST
    printf("(2) r: %016lx, a4: %016lx, t: %016lx, current_q: %016lx\n", r, a4, t, current_q);
#endif
    ZeroArray(4, temp1);
    u_mult(1, 1, &current_q, &y, 4, temp1);
#ifdef TEST
    printf("temp1: \n"); PrintArray(4, temp1); printf("\n");
#endif
    if (CompareArray(SizeArray(4, x), x, SizeArray(4, temp1), temp1) < 0)
      break;
    ZeroArray(4, temp2);
    u_sub(2, 2, x, temp1, 4, temp2);
    ZeroArray(4, current_x);
    CopyArray(4, temp2, current_x);
#ifdef TEST
    printf("temp2: \n"); PrintArray(4, temp2); printf("\n");
#endif
    if (CompareArray(SizeArray(4, current_x), current_x, 1, &y) < 0) {
      break;
    }
  }
  
  return current_q;
}

// estimate is min( |(x[1] * 2^64 + x[0])/y|, 2^64-1)
uint64_t estcalc(uint64_t* x, uint64_t y) {
  if (x[1] >= y)
    return 0xffffffffffffffff;
  return div2by1(x, y);
}


void estimate_quotient(uint64_t n0, uint64_t n1, uint64_t d1, uint64_t* e1) {
#ifdef TEST
  printf("\nestimate_quotient, n0: %016lx, n1: %016lx, d1: %016lx\n", n0, n1, d1);
#endif

  uint64_t y;
  uint64_t x[4] = {0ULL, 0ULL, 0ULL, 0ULL};

  // note n0 <= d1
  if (d1 < (1ULL << 62)) {
    // Normalize
    uint64_t d = denominator_normalize_factor(d1);
#ifdef TEST
    printf("d: %016lx\n", d);
#endif
    uint64_t t2[2];
    t2[0] = n1;
    t2[1] = n0;
#ifdef TEST
  printf("t[1]: %016lx, t[0]: %016lx, d: %016lx\n", t2[1], t2[0], d);
#endif
    u_mult(2, 1, t2, &d, 4, x);
    y = d1 * d;
  } else {
    // No need to normalize
    x[1] = n0;
    x[0] = n1;
    y = d1;
  }
  *e1 = estcalc(x, y);
#ifdef TEST
  printf("x[3]: %016lx, x[2]: %016lx, x[1]: %016lx, x[0]: %016lx, y: %016lx, estimate: %016lx\n", 
       x[3], x[2], x[1], x[0], y, *e1);
#endif
}

