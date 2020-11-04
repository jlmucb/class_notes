#include "stdio.h"
#include "math.h"

double f(double x, int degree, double a[]) {
  double y = 0.0;
  double t = 1.0;

  for (int i = 0; i <= degree; i++) {
    y += t * a[i];
    t *= x;
  }
  return y;
}

double solve (double precision, double left, double right,
      double(*f)(double x, int degree, double a[]),
      int degree, double a[]) {
  double cur_left = left;
  double cur_right = right;
  double cur_left_value;
  double cur_right_value;
  int j = 10000;
  double t_x, t_y;

  while (j-- > 0) {
    cur_left_value = f(cur_left, degree, a);
    if (fabs(cur_left_value) < precision) {
      return cur_left;
    }
    cur_right_value = f(cur_right, degree, a);
    if (fabs(cur_right_value) < precision) {
      return cur_right;
    }
    t_x = (cur_right - cur_left) / 2.0 + cur_left; 
    t_y = f(t_x, degree, a);
    if (t_y < 0.0 && cur_right_value > 0.0) {
      cur_left = t_x;
      continue;
    } else if (t_y > 0.0 && cur_right_value < 0.0) {
      cur_left = t_x;
      continue;
    } else {
      cur_right = t_x;
    }
  }
  printf("No convergence\n");
  return 0.0;
}

void print_poly(int degree, double a[]) {
  for (int i = degree; i >= 0; i--) {
    if (i>0) {
      printf("%lf x**%d + ", a[i], i);
    } else {
      printf("%lf", a[i]);
    }
  }
}

int main(int an, char** av) {
  int degree = 2;
  double a[6];

  // double x1 = 1.6730397826141868425;
  //double x2 = 0.8584070796460176991;
  //
  // 3.000000 x**4 + -6.000000 x**3 + 7.000000 x**2 + 0.000000 x**1 + -15.000000 solution is 1.673039
  // 1.00 x**5 + -2.00 x**4 + -3.00 x**3 + -2.00 x**2 + -7.00 x**1 + 10.00 solution is 0.858407
  // 2.0 x**5 + -1.0 x**4 + 3.0 x**3 + 6.0 x**2 + 5.0 x**1 + -11.0 solution is 0.858407
#if 0
  a[5] = 0.0;
  a[4] = 3.0;
  a[3] = -6.0;
  a[2] = 7.0;
  a[1] = 0.0;
  a[0] = -15.0;
  degree = 4;
  double x = solve(.0000000000001, -1.0, 2.0, f, degree, a);
  print_poly(degree, a);
  printf("\n   solution is %14.11lf, f(x) = %14.11lf\n", x, f(x, 4, a));
#else
  for (int n5 = 0; n5 < 4; n5++) {
    a[5] = (double) n5;
    for (int n4 = -4; n4 < 4; n4++) {
      a[4] = (double) n4;
      for (int n3 = -6; n3 < 6; n3++) {
        a[3] = (double) n3;
        for (int n2 = -8; n2 < 8; n2++) {
          a[2] = (double) n2;
          if (n5 != 0)
            degree = 5;
          else if (n4 != 0)
            degree = 4;
          else if (n3 != 0)
            degree = 3;
          else if (n2 != 0)
            degree = 2;
          else
            continue;
          for (int n1 = -15; n1 < 15 ; n1++) {
            a[1] = (double) n1;
            for (int n0 =  -25; n0 < 25; n0++) {
              if (n0 == 0)
                continue;
              a[0] = (double) n0;
              double x = solve(.0000000001, -1.0, 2.0, f, degree, a);
              print_poly(degree, a);
              printf(" solution is %lf\n", x);
            }
          }
        }
      }
    }
  }
#endif
  return 0;
}
