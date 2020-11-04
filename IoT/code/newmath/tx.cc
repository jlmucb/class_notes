#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>


//
// tx.cc
// (c) copyright, 2015, John L. Manferdelli


// --------------------------------------------------------------------- 


#define PI 3.14159265359

double g(double x) {
  double y = PI * x;
  
  return sin(y) / y;
}

int main(int an, char** av) {
  long int n = 600;
  double dx = .05;
  double x, y;
  
  for (int i = 1; i < n; i++) {
    x = dx * ((double) i);
    y = g(x);
    printf("%12.8F %12.8F\n", x, y);
  }
  return 0;
}


// ---------------------------------------------------------------------------


