#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>


//
// pi.cc
// (c) copyright, 2015, John L. Manferdelli


#ifndef byte
typedef unsigned char byte;
#endif


double SineHalfAngle(double x) {
  double y, z;

  y = 1.0 - x*x;
  z = 0.5 * (1.0 - sqrt(y));
  return sqrt(z);
}

int main(int an, char** av) {
  int current_n = 4;
  int new_n = 4;
  double current_p;
  double new_p;
  double current_sine;
  double new_sine;

  current_sine = 1.0 / sqrt(2.0);
  current_p = 2.0 * ((double)current_n) * current_sine;
  printf("n: %05d, sine: %9.6f, perimeter: %12.9lf\n", current_n, current_sine,
         current_p);

  int i;
  for (i = 0; i<14; i++) {
    new_n = 2 * current_n;
    new_sine = SineHalfAngle(current_sine);
    new_p = 2.0 * ((double) current_n) * new_sine;
    current_n = new_n;
    current_sine = new_sine;
    current_p = new_p;
    printf("n: %05d, sine: %9.6lf, perimeter: %12.9lf\n", current_n, current_sine,
           current_p);
  }

  return(0);
}


// ---------------------------------------------------------------------------


