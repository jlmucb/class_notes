#include <stdio.h>

int callee(int a) {
  a += 120;
  return a;
}


void caller(int a) {

  int c = callee(a);
  printf("%d\n", c);
}
