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
  bool table = false;

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
    if (strcmp(av[i], "--table") == 0) {
      table = true;
    }
  }


  double t = 0.0;
  double r_inclination = 0.0;
  double r_lat = 0.0;
  double x = 0.0;
  double dl = 0.0;

  if (table) {
    printf("Latitude: %6.2lf\n", lat);
    for (int i = 0; i < 366; i++) {
      t = 2.0 * pi * (((double) i) / 365.25);
      inclination = 23.5 * cos(t);
      r_inclination = degrees_to_radians(inclination);
      r_lat = degrees_to_radians(lat);
      x = sunrise_to_sunset_radians(r_inclination, r_lat);
      dl = (x / pi) * 12.0;
      printf("%d, %6.2lf\n", i, dl);
    }
    return 0;
  }
  t = (days_since_solstice / 365.25) * 2.0 * pi;
  inclination = 23.5 * cos(t);
  r_inclination = degrees_to_radians(inclination);
  r_lat = degrees_to_radians(lat);
  x = sunrise_to_sunset_radians(r_inclination, r_lat);
  dl = (x / pi) * 12.0;
  printf("Latitude: %6.2lf, days since winter solstice: %6.2lf, inclination: %6.2lf, ",
    lat, days_since_solstice, inclination);
  printf("%6.2lf hours of daylight\n", dl);
 
  return 0;
}
