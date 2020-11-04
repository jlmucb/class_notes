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
// File: bignum.cc

#include <stdio.h>
#include <sys/types.h>
#include <bignum.h>
#include <string.h>

void PrintArray(int n, uint64_t* a) {
  for (int i = (n - 1); i >= 0; i--) {
    printf("%016lx ", a[i]);
  }
}

void PrintBig(BigNum& A) {
  printf("[ ");
  if (A.Sign())
    printf("- ");
  PrintArray(A.Size(), A.ValuePtr());
  printf("]");
}

void ZeroArray(int n, uint64_t* p) {
  for (int i = 0; i < n; i++)
    p[i] = 0.0;
}

int SizeArray(int n, uint64_t* p) {
  for (int i = (n - 1); i > 0; i--) {
    if (p[i] > 0)
      return i + 1;
  }
  return 1;
}

bool IsZeroArray(int n, uint64_t* p) {
  for (int i = (n - 1); i > 0; i--) {
    if (p[i] > 0)
      return false;
  }
  return true;
}

int MostSignificantDigit(int n, uint64_t* p) {
  for (int i = (n - 1); i > 0; i--)
    if (p[i] != 0)
      return i;
  return 0;
}

void CopyArray(int n, uint64_t* from, uint64_t* to) {
  for (int i = 0; i < n; i++)
    to[i] = from[i];
}

// returns  1, if l>r
// returns  0, if l==r
// returns -1, if l<r
int CompareArray(int n1, uint64_t* p1, int n2, uint64_t* p2) {
  if (n1 > n2)
    return 1;
  if (n1 < n2)
    return -1;
  for (int i = (n1 - 1); i >= 0; i--) {
    if (p1[i] > p2[i])
      return 1;
    if (p1[i] < p2[i])
      return -1;
  }
  return 0;
}

BigNum::BigNum(int capacity) {
  value_ = new uint64_t[capacity];
  capacity_ = capacity;
  size_ = capacity;
  ZeroArray(capacity, value_);
  sign_ = false;
}

BigNum::BigNum(int n, uint64_t v) {
  value_ = new uint64_t[n];
  capacity_ = n;
  size_ = 1;
  ZeroArray(capacity_, value_);
  sign_ = false;
  value_[0] = v;
}

BigNum::~BigNum() {
  delete []value_;
}

int BigNum::Capacity() {
  return capacity_;
}

int BigNum::Size() {
  return size_;
}

uint64_t* BigNum::ValuePtr() {
  return value_;
}

bool BigNum::IsPositive() {
  if (sign_)
    return false;
  return true;
}

bool BigNum::IsZero() {
  Normalize();
  if (size_ == 1 && value_[0] == 0)
    return true;
  return false;
}

bool BigNum::IsOne() {
  Normalize();
  if (size_ == 1 && value_[0] == 1)
    return true;
  return false;
}

bool BigNum::IsNegative() {
  Normalize();
  return sign_;
}

void BigNum::ToggleSign() {
  sign_ = !sign_;
}

void BigNum::SetSign(bool sign) {
  sign_ = sign;
}

bool BigNum::Sign() {
  return sign_;
}

void BigNum::Normalize() {
  size_ = 1 + MostSignificantDigit(capacity_, value_);
  if (size_ == 1 && value_[0] == 0)
    sign_ = false;
}

void BigNum::ZeroNum() {
  ZeroArray(capacity_, value_);
  size_ = 1;
}

bool BigNum::CopyFrom(BigNum& old_bn) {
  old_bn.Normalize();
  if (capacity_ < old_bn.Size())
    return false;
  ZeroArray(capacity_, value_);
  CopyArray(old_bn.Size(), old_bn.ValuePtr(), value_);
  sign_ = old_bn.Sign();
  Normalize();
  return true;
}

