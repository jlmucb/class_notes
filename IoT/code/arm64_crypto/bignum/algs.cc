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
// bignum.h

#include <stdio.h>
#include <sys/types.h>
#include <bignum.h>
#include <util.h>
#include <string.h>


// r < q, a = bq + r
bool BigUnsignedEuclid(BigNum& a, BigNum& b, BigNum& q, BigNum& r) {
  if (b.IsZero())
    return false;
  if (!a.Sign() && !b.Sign()) {
    if (!BigDiv(a, b, q, r))
      return false;
    return true;
  }
  return false;
}

bool BigSquare(BigNum& a, BigNum& r) {
  return BigMult(a, a, r);
}

bool BigMod(BigNum& a, BigNum& m, BigNum& r) {
  if (m.IsZero())
    return false;
  BigNum q(a.Capacity() + 1);
  BigNum s(a.Capacity() + 1);
  BigNum n(a.Capacity() + 1);
  BigNum a_t(a.Capacity() + 1);

  if (!a.Sign() && !m.Sign()) {
    return BigDiv(a, m, q, r);
  } else if (a.Sign() && !m.Sign()) {
    a_t.CopyFrom(a);
    a_t.SetSign(false);
    if (!BigDiv(a_t, m, q, s))
      return false;
    if (s.IsZero()) {
      r.ZeroNum();
      return true;
    }
    if (!BigAdd(q, Big_One, n))
      return false;
    s.ZeroNum();
    if (!BigMult(n, m, s))
      return false;
    return BigAdd(a, s, r);
  } else {
    return false;
  }
}

bool BigModAdd(BigNum& a, BigNum& b, BigNum& m, BigNum& r) {
  int k = a.Size() > b.Size() ? a.Size() : b.Size();
  BigNum c(k + 1);

  if (!BigAdd(a, b, c))
    return false;
  return BigMod(c, m, r);
}

bool BigModSub(BigNum& a, BigNum& b, BigNum& m, BigNum& r) {
  int k = a.Size() > b.Size() ? a.Size() : b.Size();
  BigNum c(k + 1);

  if (!BigSub(a, b, c))
    return false;
  return BigMod(c, m, r);
}

bool BigModNeg(BigNum& a, BigNum& m, BigNum& r) {
  BigNum c(a.Size() + 1);

  c.CopyFrom(a);
  c.SetSign(!a.Sign());
  return BigMod(c, m, r);
}

bool BigModMult(BigNum& a, BigNum& b, BigNum& m, BigNum& r) {
  BigNum c(a.Size() + b.Size() + 2);

  if (!BigMult(a, b, c))
    return false;
  return BigMod(c, m, r);
}

bool BigModSquare(BigNum& a, BigNum& m, BigNum& r) {
  return BigModMult(a, a, m, r);
}

// recursion is
//  r[n+1] = r[n-1] + (-q[n+1])r[n]
//  r[-1] = a(1) + b(0)
//  r[0]  = a(0) + b(1)
bool euclid_update(BigNum& q, BigNum& r, BigNum& a1, BigNum& b1, BigNum& r1,
                                            BigNum& a2, BigNum& b2, BigNum& r2,
                   BigNum& a3, BigNum& b3) {
  q.ZeroNum();
  r.ZeroNum();
  a3.ZeroNum();
  b3.ZeroNum();
  BigNum t(a1.Capacity());
  if (!BigUnsignedEuclid(r1, r2, q, r))
    return false;
  if (r.IsZero())
    return true;
  q.SetSign(!q.Sign());
  q.Normalize();
  t.ZeroNum();
  if (!BigMult(q, a2, t))
    return false;
  if (!BigAdd(a1, t, a3))
    return false;
  t.ZeroNum();
  if (!BigMult(q, b2, t))
    return false;
  if (!BigAdd(b1, t, b3))
    return false;
  a1.ZeroNum();
  b1.ZeroNum();
  r1.ZeroNum();
  a1.CopyFrom(a2);
  b1.CopyFrom(b2);
  r1.CopyFrom(r2);
  a2.ZeroNum();
  b2.ZeroNum();
  r2.ZeroNum();
  r2.CopyFrom(r);
  a2.CopyFrom(a3);
  b2.CopyFrom(b3);
  return true;
}

