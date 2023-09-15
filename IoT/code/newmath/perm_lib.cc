#include "perm.h"

perm::perm() {
  n_ = 0;
  s_ = nullptr;
}

perm::perm(int n) {
  n_ = n;
  s_ = new short int[n];
}

perm::~perm() {
  if (s_ != nullptr) {
    delete []s_;
    s_ = nullptr;
  }
}

int perm::apply(short int in) {
  if (n_ <= in)
    return -1;
  return (int)s_[in];
}

bool perm::make_identity() {
  for (int i = 0; i < n_; i++)
    s_[i] = i;
  return true;
}

void perm::print() {
  byte seen[n_];

  memset(seen, 0, n_);
  int last = 0;

  while (last < n_) {
    if (seen[last] != 0) {
      last++;
      continue;
    }
    int cur = last;
    printf("(");
    last++;
    while (seen[cur] == 0) {
      printf(" %d ", cur + 1);
      seen[cur] = 1;
      cur = s_[cur];
    }
    printf(")");
    
  }
}

bool compose(const perm& in1, const perm& in2, perm* out) {
  if (in1.n_ != in2.n_ || in1.n_ != out->n_)
    return false;
  for (int i = 0; i < in1.n_; i++) {
    out->s_[i] = in2.s_[in1.s_[i]];
  }
  return true;
}

bool inverse(const perm& in, perm* out) {
  if (in.n_ != out->n_)
    return false;
  for (int i =0; i < in.n_; i++) {
    out->s_[in.s_[i]] = i;
  }
  return true;
}

int factorial(int n) {
  if (n < 0 || n > 10)
    return -1;
  if (n == 0 || n == 1)
    return 1;

  int f = 1;
  for (int i = 2; i <= n; i++)
    f *= i;
  return f;  
}

void copy_array(int k, short int* a1, short int* a2) {
  for (int i = 0; i < k; i++)
    a2[i] = a1[i];
}

//  in should be an array of (n-1)! permutations of degree n-1
//  out should be an array of n! permutations of degree n
bool gen_perms(int n, perm* in, perm* out) {
  if (n == 1) {
    out[0].s_[0] = 0;
    return true;
  }

  if (out[0].n_ != (in[0].n_ + 1)) {
    return false;
  }

  int nm1 = n - 1;
  int nm1_fact = factorial(in[0].n_);

  if (nm1 <= 0)
    return false;

  for (int i = 0; i < n; i++) {
    int base = i * nm1_fact;
    for (int j = 0; j < nm1_fact; j++) {
      copy_array(n - 1, in[j].s_, out[base + j].s_);
      if (i == nm1) { 
        out[base + j].s_[nm1] = nm1;
        continue;
      }
      int m = out[base + j].s_[i];
      out[base + j].s_[nm1] = m;
      out[base + j].s_[i] = nm1;
    }
  }
  
  return true;
}
