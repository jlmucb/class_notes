#include <stdio.h>
#include <sys/types.h>
#include <string>
#include <types.h>

#ifndef ARM64_ARITH__
#define ARM64_ARITH__

void ZeroArray(int n, uint64_t* p);
int SizeArray(int n, uint64_t* p);
int MostSignificantDigit(int n, uint64_t* p);
void CopyArray(int n, uint64_t* from, uint64_t* to);
int CompareArray(int n1, uint64_t* p1, int n2, uint64_t* p2);
void PrintArray(int n, uint64_t* a);
bool IsZeroArray(int n, uint64_t* p);
bool DivideArrayByDigit(int size_a, uint64_t* a, uint64_t b, int size_q, uint64_t* q, uint64_t* r);
int high_bit_in_unsigned(uint64_t x);
void estimate_quotient(uint64_t n0, uint64_t n1, uint64_t d1, uint64_t* e1);

// Possibly use __attribute__((packed))
//  num= value_[0]+ 2^64 value_[1] + ... + 2^(64n) value_[n]
class BigNum {
 public:
  bool sign_;  // true: negative
  __attribute__((aligned(4))) int capacity_;
  __attribute__((aligned(4))) int size_;
  __attribute__((aligned(8))) uint64_t* value_;

  BigNum(int size);
  BigNum(BigNum& n);
  BigNum(int size, uint64_t);  // BigNum with one initialized digit
  ~BigNum();

  bool Sign();
  int Capacity();  // total number of digits (64 bits) allocated
  int Size();      // number of digit required to hold current value
  uint64_t* ValuePtr();
  void SetSign(bool sign);

  bool IsPositive();
  bool IsZero();
  bool IsOne();
  bool IsNegative();
  void ToggleSign();
  void Normalize();
  void ZeroNum();
  bool CopyFrom(BigNum&);
  bool CopyTo(BigNum&);
  string* BigConvertToDecimal();
  string* BigConvertToHex();
  void Print();
};

extern BigNum Big_Zero;
extern BigNum Big_One;
extern BigNum Big_Two;
extern BigNum Big_Three;
extern BigNum Big_Four;
extern BigNum Big_Five;

// returns  1, if l>r
// returns  0, if l==r
// returns -1, if l<r
int BigCompare(BigNum& l, BigNum& r);
int BigAbsoluteCompare(BigNum& l, BigNum& r);

void PrintBig(BigNum& a);
BigNum BigAbsoluteValue(BigNum& bnA);
BigNum* BigConvertFromDecimal(const char* in);
BigNum* BigConvertFromHex(const char* in);

int LargestPowerTwo(BigNum& a);

void Uint64AddStep(uint64_t a, uint64_t b, uint64_t* result, uint64_t* carry);
void Uint64MultStep(uint64_t a, uint64_t b, uint64_t* result, uint64_t* carry);
void Uint64DivStep(uint64_t a, uint64_t b, uint64_t c, uint64_t* result,
                   uint64_t* carry);
void Uint64AddWithCarryStep(uint64_t a, uint64_t b, uint64_t carry_in,
                            uint64_t* result, uint64_t* carry_out);
void Uint64SubWithBorrowStep(uint64_t a, uint64_t b, uint64_t borrow_in,
                             uint64_t* result, uint64_t* borrow_out);
void Uint64MultWithCarryStep(uint64_t a, uint64_t b, uint64_t carry1,
                             uint64_t carry2, uint64_t* result,
                             uint64_t* carry_out);


void u_add(int size_a, int size_b, uint64_t* a, uint64_t* b, int size_c, uint64_t* c);
void u_sub(int size_a, int size_b, uint64_t* a, uint64_t* b, int size_c, uint64_t* c);
void u_mult(int size_a, int size_b, uint64_t* a, uint64_t* b, int size_c, uint64_t* c);

bool BigAdd(BigNum& bnA, BigNum& bNB, BigNum& bnC);
bool BigSub(BigNum& bnA, BigNum& bNB, BigNum& bnC);
bool BigMult(BigNum& bnA, BigNum& bNB, BigNum& bnC);
bool BigDigitShift(int shift, BigNum& bnA, BigNum& bnR);
bool BigBitShift(int shift, BigNum& bnA, BigNum& bnR);
bool BigMultByDigit(BigNum& bnA, uint64_t x, int shift, BigNum& bnR);
bool BigDiv(BigNum& bnA, BigNum& bNB, BigNum& bnQ, BigNum& bnR);
int BigMaxPowerOfTwoDividing(BigNum& n);

bool BigUnsignedEuclid(BigNum& a, BigNum& b, BigNum& q, BigNum& r);
bool BigSquare(BigNum& a, BigNum& r);
bool BigMod(BigNum& a, BigNum& m, BigNum& r);
bool BigModAdd(BigNum& a, BigNum& b, BigNum& m, BigNum& r);
bool BigModSub(BigNum& a, BigNum& b, BigNum& m, BigNum& r);
bool BigModNeg(BigNum& a, BigNum& m, BigNum& r);
bool BigModMult(BigNum& a, BigNum& b, BigNum& m, BigNum& r);
bool BigModSquare(BigNum& a, BigNum& m, BigNum& r);
bool BigModInv(BigNum& a, BigNum& m, BigNum& r);
bool BigModDiv(BigNum& a, BigNum& b, BigNum& m, BigNum& r);
bool BigModExp(BigNum& b, BigNum& e, BigNum& m, BigNum& r);
bool BigMakeMont(BigNum& a, int r, BigNum& p, BigNum& mont_a);
bool BigMontReduce(BigNum& a, int r, BigNum& m, BigNum& m_prime, BigNum& out);
bool BigMontParams(BigNum& m, int r, BigNum& m_prime);
bool BigMontMult(BigNum& aR, BigNum& bR, BigNum& m, uint64_t r, BigNum& m_prime, BigNum& abR);
bool BigMontExp(BigNum& b, BigNum& e, int r, BigNum& m, BigNum& m_prime, BigNum& out);
bool BigExtendedGCD(BigNum& a, BigNum& b, BigNum& x, BigNum& y, BigNum& g);
bool BigCRT(BigNum& s1, BigNum& s2, BigNum& m1, BigNum& m2, BigNum& r);
bool BigGenPrime(BigNum& p, uint64_t num_bits);
bool BigIsPrime(BigNum& n);
bool BigMillerRabin(BigNum& n, BigNum** a, int trys = 20);
bool BigModIsSquare(BigNum& n, BigNum& p);
bool BigModSquareRoot(BigNum& n, BigNum& p, BigNum& r);
bool BigModTonelliShanks(BigNum& n, BigNum& p, BigNum& s);

#endif