// ax + by = g
bool BigExtendedGCD(BigNum& a, BigNum& b, BigNum& x, BigNum& y, BigNum& g) {
  int k = a.Size() > b.Size() ? a.Size() : b.Size();
  BigNum a1(2*k + 1);
  BigNum b1(2*k + 1);
  BigNum r1(2*k + 1);
  BigNum a2(2*k + 1);
  BigNum b2(2*k + 1);
  BigNum r2(2*k + 1);
  BigNum a3(2*k + 1);
  BigNum b3(2*k + 1);
  BigNum r3(2*k + 1);
  BigNum q(2*k + 1);

  a1.CopyFrom(Big_One);
  b1.ZeroNum();
  r1.CopyFrom(a);
  a2.ZeroNum();
  b2.CopyFrom(Big_One);
  r2.CopyFrom(b);
  for (;;) {
    if (!euclid_update(q, r3, a1, b1, r1, a2, b2, r2, a3, b3))
      return false;
    if (r3.IsZero()) {
      // terminate
      x.CopyFrom(a2);
      y.CopyFrom(b2);
      g.CopyFrom(r2);
      return true;
    }
  }
}

bool BigModInv(BigNum& a, BigNum& m, BigNum& r) {
  if (a.IsZero())
    return false;
  int k = a.Size() > m.Size() ? a.Size() : m.Size();
  BigNum a_t(k + 1);
  BigNum m_t(k + 1);
  BigNum t(k + 1);
  if (!BigExtendedGCD(a, m, a_t, m_t, t))
    return false;
  if (BigCompare(Big_One, t) != 0)
    return false;
  return BigMod(a_t, m, r);
}

bool BigModDiv(BigNum& a, BigNum& b, BigNum& m, BigNum& r) {
  int k = a.Size() > b.Size() ? a.Size() : b.Size();
  BigNum t(k + 1);
  if (!BigModInv(b, m, t))
    return false;
  return BigModMult(a, t, m, r);
}

bool BigModExp(BigNum& b, BigNum& e, BigNum& m, BigNum& r) {
  int k = m.Size() > b.Size() ? m.Size() : b.Size();
  BigNum t(2*k + 1);
  BigNum s(2*k + 1);
  BigNum s_t(2*k + 1);
  BigNum accum(2*k + 1, 1ULL);
  int n = LargestPowerTwo(e);
  s.CopyFrom(b);
  uint64_t* pe = e.ValuePtr();
  int word_shift, bit_shift;
  uint64_t mask;

  for (int i = 0; i < n; i++) {
    word_shift = (i >> 6);
    bit_shift = i & 0x3f;
    mask = 1ULL << bit_shift;
    if ((pe[word_shift] & mask) != 0) {
      t.ZeroNum();
      if (!BigModMult(s, accum, m, t))
        return false;
      accum.CopyFrom(t);
    }
    s_t.ZeroNum();
    if (!BigModMult(s, s, m, s_t))
      return false;
    s.ZeroNum();
    s.CopyFrom(s_t);
  }
  r.CopyFrom(accum);
  return true;
}

bool BigCRT(BigNum& s1, BigNum& s2, BigNum& m1, BigNum& m2, BigNum& r) {
  int m = m1.size_ > m2.size_ ? m1.size_ : m2.size_;
  if (s1.size_ > m)
    m = s1.size_;
  if (s2.size_ > m)
    m = s2.size_;
  BigNum u1(3 * m + 1);
  BigNum u2(3 * m + 1);
  BigNum t1(3 * m + 1);
  BigNum t2(3 * m + 1);
  BigNum r1(3 * m + 1);
  BigNum r2(3 * m + 1);
  BigNum n(3 * m + 1);
  BigNum m1m2(3 * m + 1);

  // u1 m1 + u2 m2= 1
  if (!BigExtendedGCD(m1, m2, u1, u2, t1))
    return false;

  // r= u1 m1 s2 + u2 m2 s1
  if (!BigMult(u1, m1, t1))
    return false;
  if (!BigMult(t1, s2, r1))
    return false;
  if (!BigMult(u2, m2, t2))
    return false;
  if (!BigMult(t2, s1, r2))
    return false;
  if (!BigMult(m1, m2, m1m2))
    return false;
  if (!BigAdd(r1, r2, n))
    return false;
  return BigMod(n, m1m2, r);
}