bool BigNum::CopyTo(BigNum& new_bn) {
  Normalize();
  if (new_bn.Capacity() < size_)
    return false;
  ZeroArray(new_bn.Capacity(), new_bn.ValuePtr());
  CopyArray(size_, value_, new_bn.ValuePtr());
  new_bn.SetSign(sign_);
  new_bn.Normalize();
  return true;
}

BigNum::BigNum(BigNum& n) {
  value_ = new uint64_t[n.Capacity()];
  capacity_ = n.Capacity();
  CopyArray(capacity_, n.ValuePtr(), value_);
  sign_ = n.Sign();
  Normalize();
}

BigNum Big_Zero(1, 0ULL);
BigNum Big_One(1, 1ULL);
BigNum Big_Two(1, 2ULL);
BigNum Big_Three(1, 3ULL);

// returns  1, if l>r
// returns  0, if l==r
// returns -1, if l<r
int BigCompare(BigNum& l, BigNum& r) {
  if (!l.Sign() && r.Sign())
    return 1;
  if (l.Sign() && !r.Sign())
    return -1;

  int cmp = CompareArray(l.Size(), l.ValuePtr(), r.Size(), r.ValuePtr());

  if (l.Sign())
    return -cmp;
  return cmp;
}

// returns  1, if |l|>|r|
// returns  0, if |l|==|r|
// returns -1, if |l|<|r|
int BigAbsoluteCompare(BigNum& l, BigNum& r) {
  return CompareArray(l.Size(), l.ValuePtr(), r.Size(), r.ValuePtr());
}

BigNum* BigConvertFromDecimal(const char* in) {
  return nullptr;
}

BigNum* BigConvertFromHex(const char* in) {
  return nullptr;
}

void BigNum::Print() {
  string* s = BigConvertToDecimal();
  if (s == nullptr) {
    printf("***");
    return;
  }

  if (Sign())
    printf("(-%s)", s->c_str());
  else
    printf("%s", s->c_str());
  delete s;
}

string* BigNum::BigConvertToDecimal() {
  BigNum q(Capacity() + 1);
  BigNum t(Capacity() + 1);
  BigNum ten(1, 10ULL);
  BigNum r(Capacity() + 1);
  uint64_t* pr = r.ValuePtr();
  string s;
  string* new_s = new string;

  CopyTo(t);
  for(;;) {
    s.clear();
    q.ZeroNum();
    r.ZeroNum();
    if (!BigDiv(t, ten, q, r))
      return nullptr;
    s += '0' + (char)(*pr);
    *new_s = s + *new_s;
    
    if (q.IsZero())
      break;
    t.ZeroNum();
    t.CopyFrom(q);
  }
  return new_s;
}

string* BigNum::BigConvertToHex() {
  return nullptr;
}

BigNum BigAbsoluteValue(BigNum& bnA) {
  BigNum* bnB = new BigNum(bnA.Capacity());
  bnB->CopyFrom(bnA);
  bnB->SetSign(false);
  bnB->Normalize();
  return *bnB;
}

int LargestPowerTwo(BigNum& a) {
  a.Normalize();
  int m = (a.Size() - 1) * num_bits_in_uint64_t;
  uint64_t* pa = a.ValuePtr();
  m += high_bit_in_unsigned(pa[a.Size() - 1]);
  return m;
}

