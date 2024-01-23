#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 

#ifndef byte
typedef unsigned char byte;
#endif
using namespace std;

void mult( int a1, int b1, int c1, int d1, int a2, int b2, int c2, int d2,
	int n, int* aa, int* bb, int* cc, int* dd) {
  *aa = (a1 * a2 + b1 * c2) % n;
  *bb = (a1 * b2 + b1 * d2) % n;
  *cc = (c1 * a2 +d1 * c2) % n;
  *dd = (c1 * b2 + d1 * d2) % n;
}

bool one(int a, int b, int c, int d) {
  return a ==1 && b == 0 && c == 0 && d == 1;
}

void printm(int a, int b, int c, int d) {
  printf("%02d %02d\n", a, b);
  printf("%02d %02d\n", c, d);
}

int posmod(int n, int a) {
  int b = a % n;

  while (b < 0)
    b += n;
  return b;
}

int det(int n, int a, int b, int c, int d) {
  return posmod(n, a*d - b * c);
}

int main(int an, char** av) {
  int k = 1;
  int n = atoi(av[1]);
  int a = atoi(av[2]);
  int b = atoi(av[3]);
  int c = atoi(av[4]);
  int d = atoi(av[5]);
  int na = a;
  int nb = b;
  int nc = c;
  int nd = d;
  int ta, tb, tc, td;

  if (1 != det(n, a, b,c,d)) {
    printf("Not in SL(2, %d)\n",n);
    return 1;
  }

  printf("Modulus: %d, Matrix:\n", n);
  printm(a,b,c,d);
  printf("\n");

  for (;;) {
    if (one(na, nb,nc, nd)) {
      printf("Order %d\n", k);
      break;
    }
    ta= na;
    tb= nb;
    tc= nc;
    td= nd;

    mult(a,b,c,d,ta,tb,tc,td,n, &na, &nb, &nc, &nd);
    k++;
  }
  return 0;
}