int BigMaxPowerOfTwoDividing(BigNum& n) {
  uint64_t* pN = n.ValuePtr();

  for (int i = 0; i < n.Size(); i++) {
    uint64_t mask = 1ULL;
    uint64_t x = pN[i];
    for (int j = 0; j < num_bits_in_uint64_t; j++) {
      if ((x & mask) != 0) {
        return ((num_bits_in_uint64_t * i) + j + 1);
      }
      mask <<= 1;
    }
  }
  return -1;
}

bool BigMillerRabin(BigNum& n, BigNum** random_a, int trys) {
  BigNum n_minus_1(2 * n.size_);
  BigNum odd_part_n_minus_1(2 * n.size_);
  BigNum y(4 * n.size_ + 1);
  BigNum z(4 * n.size_ + 1);
  int i;
  int j;
  int shift;

  if (!BigSub(n, Big_One, n_minus_1))
    return false;
  shift = BigMaxPowerOfTwoDividing(n_minus_1);
  if (shift > 0) {
    if (!BigBitShift(-shift, n_minus_1, odd_part_n_minus_1))
      return false;
  }
  for (i = 0; i < trys; i++) {
    y.ZeroNum();
    if (!BigModExp(*random_a[i], odd_part_n_minus_1, n, y))
      return false;
    if (BigCompare(y, Big_One) == 0 || BigCompare(y, n_minus_1) == 0)
      continue;
    for (j = 0; j < shift; j++) {
      z.ZeroNum();
      if (!BigModMult(y, y, n, z))
        return false;
      if (BigCompare(z, Big_One) == 0)
        return false;
    }
    y.CopyFrom(z);
    if (BigCompare(y, n_minus_1) == 0)
      break;
  }
  if (BigCompare(y, n_minus_1) != 0)
    return false;
  return true;
}

bool add_to_me(BigNum& me, BigNum& adder) {
  BigNum t(me.Capacity());

  if (!BigAdd(me, adder, t))
    return false;
  me.CopyTo(t);
  return true;
}

const int max_prime_tries = 25000;
bool BigGenPrime(BigNum& p, uint64_t num_bits) {
  int i, j;

  for (i = 0; i < max_prime_tries; i++) {
    p.ZeroNum();
    if (!GetCryptoRand(num_bits, (byte*)p.ValuePtr())) {
      // LOG(ERROR) << "GetCryptoRand in BigGenPrime fails\n";
      return false;
    }
    p.Normalize();
    p.value_[p.Size() - 1] |= (1ULL) << 63;
    p.value_[0] |= 1ULL;
    p.Normalize();
    for (j = 0; j < 250; j++, i++) {
      if (BigIsPrime(p)) {
        return true;
      }

      if (!add_to_me(p, Big_Two)) {
        // LOG(ERROR) << "BigUnsignedAddTo failed in BigGenPrime\n";
        return false;
      }
    }
  }
  return false;
}

// FIX
bool rands_avail = false;
BigNum* random_nums[40];

bool FillRandom(int n, BigNum** random_array) {
  int i;

  if (n > 20)
    return false;

  if (!rands_avail) {
    random_nums[0] = new BigNum(5, 2ULL);
    for (i = 0; i < 19; i++) {
      random_nums[i + 1] = new BigNum(5, 2ULL + (uint64_t)(19 * i));
      random_nums[i + 1]->Normalize();
    }
    rands_avail = true;
  }
  for (i = 0; i < 20; i++) {
    random_array[i] = random_nums[i];
  }
  return true;
}

