#include "stdio.h"

// over GF(7)
const int m = 7;

inline int index (int nr, int nc, int i, int j) {
  return nr * i + j;
}

int row_times_col(int nr1, int nc1, int nr2, int nc2, int r, int c, int* a, int* b) {
  int t = 0;

  for (int i = 0; i < nc1; i++) {
    t+= (a[index(nr1, nc1, r, i)] * b[index(nr2, nc2, i, c)]) % m;
    t %= m;
  }
  return t;
}

bool mat_mult(int nr1, int nc1, int nr2, int nc2, int* a, int* b, int* c) {
  if (nc1 != nr2)
    return false;

  for (int i = 0; i < nr1; i++) {
    for (int j = 0; j < nc2; j++) {
      c[index(nr1, nc1, i, j)] = row_times_col(nr1, nc1, nr2, nc2, i, j, a, b);
    }
  }
  return true;
}

bool sl2_matrix_gen (int* a, int u, int v, int w, int x) {

  int t = (m * m + u * x - v * w) % m;
  if (t != 1)
    return false;

  a[index(2, 2, 0, 0)] = u;
  a[index(2, 2, 0, 1)] = v;
  a[index(2, 2, 1, 0)] = w;
  a[index(2, 2, 1, 1)] = x;
  
  return true;
}

void print_mat(int nr, int nc, int* a) {
  printf("-------------------------\n");
  for (int i = 0; i < nr; i++) {
    for (int j = 0; j < nr; j++) {
      printf("%2d ", a[index(nr, nc, i, j)]);
    }
    printf("\n");
  }
  printf("-------------------------\n");
}

bool copy_mat(int nr, int nc, int* a, int* b) {
  for (int i = 0; i < nr; i++) {
    for (int j = 0; j < nc; j++) {
      b[index(nr, nc, i, j)]= a[index(nr, nc, i, j)];
    }
  }
  return true;
}

bool is_identity(int nr, int nc, int* a) {
  for (int i = 0; i < nr; i++) {
    for (int j = 0; j < nc; j++) {
      if (i == j) {
        if  ((a[index(nr, nc, i, j)] % m) != 1)
          return false;
      }
      else {
        if ((a[index(nr, nc, i, j)] % m) != 0)
          return false;
      }
    }
  }
  return true;
}

int calc_order(int max, int* a) {
  int b[4], c[4];

  if (is_identity(2, 2, a))
    return 1;
  copy_mat(2, 2, a, b);

  for (int i = 2; i <= max; i++) {
    if (!mat_mult(2, 2, 2, 2, a, b, c))
      continue;
    if (is_identity(2, 2, c))
      return i;
    copy_mat(2, 2, c, b);
  }
  return -1;
}

int main(int an, char** av) {
  int a[4];
  int n = 0;
  int order_ct[9] = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
  };

  for (int u = 0; u < m; u++) {
    for (int v = 0; v < m; v++) {
      for (int w = 0; w < m; w++) {
        for (int x = 0; x < m; x++) {
          if (!sl2_matrix_gen (a, u, v, w, x))
            continue;
          n++;
          int k = calc_order(8, a);
          if (k > 0) {
            order_ct[k]++;
            print_mat(2, 2, a);
            printf(" has order %d\n\n", k);
          }
        }
      }
    }
  }
  printf("\nNumber generated: %d\n", n);
  printf("\n\norder summary\n");
  for (int i = 0; i < 9; i++)
    printf("%3d ", i);
  printf("\n");
  for (int i = 0; i < 9; i++)
    printf("%3d ", order_ct[i]);
  printf("\n");
  return 0;
}
