#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>

// SL_2(p)
//  mat is
//    a  b
//    c  d
// Stored as abcd in array

void print_mat(int* m) {
  printf("| %02d %02d|\n", m[0], m[1]);
  printf("| %02d %02d|\n", m[2], m[3]);
}

void get_mat(int* m, int* a, int* b, int* c, int* d) {
  *a = m[0];
  *b = m[1];
  *c = m[2];
  *d = m[3];
}

void set_mat(int a, int b, int c, int d, int* m) {
  m[0] = a;
  m[1] = b;
  m[2] = c;
  m[3] = d;
}

int normalize(int p, int n) {
  if (n < 0) {
    int m = -n;
    m += p - 1;
    m /= p;
    n += p * m;
  }
  if (n >= p) {
    int m = n / p;
    n -= m * p;
  }
  return n;
}

int det(int p, int* m) {
  int a, b, c, d;

  get_mat(m, &a, &b, &c, &d);
  return normalize(p, a * d - b * c);;
}

bool mult_mat(int p, int* m1, int* m2, int* res) {
  int a1, b1, c1, d1;
  int a2, b2, c2, d2;
  int a, b, c, d;

  get_mat(m1, &a1, &b1, &c1, &d1);
  get_mat(m2, &a2, &b2, &c2, &d2);

  a = normalize(p, a1 * a2 + b1 * c2);
  b = normalize(p, a1 * b2 + b1 * d2);
  c = normalize(p, c1 * a2 + d1 * c2);
  d = normalize(p, c1 * b2 + d1 * d2);

  set_mat(a, b, c, d, res);
  return true;
}

void copy_mat(int* m1, int* m2) {
  for (int i= 0; i < 4; i++)
    m2[i]= m1[i];
}

bool is_identity(int* m) {
  int a, b, c, d;
  get_mat(m, &a, &b, &c, &d);
  return a == 1 && b == 0 && c == 0 && d == 1;
}

int order(int p, int* m) {
  int t[4] = {
    1, 0, 0, 1
  };
  int r[4] = {
    0, 0, 0, 0
  };

  if (is_identity(m)) {
    return 1;
  }

  int ord = 0;
  for (;;) {
    mult_mat(p, m, t, r);
    ord++;
    copy_mat(r, t);
    if (is_identity(r)) {
      return ord;
    }
  }
  return -1;
}

class T {
public:
  int mat_[4];
  int order_;
};


int main(int an, char** av) {
  int p = 7;

  if (an < 2) {
    printf("sl2p.exe --p=7\n");
    return 0;
  }

  for (int i = 0; i < an; i++) {
    int n = strlen("--p=");
    if (strncmp(av[i], "--p=", n) == 0) {
      sscanf(&(av[i][n]), "%d", &p);
    }
  }

  T x[p*p*p*p];
  int n_t = 0;

  for (int i = 0; i < p; i++) {
    for (int j = 0; j < p; j++) {
      for (int k = 0; k < p; k++) {
        for (int l = 0; l < p; l++) {
          int m[4];

          set_mat(i, j, k, l, m);
          if (det(p, m) == 1) {
            int ord = order(p, m);
            copy_mat(m, x[n_t].mat_);
            x[n_t++].order_ = ord;
          }
        }
      }
    }
  }

  printf("p= %d, %d elements\n", p, n_t);

  return 0;
}