bool BigIsPrime(BigNum& n) {
  extern uint64_t smallest_primes[];
  extern int num_smallest_primes;
  int i;
  uint64_t q[n.Size()];
  uint64_t r;
  BigNum** random_a = new BigNum* [20];

  for (i = 0; i < num_smallest_primes; i++) {
    ZeroArray(n.Size(), q);
    if (n.Size() == 1 && smallest_primes[i] >= n.value_[0])
      return true;
    if (!DivideArrayByDigit(n.size_, n.value_, (uint64_t)smallest_primes[i], n.size_, q, &r)) {
      //LOG(ERROR) << "DivideArrayByDigit failed in BigIsPrime\n";
      return false;
    }
    if (r == 0ULL)
      return false;
  }
  if (!FillRandom(20, random_a)) {
    //LOG(ERROR) << "Couldnt FillRandom in BigIsPrime\n";
    return false;
  }
  return BigMillerRabin(n, random_a);
  return false;
}

// Is n^(p-1) == 1?
bool BigModIsSquare(BigNum& n, BigNum& p) {
  BigNum p_minus_1(n.Size());
  BigNum e(n.Size());
  int m = (n.Capacity() > p.Capacity()) ? n.Capacity() : p.Capacity();
  BigNum residue(4 * m + 1);
  uint64_t unused;
  int size_e;

  BigSub(p, Big_One, p_minus_1);
  size_e = p_minus_1.Size();
  if (!DivideArrayByDigit(p_minus_1.size_, p_minus_1.ValuePtr(),
                             2ULL, size_e, e.value_, &unused)) {
    // LOG(ERROR) << "DivideArrayByDigit failed in BigModIsSquare\n";
    return false;
  }
  e.Normalize();
  size_e = e.Size();
  if (!BigModExp(n, e, p, residue)) {
    LOG(ERROR) << "BigModExp failed in BigModIsSquare\n";
    return false;
  }
  if (!residue.IsOne())
    return false;
  return false;
}

//  a>0, p!=2
//  Find x: x^2=a (mod p).  Caller should have checked that a is a QR
//    Reference: Cohen, A Course of computational number theory, p32.
//  p-1= 2^e q
//  Pick a quadratic non-residue n
//  z= n^q  (z is a generator)
//  y= z; r= e; x= a^((q-1)/2) (mod p), b= ax^2
//  x= ax  (RHS is a^2x^2= ab (mod p))
//  while(b!=1) {
//    -- at this point ab= x^2, y^(2^(r-1))= -1 (mod p), b^(2^(r-1)) =1
//    find smallest m: b^(2^m)= 1 (mod p) --- note m<r
//    t=y^(2^(r-m-1)) (mod p)
//    y= t^2
//    r= m; x=xt; b=by;
//
// find smallest m: b^(2^m)= 1 (mod p) --- note m<r
int smallestunitaryexponent(BigNum& b, BigNum& p, int maxm) {
  BigNum e(2 * p.capacity_ + 1);
  BigNum t(2 * p.capacity_ + 1);
  int i;

  for (i = 1; i < maxm; i++) {
    if (!BigBitShift(i, Big_One, e))
      return -1;
    if (!BigModExp(b, e, p, t))
      return -1;
    if (BigCompare(Big_One, t) == 0) {
      break;
    }
    e.ZeroNum();
    t.ZeroNum();
  }

  return i;
}

