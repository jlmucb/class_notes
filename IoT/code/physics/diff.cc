#include "stdio.h"
#include "stdlib.h"
#include "math.h"


int main(int an, char** av) {

  if (an != 3) {
    printf("diff num-radiators 1/inter-radiator-dist(in wavelengths)\n");
    return 1;
  }
  int n, m;
  double d, phi, t;
  double x, s;

  n = atoi(av[1]);
  m = atoi(av[2]);
  d = 1.0/((double)m);
  s = (double)n;
  s = s * s;

  // theta varies from 0 to 90 deg, inclusive
  printf("%d radiators separated by %014.8f wavelengths\n", n, d);
  for (int i = 0; i<=90; i++) {
    if (i == 0) {
      x = ((double)n);
      x = x*x;
      // printf("theta: %2d degrees, phi: **************, intensity: %14.8f, relative intensity: %14.8f\n", i, x, x/s);
      printf("%2d %14.8f\n", i, x/s);
    } else {
      t = ((double) i)*3.1415826/180.0;
      // phi = (2*pi*d*sin(t)), d measured in wavelengths
      phi = 2.0 * 3.141592654 * d * sin(t);
      x = sin(((double)n)*phi/2.0) / sin(phi/2.0);
      x = x*x;
      // printf("theta: %2d degrees, phi: %14.8f, intensity: %14.8f, relative intensity: %14.8f\n", i, phi, x, x/s);
      printf("%2d %14.8f\n", i, x/s);
    }
  }

  return 0;
}