// Positive shift makes positive number bigger.
bool BigBitShift(int shift, BigNum& bnA, BigNum& bnR) {
  int word_shift;
  int bit_shift;
  bnA.Normalize();
  bnR.ZeroNum();
  uint64_t* pA = bnA.ValuePtr();
  uint64_t* pR = bnR.ValuePtr();

  if (shift >= 0) {
    word_shift = (shift / num_bits_in_uint64_t);
    bit_shift = shift - (word_shift * num_bits_in_uint64_t);
    if ((bnA.Size() + word_shift + (bit_shift == 0)? 0 : 1) >  bnR.Capacity())
      return false;
    for (int i = 0; i < bnA.Size(); i++) {
      if (bit_shift > 0) {
        pR[i + word_shift + 1] |= pA[i] >> (num_bits_in_uint64_t - bit_shift);
      }
      pR[i + word_shift] |=  pA[i] << bit_shift;
    }
  } else {
    shift *= (-1);
    word_shift = (shift / num_bits_in_uint64_t);
    bit_shift = shift - (word_shift * num_bits_in_uint64_t);
    int k = word_shift + (bit_shift == 0) ? 0 : 1;
    if ((bnA.Size() - k ) >  bnR.Capacity())
      return false;
    for (int i = 0; i < bnA.Size(); i++) {
      int m = i - word_shift - 1;
      if (bit_shift > 0 && m >= 0) {
        pR[m] |= pA[i] << (num_bits_in_uint64_t - bit_shift);
      }
      m++;
      if (m >= 0)
        pR[m] |=  pA[i] >> bit_shift;
    }
  }
  bnR.Normalize();
  return true;
}

// Positive shift makes positive number bigger.
bool BigDigitShift(int shift, BigNum& bnA, BigNum& bnR) {
  bnA.Normalize();
  if (bnR.Capacity() < (shift + bnA.Size()))
    return false;
  bnR.ZeroNum();
  uint64_t* pA = bnA.ValuePtr();
  uint64_t* pR = bnR.ValuePtr();
  if (shift >= 0) {
    for (int i = 0; i < bnA.Size(); i++) {
      pR[i + shift] = pA[i];
    }
  } else {
    if (abs(shift) < bnA.Size()) {
      for (int i = (bnA.Size() - 1); i >= 0; i--) {
        int j = i + shift;
        if (j < 0)
          break;
        pR[j] = pA[i];
      }
    }
  }
  bnR.Normalize();
  return true;
}

// A + B = C
bool BigAdd(BigNum& bnA, BigNum& bnB, BigNum& bnC) {
  int size_a, size_b, size_c;
  uint64_t* a;
  uint64_t* b;
  uint64_t* c;

  bnA.Normalize();
  bnB.Normalize();
  size_c = bnC.Capacity();
  c = bnC.ValuePtr();

  if (bnA.Sign() == bnB.Sign()) {
    if (BigAbsoluteCompare(bnA, bnB) >= 0) {
      size_a = bnA.Size();
      size_b = bnB.Size();
      a = bnA.ValuePtr();
      b = bnB.ValuePtr();
    } else {
      size_a = bnB.Size();
      size_b = bnA.Size();
      a = bnB.ValuePtr();
      b = bnA.ValuePtr();
    }
    u_add(size_a, size_b, a, b, size_c,  c);
    bnC.SetSign(bnA.Sign());
  } else if (bnA.Sign()) {
    if (BigAbsoluteCompare(bnA, bnB) >= 0) {
      size_a = bnA.Size();
      size_b = bnB.Size();
      a = bnA.ValuePtr();
      b = bnB.ValuePtr();
      bnC.SetSign(true);
    } else {
      size_a = bnB.Size();
      size_b = bnA.Size();
      a = bnB.ValuePtr();
      b = bnA.ValuePtr();
      bnC.SetSign(false);
    }
    size_c = bnC.Capacity();
    u_sub(size_a, size_b, a, b, size_c,  c);
  } else {
    if (BigAbsoluteCompare(bnA, bnB) >= 0) {
      size_a = bnA.Size();
      size_b = bnB.Size();
      a = bnA.ValuePtr();
      b = bnB.ValuePtr();
      bnC.SetSign(false);
    } else {
      size_a = bnB.Size();
      size_b = bnA.Size();
      a = bnB.ValuePtr();
      b = bnA.ValuePtr();
      bnC.SetSign(true);
    }
    size_c = bnC.Capacity();
    u_sub(size_a, size_b, a, b, size_c,  c);
  }
  bnC.Normalize();
  return true;
}