bool BigModTonelliShanks(BigNum& a, BigNum& p, BigNum& s) {
  BigNum t1(2 * p.size_ + 1);
  BigNum t2(2 * p.size_ + 1);

  BigNum q(2 * p.size_ + 1);  // p-1= 2^max_two_power q
  BigNum p_minus(2 * p.size_ + 1);

  int max_two_power;
  int m;

  BigNum e(2 * p.size_ + 1);  // exponent
  BigNum n(2 * p.size_ + 1);  // non-residue
  BigNum x(2 * p.size_ + 1);
  BigNum y(2 * p.size_ + 1);
  BigNum z(2 * p.size_ + 1);
  BigNum b(2 * p.size_ + 1);
  BigNum t(2 * p.size_ + 1);

  if (!BigSub(p, Big_One, p_minus)) {
    LOG(ERROR) << "BigUnsignedSub 1 in BigModTonelliShanks failed\n";
    return false;
  }
  max_two_power = BigMaxPowerOfTwoDividing(p_minus);
  if (!BigBitShift(-max_two_power, p_minus, q)) {
    //LOG(ERROR) << "BigBitShift 1 in BigModTonelliShanks failed\n";
    return false;
  }
  n.value_[0] = 2ULL;
  while (!BigModIsSquare(n, p)) {
    if (!add_to_me(n, Big_One)) {
      //LOG(ERROR) << "BigUnsignedAddTo in BigModTonelliShanks failed\n";
      return false;
    }
  }
  if (!BigModExp(n, q, p, z)) {
    //LOG(ERROR) << "BigModExp 1 in BigModTonelliShanks failed\n";
    return false;
  }
  if (!z.CopyTo(y)) {
    return false;
  }
  if (!BigSub(q, Big_One, t1)) {
    //LOG(ERROR) << "BigUnsignedSub 2 in BigModTonelliShanks failed\n";
    return false;
  }
  if (!BigBitShift(-1, t1, t2)) {
    //LOG(ERROR) << "BigBitShift 2 in BigModTonelliShanks failed\n";
    return false;
  }
  if (!BigModExp(a, t2, p, x)) {
    //LOG(ERROR) << "BigModExp 1 in BigModTonelliShanks failed\n";
    return false;
  }
  t1.ZeroNum();
  t2.ZeroNum();
  if (!BigModMult(x, x, p, t1)) {
    //LOG(ERROR) << "BigModMult 1 in BigModTonelliShanks failed\n";
    return false;
  }
  if (!BigModMult(t1, a, p, b)) {
    //LOG(ERROR) << "BigModMult 2 in BigModTonelliShanks failed\n";
    return false;
  }
  t1.ZeroNum();
  if (!BigModMult(x, a, p, t1)) {
    //LOG(ERROR) << "BigModMult 3 in BigModTonelliShanks failed\n";
    return false;
  }
  t1.CopyTo(x);

  for (;;) {
    if (BigCompare(Big_One, b) == 0)
      break;
    // at this point ab= x^2, y^(2^(r-1))= -1 (mod p), b^(2^(r-1)) =1

    // find smallest m: b^(2^m)= 1 (mod p) --- note m<r
    m = smallestunitaryexponent(b, p, max_two_power);

    // t=y^(2^(r-m-1)) (mod p)
    if (!BigBitShift(max_two_power - m - 1, Big_One, e)) {
      //LOG(ERROR) << "BigBitShift 3 in BigModTonelliShanks failed\n";
      return false;
    }
    if (!BigModExp(y, t2, p, t)) {
      //LOG(ERROR) << "BigModMult 3 in BigModTonelliShanks failed\n";
      return false;
    }
    y.ZeroNum();

    // y= t^2
    if (!BigModMult(t, t, p, y)) {
      return false;
    }
    // r= m; x=xt; b=by;
    max_two_power = m;
    t1.ZeroNum();
    if (!BigModMult(x, t, p, t1)) {
      //LOG(ERROR) << "BigModMult 4 in BigModTonelliShanks failed\n";
      return false;
    }
    t1.CopyTo(x);
    t1.ZeroNum();
    if (!BigModMult(y, b, p, t1)) {
      //LOG(ERROR) << "BigModMult 5 in BigModTonelliShanks failed\n";
      return false;
    }
    t1.CopyTo(y);
  }
  return true;
}

BigNum Big_Five(1, 5ULL);

