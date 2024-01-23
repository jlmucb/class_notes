#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <string>
#include <unistd.h>
#include <memory> 

#ifndef __PERM__
#define __PERM__

#ifndef byte
typedef unsigned char byte;
#endif

class perm {
public:
  int n_;
  short int *s_;

  perm();
  perm(int n);
  ~perm();

  bool make_identity();
  int apply(short int in);
  void print();
};

int factorial(int n);
bool compose(const perm& in1, const perm& in2, perm* out);
bool inverse(const perm& in, perm* out);
bool gen_perms(int n, perm* in, perm* out);

#endif
