#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>


//
// newpi.cc
// (c) copyright, 2015, John L. Manferdelli


// --------------------------------------------------------------------- 


double g(double x) {
  double y;

  y = 1.0 - x*x;
  return sqrt(y);
}

int main(int an, char** av) {
  double r = 1.0;
  long long int i;
  long long int n = 10000000;
  double dx = r/((double)n);
  double sum = 0.0;
  double cx = 0.0;
  double u, v;
  
  for (i = 0; i < n; i++) {
    u = g(cx);
    cx += dx;
    if (i < (n-1)) {
    	v = g(cx); 
        sum += dx * ((u + v)/2.0);
    } else {
        sum += dx * u;
    }
  }
  printf("pi:  %12.8F\n", 4.0 * sum);
  return 0;
}


// ---------------------------------------------------------------------------