//  a>0, p!=2
//  Find x: x^2=n (mod p)
//  if(p==3 (mod 4)) x= a^((p+1)/4 (mod p)
//  if(p==5 (mod 8)) put b= n^((p-1)/4) (mod p)
//    if(b==1) x= a^((p+3)/8) (mod p)
//    otherwise, x= (2a)(4a)^((p-5)/8)
//  in all other cases, apply Tonneli-Shanks
bool BigModSquareRoot(BigNum& n, BigNum& p, BigNum& r) {
  uint64_t bot = p.value_[0] & 0x7;
  BigNum p_temp(p.Size());

  if (bot == 1)
    return BigModTonelliShanks(n, p, r);

  BigNum t1(1 + 2 * p.size_);
  BigNum t2(1 + 2 * p.size_);
  BigNum t3(1 + 2 * p.size_);
  if (bot == 3 || bot == 7) {
    if (!BigAdd(p, Big_One, p_temp)) {
      //LOG(ERROR) << "BigUnsignedAdd 1 in BigModSquareRoot failed\n";
      return false;
    }
    if (!BigBitShift(-2, p_temp, t1)) {
      //LOG(ERROR) << "BigBitShift 1 in BigModSquareRoot failed\n";
      return false;
    }
    if (!BigModExp(n, t1, p, r)) {
      //LOG(ERROR) << "BigModExp 1 in BigModSquareRoot failed\n";
      return false;
    }
  } else if (bot == 5) {
    if (!BigSub(p, Big_One, p_temp)) {
      //LOG(ERROR) << "BigUnsignedSub 2 in BigModSquareRoot failed\n";
      return false;
    }
    if (!BigBitShift(-2, p_temp, t1)) {
      //LOG(ERROR) << "BigBitShift 2 in BigModSquareRoot failed\n";
      return false;
    }
    if (!BigModExp(n, t1, p, t2)) {
      //LOG(ERROR) << "BigModExp 2 in BigModSquareRoot failed\n";
      return false;
    }

    if (BigCompare(Big_One, t2) == 0) {
      p_temp.ZeroNum();
      // if(b==1) x= a^((p+3)/8) (mod p)
      p_temp.ZeroNum();
      t1.ZeroNum();
      if (!BigAdd(p, Big_Three, p_temp)) {
        //LOG(ERROR) << "BigUnsignedAdd 2 in BigModSquareRoot failed\n";
        return false;
      }
      if (!BigBitShift(-3, p_temp, t1)) {
        //LOG(ERROR) << "BigBitShift 3 in BigModSquareRoot failed\n";
        return false;
      }
      if (!BigModExp(n, t1, p, r)) {
        //LOG(ERROR) << "BigModExp 3 in BigModSquareRoot failed\n";
        return false;
      }
    } else {
      t1.ZeroNum();
      t2.ZeroNum();
      t3.ZeroNum();
      p_temp.ZeroNum();
      //  otherwise, x= (2a)(4a)^((p-5)/8)
      if (!BigSub(p, Big_Five, p_temp)) {
        //LOG(ERROR) << "BigUnsignedSub 3 in BigModSquareRoot failed\n";
        return false;
      }
      if (!BigBitShift(-3, p_temp, t1)) {
        //LOG(ERROR) << "BigBitShift 4 in BigModSquareRoot failed\n";
        return false;
      }
      if (!BigBitShift(2, n, t2)) {
        //LOG(ERROR) << "BigBitShift 5 in BigModSquareRoot failed\n";
        return false;
      }
      if (!BigModExp(t2, t1, p, t3)) {
        return false;
      }
      t1.ZeroNum();
      t2.ZeroNum();
      if (!BigBitShift(1, n, t2)) {
        //LOG(ERROR) << "BigBitShift 6 in BigModSquareRoot failed\n";
        return false;
      }
      if (!BigModMult(t2, t3, p, r)) {
        LOG(ERROR) << "BigModMult in BigModSquareRoot failed\n";
        return false;
      }
    }
  } else {
    return false;
  }
  return true;
}

