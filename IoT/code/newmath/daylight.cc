#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>

// daylight with spherical trigonometry

const double pi = 3.14159265;

double degrees_to_radians(double d) {
  return (d / 180.0) * pi;
}

double radians_to_degrees(double r) {
  return (r / pi) * 180.0;
}

double sunrise_to_sunset_radians(double r_inclination, double r_lat) {
  double a = acos(sin(r_inclination) / cos(r_lat));
  double x = asin(sin(a) / cos(r_inclination));

  if (a <= (pi / 2.0))
    return 2.0 * x;
  return 2.0 * (pi - x);
}

int main(int an, char** av) {
  double lat = 37.45;
  double days_since_solstice = 0.0;
  double inclination = 0.0;
  int n;

  if (an < 2) {
    printf("daylight.exe --days=ddd --lat=latitude\n");
    return 0;
  }

  for (int i = 0; i < an; i++) {
    n = strlen("--days=");
    if (strncmp(av[i], "--days=", n) == 0) {
      sscanf(&(av[i][n]), "%lf", &days_since_solstice);
    }
    n = strlen("--lat=");
    if (strncmp(av[i], "--lat=", n) == 0) {
      sscanf(&(av[i][n]), "%lf", &lat);
    }
  }

  double t = (days_since_solstice / 365.25) * 2.0 * pi;
  inclination = 23.5 * cos(t);
  printf("Latitude: %6.2lf, days since winter solstice: %6.2lf, inclination: %6.2lf, ",
    lat, days_since_solstice, inclination);

  double r_inclination = degrees_to_radians(inclination);
  double r_lat = degrees_to_radians(lat);

  double x = sunrise_to_sunset_radians(r_inclination, r_lat);
  double dl = (x / pi) * 12.0;
  printf("%6.2lf hours of daylight\n", dl);
 
  return 0;
}