// A - B = C
bool BigSub(BigNum& bnA, BigNum& bnB, BigNum& bnC) {
  int size_a, size_b, size_c;
  uint64_t* a;
  uint64_t* b;
  uint64_t* c;

  bnA.Normalize();
  bnB.Normalize();
  bnC.ZeroNum();
  size_c = bnC.Capacity();
  c = bnC.ValuePtr();

  if (bnA.Sign() == bnB.Sign()) {
    if (BigAbsoluteCompare(bnA, bnB) >= 0) {
      size_a = bnA.Size();
      size_b = bnB.Size();
      a = bnA.ValuePtr();
      b = bnB.ValuePtr();
    } else {
      size_a = bnB.Size();
      size_b = bnA.Size();
      a = bnB.ValuePtr();
      b = bnA.ValuePtr();
    }
    u_sub(size_a, size_b, a, b, size_c,  c);
    bnC.SetSign(bnA.Sign());
  } else if (bnA.Sign()) {
    if (BigAbsoluteCompare(bnA, bnB) >= 0) {
      size_a = bnA.Size();
      size_b = bnB.Size();
      a = bnA.ValuePtr();
      b = bnB.ValuePtr();
      bnC.SetSign(true);
    } else {
      size_a = bnB.Size();
      size_b = bnA.Size();
      a = bnB.ValuePtr();
      b = bnA.ValuePtr();
      bnC.SetSign(false);
    }
    size_c = bnC.Capacity();
    u_sub(size_a, size_b, a, b, size_c,  c);
  } else {
    if (BigAbsoluteCompare(bnA, bnB) >= 0) {
      size_a = bnA.Size();
      size_b = bnB.Size();
      a = bnA.ValuePtr();
      b = bnB.ValuePtr();
      bnC.SetSign(false);
    } else {
      size_a = bnB.Size();
      size_b = bnA.Size();
      a = bnB.ValuePtr();
      b = bnA.ValuePtr();
      bnC.SetSign(true);
    }
    u_sub(size_a, size_b, a, b, size_c,  c);
  }
  bnC.Normalize();
  return true;
}

// A * B = C
bool BigMult(BigNum& bnA, BigNum& bnB, BigNum& bnC) {
  bnA.Normalize();
  bnB.Normalize();
  bnC.ZeroNum();
  if (bnC.Capacity() <= (bnA.Size() + bnB.Size()))
    return false;
  u_mult(bnA.Size(), bnB.Size(), bnA.ValuePtr(), bnB.ValuePtr(), bnC.Capacity(), bnC.ValuePtr());
  if (bnA.Sign() == bnB.Sign())
    bnC.SetSign(false);
  else
    bnC.SetSign(true);
  bnC.Normalize();
  return true;
}

bool BigMultByDigit(BigNum& bnA, uint64_t x, BigNum& bnR) {
  if (bnR.Capacity() <= bnA.Size())
    return false;
  bnR.ZeroNum();
  bnA.Normalize();
  u_mult(bnA.Size(), 1, bnA.ValuePtr(), &x, bnR.Capacity(), bnR.ValuePtr());
  bnR.Normalize();
  return true;
}