// BigMakeMont(a,m,r)= a R (mod m)
//  R>p, (p,R)=1.  Usually, R=2^r
//  0<= T < pR, p'= -p^(-1) (mod R)
//
//  Reduction: (p, R)= 1.  U= Tp' (mod R)
//    (T+Um)/R = TR^(-1) (mod p)
//  Theorem: (T+Um)/R = TR^(-1) or TR^(-1) + p
//
//  Reduce
//    A= T
//    for(i=0;i<n; i++) {
//      u[i]= a[i]p' mod b  (b is base, R=b^n)
//      A= A+u[i]m b^i
//    }
//    A= A/b^i
//    if(A>=p) A=- p;
//    return A
//
//  Multiply.  0<= x, y <p.  R=b^n.  output: xyR^(-1) (mod p)
//    A= 0
//    for(i=0;i<n; i++) {
//      u[i]= (a[0]+x[i]y[0]) p' mod b  (b is base, R=b^n)
//      A= A+u[i]y[0] (mod b)
//      A= (A+x[i]y+u[i]m)/b
//    }
//    if(A>=p)  A-= p
//    return A
bool BigMakeMont(BigNum& a, int r, BigNum& m, BigNum& mont_a) {
#if 0
  int n = a.size_ > m.size_ ? a.size_ : m.size_;
  int k = (r + NBITSINUINT64 - 1) / NBITSINUINT64;
  if (k > n)
    n = k;

  BigNum t1(1 + 2 * n);

  if (m.IsZero()) {
    LOG(ERROR) << "Modulus is 0 BigMakeMont\n";
    return false;
  }
  if (!BigBitShift(Big_One, r, t1)) {
    //LOG(ERROR) << "BigBitShift fails in BigMakeMont\n";
    return false;
  }
  return BigModMult(a, t1, m, mont_a);
#endif
  return false;
}

// BigMontParams
//  Calculate m': RR'-mm'=1
bool BigMontParams(BigNum& m, int r, BigNum& m_prime) {
#if 0
  int n = (r + NBITSINUINT64 - 1) / NBITSINUINT64;
  if (m.size_ > n)
    n = m.size_;
  BigNum g(2 * n + 1);
  BigNum R(2 * n + 1);
  BigNum R_prime(2 * n + 1);
  BigNum neg_m_prime(2 * n + 1);

  if (!BigBitShift(Big_One, r, R)) {
    LOG(ERROR) << "BigBitShift fails in BigMontParams\n";
    return false;
  }
  if (!BigExtendedGCD(m, R, neg_m_prime, R_prime, g)) {
    LOG(ERROR) << "BigExtendedGCD fails in BigMontParams\n";
    return false;
  }
  if (!BigModNormalize(neg_m_prime, R)) {
    LOG(ERROR) << "BigModNormalize fails in BigMontParams\n";
    return false;
  }
  if (!BigSub(R, neg_m_prime, m_prime)) {
    LOG(ERROR) << "BigSub fails in BigMontParams\n";
    return false;
  }
#endif
  return true;
}

// BigMontReduce(a,m,r)= a R^(-1) (mod m)
bool BigMontReduce(BigNum& a, int r, BigNum& m, BigNum& m_prime,
                   BigNum& mont_a) {
#if 0
  int n = (r + NBITSINUINT64 - 1) / NBITSINUINT64;
  if (m.size_ > n)
    n = m.size_;
  if (a.size_ > n)
    n = a.size_;
  BigNum t(4 * n + 1);
  BigNum v(4 * n + 1);
  BigNum w(4 * n + 1);
  BigNum R(4 * n + 1);
  int i;

  if (!BigMult(a, m_prime, t))
    return false;

  // reduce t mod 2^r
  int k = r / NBITSINUINT64;
  uint64_t u;

  for (i = (k + 1); i < t.capacity_; i++) t.value_[i] = 0ULL;
  int l = r - (k * NBITSINUINT64);
  u = (0xffffffffffffffffULL) >> (NBITSINUINT64 - l);
  t.value_[k] = t.value_[k] & u;
  t.Normalize();

  if (!BigMult(t, m, w)) {
    LOG(ERROR) << "BigMult error in BigMontReduce\n";
    return false;
  }
  if (!BigAdd(w, a, v)) {
    LOG(ERROR) << "BigAdd error in BigMontReduce\n";
    return false;
  }
  if (!BigBitShift(v, -r, mont_a)) {
    LOG(ERROR) << "BigBitShift error in BigMontReduce\n";
    return false;
  }
  if (BigCompare(m, mont_a) <= 0) {
    if (!BigUnsignedSubFrom(mont_a, m)) {
      LOG(ERROR) << "BigUnsignedSubFrom error in BigMontReduce\n";
      return false;
    }
  }
  if (BigCompare(m, mont_a) <= 0) {
    // shouldn't need this
    BigModNormalize(mont_a, m);
  }
#endif
  return true;
}

