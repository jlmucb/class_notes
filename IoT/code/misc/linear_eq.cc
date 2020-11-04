#include "stdio.h"
#include "math.h"

class rows {
public:
  rows(int n);
  ~rows();
  void multiply_row_by(double d);
  int n_;
  double *r_;
};

rows::rows(int n) {
  n_ = n;
  r_ = new double[n];
}

rows::~rows() {
  delete []r_;
}

void rows::multiply_row_by(double d) {
  for (int i = 0; i < n_; i++)
    r_[i] *= d;
}

class array {
public:
  array(int m, int n);
  ~array();
  void swap_rows(int r1, int r2);
  void subtract_rows(int r1, int r2);
  int find_non_zero_col_in_row(int r, int c);
  int m_, n_;
  rows** row_;
};

array::array(int m, int n) {
  m_ = m;
  n_ = n;
  row_ = new rows*[m_];
  for (int i = 0; i < m_; i++) {
    row_[i] = new rows(n_);
  }
}

array::~array() {
  for (int i = 0; i < m_; i++) {
    delete row_[i];
  }
  delete []row_;
}

void array::swap_rows(int r1, int r2) {
  if (r1 >= m_ || r2 >= m_)
    return;
  rows* r = row_[r1];
  row_[r1] = row_[r2];
  row_[r2] = r;
}

void array::subtract_rows(int r1, int r2) {
  if (r1 >= m_ || r2 >= m_)
    return;
  for (int i = 0; i < n_; i++) {
    row_[r1]->r_[i] -= row_[r2]->r_[i];
  }
}

int array::find_non_zero_col_in_row(int r, int c) {
  if (c >= n_)
    return -1;

  for (int i = r; i < m_; i++) {
    if (row_[i]->r_[c] != 0.0)
      return i;
  }
  return -1;
}

void print_poly(int degree, double a[]) {
  for (int i = degree; i >= 0; i--) {
    if (i > 0) {
      printf("%lf x**%d + ", a[i], i);
    } else {
      printf("%lf", a[i]);
    }
  }
}

void print_matrix(array& mat) {
  for (int i = 0; i < mat.m_; i++) {
    for (int j = 0; j < mat.n_; j++) {
      printf("%9.6lf ", mat.row_[i]->r_[j]);
    }
  printf("\n");
  }
}

bool linear_solve(array& ar, double* c, double* sols) {
  int n;
  double d;

  for (int i = 0; i < ar.n_; i++) {
    n = ar.find_non_zero_col_in_row(i, i);
    if (n < 0)
      return false;
    if (n != i) {
      ar.swap_rows(i, n);
    }
    for (int j = i; j < ar.m_; j++) {
      d = 1.0 / ar.row_[j]->r_[i];
      ar.row_[j]->multiply_row_by(d);
      c[j] *= d;
    }
    for (int j = (i + 1); j < ar.m_; j++) {
      ar.subtract_rows(j, i);
      c[j] -= c[i];
    }
printf("Step %d:\n", i);
print_matrix(ar); for (int j = 0; j < ar.n_; j++) printf("%lf\n", c[j]);
  }
  for (int i = (ar.n_ - 1); i >= 0; i--) {
    double t = c[i];
    for (int k = (i + 1); k < ar.n_; k++)
      t -= ar.row_[i]->r_[k] * sols[k];
    sols[i] = t;
  }
  return true;
}

double x1 = 1.6730397826141868425;
double x2 = 0.8584070796460176991;
double x3 = 0.2807764064044151374;
double x4 = 0.1823285041253521755;


int main(int an, char** av) {
  int degree = 2;
  array matrix(degree + 1, degree + 1);

  double* c = new double[degree + 1];
  double* sols = new double[degree + 1];

#define TEST
#ifdef TEST
  printf("test\n");
  matrix.row_[0]->r_[0] = 1.0;
  matrix.row_[0]->r_[1] = 2.0;
  matrix.row_[0]->r_[2] = 3.0;
  c[0] = 6.0;
  matrix.row_[1]->r_[0] = 1.0;
  matrix.row_[1]->r_[1] = 3.0;
  matrix.row_[1]->r_[2] = 4.0;
  c[1] = 8.0;
  matrix.row_[2]->r_[0] = 2.0;
  matrix.row_[2]->r_[1] = 9.0;
  matrix.row_[2]->r_[2] = 17.0;
  c[2] = 28.0;

  print_matrix(matrix);
  for (int i = 0; i <= degree; i++)
    printf("c[%d] = %lf\n", i, c[i]);

  if (!linear_solve(matrix, c, sols)) {
    printf("No solutions\n");
  } else {
    printf("\nreduced matrix:\n");
    print_matrix(matrix);
    printf("\n");
    printf("solutions:\n");
    for (int i = 0; i <= degree; i++) {
      printf("%9.6lf ", sols[i]);
    }
    printf("\n");
  }
#else
  double t;
  double x = x1;

  for (int i = 0; i < (degree + 1); i++) {
    t = 1.0;
    for (int k = 0; k < i; k++)
      t *= x;
    for (int j = 0; j < (degree + 1); j++) {
      matrix.row_[i]->r_[j] = t;
      t *= x;
    }
  }

  printf("x = %lf\n", x);
  print_matrix(matrix);
  for (int i = 0; i <= degree; i++)
    c[i] = 0.0;

  if (!linear_solve(matrix, c, sols)) {
    printf("No solutions\n");
  } else {
    printf("reduced matrix:\n");
    print_matrix(matrix);
    printf("\n");
    printf("solutions:\n");
    for (int i = 0; i <= degree; i++) {
      printf("%9.6lf ", sols[i]);
    }
    printf("\n");
  }
#endif

  delete []c;
  delete []sols;
  return 0;
}
