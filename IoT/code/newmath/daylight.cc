#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>


//
// daylight

const double pi = 3.14159265;


int main(int an, char** av) {
  double lat = 37.45;
  double days_sinc_solstice = 0.0;
  double inclination = 0.0;
  int n;

  if (an < 2) {
    printf("daylight.exe --days=ddd --lat=latitude\n");
    return 0;
  }

  for (int i = 0; i < an; i++) {
    n = strlen("--days=");
    if (strncmp(av[i], "--days=", n) == 0) {
      sscanf(&(av[i][n]), "%lf", &days_sinc_solstice);
    }
    n = strlen("--lat=");
    if (strncmp(av[i], "--lat=", n) == 0) {
      sscanf(&(av[i][n]), "%lf", &lat);
    }
  }

  double t = (days_sinc_solstice / 365.25) * 2 * pi;
  inclination = 23.5 * cos(t);
  printf("Latitude: %lf, days since winter solstice: %lf, inclination: %lf\n",
    lat, days_sinc_solstice, inclination);

  double r_inclination = (inclination / 180.0) * pi;
  double r_lat = (lat / 180.0) * pi;

  double a = acos(sin(r_inclination) / cos(r_lat));
  double x = asin(sin(a) / cos(r_inclination));
  double dl;

  if (a <= (pi / 2.0))
    dl = ((2.0 * x) / pi) * 12.0;
  else
    dl = ((2.0 * (pi - x)) / pi) * 12.0;
  printf("%lf hours of daylight\n", dl);
 
  return 0;
}


// ---------------------------------------------------------------------------