bool BigMontMult(BigNum& aR, BigNum& bR, BigNum& m, uint64_t r, BigNum& m_prime,
                 BigNum& abR) {
#if 0
  int n = (r + NBITSINUINT64 - 1) / NBITSINUINT64;
  if (m.size_ > n)
    n = m.size_;
  if (aR.size_ > n)
    n = aR.size_;
  if (bR.size_ > n)
    n = bR.size_;

  BigNum t(2 * n + 1);
  bool ret = true;

  if (!BigUnsignedMult(aR, bR, t)) {
    LOG(ERROR) << "BigMult fails in BigMontMult\n";
    ret = false;
  }
  if (ret && !BigMontReduce(t, r, m, m_prime, abR)) {
    LOG(ERROR) << "BigMontReduce fails in BigMontMult\n";
    ret = false;
  }
  if (BigCompare(abR, m) >= 0) {
    // shouldn't need this
    BigModNormalize(abR, m);
  }
  return ret;
#endif
  return false;
}

//  MontExp
//    Let Mont(a,b)= abR^-1 (mod p)
//    R= b^r, m'= -m^(-1) (mod p), e= (e[t]...e[0])_2
//    X= Mont(x, R^2(mod p))
//    A= R (mod p)
//    for(i=t; i>=0; i--) {
//      A= Mont(A,A)
//      if(e[i]==1) A= Mont(A,X)
//    }
//    A= Mont(A,1)
//    return A
bool BigMontExp(BigNum& b, BigNum& e, int r, BigNum& m, BigNum& m_prime,
                BigNum& out) {
#if 0
  int n = (r + NBITSINUINT64 - 1) / NBITSINUINT64;
  if (m.size_ > n)
    n = m.size_;
  if (b.size_ > n)
    n = b.size_;
  if (e.size_ > n)
    n = e.size_;

  BigNum square(4 * n + 1);
  BigNum accum(4 * n + 1);
  BigNum t(4 * n + 1);
  int k = BigHighBit(e);
  int i;

  if (!BigMakeMont(b, r, m, square)) {
    LOG(ERROR) << "BigMakeMont 1 fails in BigMontExp\n";
    return false;
  }
  if (!BigMakeMont(Big_One, r, m, accum)) {
    LOG(ERROR) << "BigMontMult 2 fails in BigMontExp " << m.size_ << ", "
               << square.size_ << "\n";
    return false;
  }
  for (i = 1; i <= k; i++) {
    if (BigBitPositionOn(e, i)) {
      if (!BigMontMult(accum, square, m, r, m_prime, t)) {
        LOG(ERROR) << "BigMontMult 3 fails in BigMontExp " << m.size_ << ", "
                   << square.size_ << "\n";
        return false;
      }
      accum.CopyFrom(t);
    }
    t.ZeroNum();
    if (i != k) {
      if (!BigMontMult(square, square, m, r, m_prime, t)) {
        printf(
            "b.size_: %d, square.size_: %d, m.size_: %d, m_prime.size_: %d\n",
            b.size_, square.size_, m.size_, m_prime.size_);
        LOG(ERROR) << "BigMontMult 4 fails in BigMontExp " << i << " "
                   << m.size_ << ", " << square.size_ << "\n";
        return false;
      }
      square.CopyFrom(t);
    }
  }
  return BigMontReduce(accum, r, m, m_prime, out);
#endif
  return false;
}
