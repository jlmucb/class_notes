#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>


//
// projellipse.cc
// (c) copyright, 2017, John L. Manferdelli

// vec(h)= (0,0,h)
// C(d, theta) = (d cos(theta), d sin(theta), 0)
// vec(p) = (d cos(theta), d sin(theta), -h)
// w0 = t0C(d, theta) + (1-t0)vec(h)
// q(chi) = (d cos(theta) + r cos(chi), d sin(theta) + r sin(chi, 0)
// l(chi) = t C(d, theta) + (1-t) vec(h)
//
// t= (t0(d^2+h^2))/(d^2+h^2 + dr cos(theta - chi))
// q_pi(chi) - w0 = t(d cos(theta) + r cos(chi), d sin(theta) + r sin(chi), 0) + (1-t) vec(h)
//
// a[i] = i delta(chi), i = 0, ..., n
// r[i]= q_pi(a[i])
// phi[i] = r[i] dot r[0]/ (||r[i]|| ||r[0]||)

#ifndef nullptr
#define nullptr NULL
#endif


// --------------------------------------------------------------------- 


#define PI 3.14159265359

void printResult(FILE* file, double norm_r, double new_theta) {
  double x = norm_r * cos(new_theta);
  double y = norm_r * sin(new_theta);
  printf("||r|| = %9.5lf, new_theta = %9.5lf, (%9.5lf, %9.5lf)\n",
         norm_r, (180.0 * new_theta) / PI, x, y);
  if (file != nullptr) {
    fprintf(file, "%9.5lf, %9.5lf\n", x, y);
  }
}

void printResult2(FILE* file, double norm_r, double x, double y) {
  printf("||r|| = %9.5lf, (%9.5lf, %9.5lf)\n",
         norm_r, x, y);
  if (file != nullptr) {
    fprintf(file, "%9.5lf, %9.5lf\n", x, y);
  }
}

void printProjectedResult(FILE* file, double x, double y) {
  double r = sqrt(x * x + y * y);
  printf("||r|| = %9.5lf, (%9.5lf, %9.5lf)\n",
         r, x, y);
  if (file != nullptr) {
    fprintf(file, "%9.5lf %9.5lf\n", x, y);
  }
}

void project_onto_axis(double ax, double ay, double az, double bx, double by, double bz,
		       double x, double y, double z, double* cx, double* cy) {

  *cx = ax * x + ay * y + az * z;
  *cy = bx * x + by * y + bz * z;
}



int main(int an, char** av) {
  long int n = 360;
  double h = 80;
  double d = 40;
  double r = 40.0;
  double theta = 45 * PI / 180.0;
  double t0 = .1;
  double t;
  double chi;
  double wx0, wy0, wz0;
  double cx0, cy0, cz0;

  double rx0, ry0, rz0;
  double rx, ry, rz;
  double delta_chi = PI * 2.0/(double (n));
  double a;
  double norm_r;
  double norm_r0;
  double new_theta;
  double dot_r_r0;

  FILE* file = fopen("./ellipse.csv", "w");

  cx0 = d*cos(theta);
  cy0 = d*sin(theta);
  cz0 = 0.0;

  wx0 = t0*cx0;
  wy0 = t0*cy0;
  wz0 = (1.0 - t0) * h;
  
  chi = 0.0;
  rx0 = t0 * (cx0 + r * cos(chi)) - wx0;
  ry0 = t0 * (cy0 + r * sin(chi)) - wy0;
  rz0 = (1.0 - t0) * h - wz0;
  norm_r0 = sqrt(rx0 * rx0 + ry0 * ry0 + rz0 * rz0);

  double w0x_unit, w0y_unit, w0z_unit;
  double ax_unit, ay_unit, az_unit;
  double bx_unit, by_unit, bz_unit;
  double norm_w0 = sqrt(wx0 * wx0 + wy0 * wy0 + wz0 * wz0);

  w0x_unit = wx0 / norm_w0;
  w0y_unit = wy0 / norm_w0;
  w0z_unit = wz0 / norm_w0;

  ax_unit = rx0 / norm_r0;
  ay_unit = ry0 / norm_r0;
  az_unit = rz0 / norm_r0;

  // b_unit = w_unit X a_unit
  bx_unit = w0y_unit * az_unit - w0z_unit * ay_unit;
  by_unit = w0z_unit * ax_unit - w0x_unit * ax_unit;
  bz_unit = w0x_unit * ay_unit - w0y_unit * ax_unit;

  printf("Unit vectors: (%9.4lf, %9.4lf, %9.4lf), (%9.4lf, %9.4lf, %9.4lf), (%9.4lf, %9.4lf, %9.4lf)\n",
    w0x_unit, w0y_unit, w0z_unit, ax_unit, ay_unit, az_unit, bx_unit, by_unit, bz_unit);

  double new_x, new_y;

  for (int i = 0; i < n; i++) {
    chi = delta_chi * ((double) i);
    t = (t0 * (d * d + h * h)) / (d * d + h * h + (d * r * cos(chi - theta)));
    // printf("(chi= %9.5lf, t= %9.5lf) ", chi * 180.0 / PI, t);
    rx = t * (cx0 + r * cos(chi)) - wx0;
    ry = t * (cy0 + r * sin(chi)) - wy0;
    rz = (1.0 - t) * h - wz0;
#if 0
    norm_r = sqrt(rx * rx + ry * ry + rz * rz);
    printResult2(file, norm_r, rx, ry);
#else
    project_onto_axis(ax_unit, ay_unit, az_unit, bx_unit, by_unit, bz_unit,
		      rx, ry, rz, &new_x, &new_y);
    printProjectedResult(file, new_x, new_y);
#endif
  } 

  fclose(file);   
  return 0;
}


// ---------------------------------------------------------------------------


