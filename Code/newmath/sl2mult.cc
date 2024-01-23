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
  int a1 = atoi(av[2]);
  int b1 = atoi(av[3]);
  int c1 = atoi(av[4]);
  int d1 = atoi(av[5]);
  int a2 = atoi(av[6]);
  int b2 = atoi(av[7]);
  int c2 = atoi(av[8]);
  int d2 = atoi(av[9]);
  int fa, fb, fc, fd;
  int ba, bb, bc, bd;

  if (1 != det(n, a1, b1, c1, d1)) {
    printf("A not in SL(2, %d)\n", n);
    return 1;
  }
  if (1 != det(n, a2, b2, c2, d2)) {
    printf("B not in SL(2, %d)\n", n);
    return 1;
  }

  printf("Modulus: %d, A:\n", n);
  printm(a1,b1,c1,d1);
  printf("\n");
  printf("Modulus: %d, B:\n", n);
  printm(a2,b2,c2,d2);
  printf("\n");

  mult(a1,b1,c1,d1,a2,b2,c2,d2,n, &fa, &fb, &fc, &fd);
  mult(a2,b2,c2,d2,a1,b1,c1,d1,n, &ba, &bb, &bc, &bd);

  printf("Modulus: %d, A * B:\n", n);
  printm(fa,fb,fc,fd);
  printf("\n");
  printf("Modulus: %d, B * A:\n", n);
  printm(ba,bb,bc,bd);
  printf("\n");

  return 0;
}