bool DivisionAlgorithm(BigNum& bnA, BigNum& bnB, BigNum& bnQ, BigNum& bnR) {
  int shift = bnA.Size() - bnB.Size();

  // Division Algorithm
  //
  // At = A
  // i = shift
  // loop
  //    if i < 0 break
  //    Bt = B << i
  //    if Bt > At
  //      i--
  //    estimate quotient digit est
  //    do forever
  //        Ct = At - B * (est << i)
  //        if (C' < B')
  //          break
  //        est++
  //    A' = C'
  //    Q[i] = est
  //    i--
  //    goto loop
  //    R = A'

  BigNum At(bnA.Capacity() + 1);
  BigNum Bt(bnA.Capacity() + 1);
  BigNum Ct(bnA.Capacity() + 1);
  BigNum Dt(bnA.Capacity() + 1);
  uint64_t est_q_digit = 0ULL;
  uint64_t n0;
  uint64_t n1;
  uint64_t* pQ = bnQ.ValuePtr();
  uint64_t* pB = bnB.ValuePtr();
  uint64_t d1;

  // Loop through possible quotient digit positions
  At.CopyFrom(bnA);
  for (int i = shift; i >= 0; i--) {
    Bt.ZeroNum();
    if (!BigDigitShift(i, bnB, Bt))
      return false;
#ifdef TEST
    printf("\n\ni: %d, At: ", i); PrintBig(At); printf("\n"); printf("      Bt: "); PrintBig(Bt); 
      printf(", bnB size: %d, At size: %d\n", bnB.Size(), At.Size());
#endif
    if (BigCompare(Bt, At) > 0)
      continue;
    // Now At >= Bt
    int k1 = bnB.Size() - 1;
    d1 = pB[k1];
    uint64_t* pAt = At.ValuePtr();
    int k2 = At.Size() - 1;
    if (At.Size() > Bt.Size()) {
      n0 = pAt[k2];
      if (k2 > 0)
        n1 = pAt[k2 - 1];
      else
        n1 = 0ULL;
    } else {
      n0 = 0ULL;
      n1 = pAt[k2];
    }

    estimate_quotient(n0, n1, d1, &est_q_digit);
#ifdef TEST
    printf("Estimate quotient, shift %d, return:  %lx\n", i, est_q_digit);
#endif

    // This should not loop much
    for(;;) {
#ifdef TEST
      printf("\nLoop: %016lx\n", est_q_digit);
#endif
      // This shouldn't happen
      if (est_q_digit == 0ULL) {
        printf("Estimated quotient digit is zero, shouldn't happen\n");
        return false;
      }
      Dt.ZeroNum();
      if (!BigMultByDigit(Bt, est_q_digit, Dt))
        return false;
#ifdef TEST
      printf("\nBt * %lx, Dt: ", est_q_digit); PrintBig(Dt); printf("\n");
#endif
      if (BigCompare(At, Dt) < 0) {
        est_q_digit--;
        continue;
      }
      Ct.ZeroNum();
      if(!BigSub(At, Dt, Ct))
        return false;
      if (BigCompare(Ct, Bt) < 0) {
        break;
      } else {
        // This shouldn't happen
        printf("Number minus estimated digit times shifted divisor is bigger than shifted divisor, shouldn't happen\n");
        return false;
      }
    }
    pQ[i] = est_q_digit;  
    At.ZeroNum();
    At.CopyFrom(Ct);
    At.Normalize();
  }
  bnQ.Normalize();
  bnR.CopyFrom(At);
  return true;
}

// A / B = Q, remainder R
bool BigDiv(BigNum& bnA, BigNum& bnB, BigNum& bnQ, BigNum& bnR) {
  bool ret = true;

  bnB.Normalize();
  if (bnB.IsZero())
    return false;
  bnA.Normalize();
  bnQ.ZeroNum();
  bnR.ZeroNum();
  bool signA = bnA.Sign();
  bool signB= bnB.Sign();
  bnA.SetSign(false);
  bnB.SetSign(false);

  if (BigAbsoluteCompare(bnA, bnB) < 0) {
    bnR.CopyFrom(bnA);
  } else {
    ret = DivisionAlgorithm(bnA, bnB, bnQ, bnR);
  }

  bnA.SetSign(signA);
  bnB.SetSign(signB);
  if (!ret) {
    return false;
  }

  if (bnA.Sign() == bnB.Sign()) {
    bnQ.SetSign(false);
    bnR.SetSign(false);
  } else {
    bnQ.SetSign(true);
    bnR.SetSign(true);
  }
  bnQ.Normalize();
  bnR.Normalize();
  return ret;
}

