//
// Copyright 2018 John Manferdelli, All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// or in the the file LICENSE-2.0.txt in the top level sourcedirectory
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <ml.h>

#define PREC 1e-14

double_matrix::double_matrix(int m, int n) {
  M_ = new double[m * n];
  m_ = m;
  n_ = n;
}

double_matrix::~double_matrix() {
  delete []M_;
  M_ = nullptr;
}

double double_matrix::get_entry(int i, int j) {
  return M_[index(i,j)];
}

void double_matrix::set_entry(int i, int j, double x) {
  M_[index(i,j)] = x;
}

bool double_matrix::swap_rows(int i, int j) {
  double t, s;
  for (int k = 0; k < num_columns(); k++) {
    t = get_entry(i, k);
    s = get_entry(j, k);
    set_entry(i, k, s);
    set_entry(j, k, t);
  }
  return true;
}

bool double_matrix::swap_columns(int i, int j) {
  double t, s;
  for (int k = 0; k < num_rows(); k++) {
    t = get_entry(k, i);
    s = get_entry(k, j);
    set_entry(k, i, s);
    set_entry(k, j, t);
  }
  return true;
}

// add a * row i to row j
bool double_matrix::add_row_times_scaler_to_row(int i, int j, double a) {
  double s;
  for (int k = 0; k < num_columns(); k++) {
    s = get_entry(j, k) + a * get_entry(i, k);
    set_entry(j, k, s);
  }
  return true;
}

// add a * column i to column j
bool double_matrix::add_column_times_scaler_to_column(int i, int j, double a) {
  double s;
  for (int k = 0; k < num_rows(); k++) {
    s = get_entry(k, j) + a * get_entry(k, i);
    set_entry(k, j, s);
  }
  return true;
}

bool double_matrix_multiply(double_matrix& A, double_matrix& B, double_matrix* C) {
  if (A.num_columns() != B.num_rows() || A.num_rows() != C->num_rows() ||
      A.num_columns() != C->num_columns())
    return false;

  double a;
  for (int i = 0; i < A.num_rows(); i++) {
    for (int j = 0; j < B.num_columns(); j++) {
      a = 0.0;
      for (int k = 0; k < A.num_columns(); k++) {
        a += A.get_entry(i, k) * B.get_entry(k, j);
      }
      C->set_entry(i, j, a);
    }
  }
  return true;
}

bool double_matrix::multiply_row_by_scalar(int i, double s) {
  for (int j = 0; j < num_columns(); j++) {
    set_entry(i, j, s * get_entry(i, j));
  }
  return true;
}

bool double_matrix::multiply_column_by_scalar(int i, double s) {
  for (int j = 0; j < num_rows(); j++) {
    set_entry(j, i, s * get_entry(j, i));
  }
  return true;
}

void double_matrix::make_zero_matrix() {
  for (int i = 0; i < num_rows(); i++) {
    for (int j = 0; j < num_columns(); j++) {
      set_entry(i, j, 0.0);
    }
  }
}

void double_matrix::make_identity_matrix() {
  for (int i = 0; i < num_rows(); i++) {
    for (int j = 0; j < num_columns(); j++) {
      if (i == j)
        set_entry(i, j, 1.0);
      else
        set_entry(i, j, 0.0);
    }
  }
}

void double_matrix::print_matrix() {
  for (int i = 0; i < num_rows(); i++) {
    for (int j = 0; j < num_columns(); j++) {
      printf("%8.4f ", get_entry(i, j));
    }
    printf("\n");
  }
}

bool double_matrix::is_equal(double_matrix& A) {
  if (num_rows() != A.num_rows() || num_columns() != A.num_columns())
    return false;
  for (int i = 0; i < A.num_rows(); i++) {
    for (int j = 0; j < A.num_columns(); j++) {
      if ((fabs(get_entry(i, j) - A.get_entry(i, j)) > PREC)) {
printf("FAIL: %d, %d; %8.4f, %8.4f\n", i, j, get_entry(i, j), A.get_entry(i, j));
        return false;
      }
    }
  }
  return true;
}

bool double_matrix_copy(double_matrix& A, double_matrix* C) {
  if (A.num_columns() != C->num_rows() || A.num_columns() != C->num_columns())
    return false;

  for (int i = 0; i < A.num_rows(); i++) {
    for (int j = 0; j < A.num_columns(); j++) {
      C->set_entry(i, j, A.get_entry(i, j));
    }
  }
  return true;
}

// Find non-zero entry in column j starting at row i, return row
//  -1 means none found
int non_zero_position(double_matrix& A, int i, int j) {
  for (int k = i; k < A.num_rows(); k++) {
    if (A.get_entry(k, j) != 0.0)
      return k;
  }
  return -1;
}

bool double_matrix_invert(double_matrix& In, double_matrix* B) {
  if (In.num_columns() != In.num_rows() || B->num_columns() != B->num_rows() ||
      In.num_rows() != B->num_rows())
    return false;
  double_matrix A(In.num_rows(), In.num_columns());
  double_matrix_copy(In, &A);

  // B will become the inverse
  B->make_identity_matrix();
  double_matrix E(A.num_rows(), A.num_columns());
  double_matrix T(A.num_rows(), A.num_columns());

  // Row reduction so lower triangle is 0 and diagonal elements are 1
  double a;
  for (int i = 0; i < A.num_rows(); i++) {
    int l = non_zero_position(A, i, i);
    if (l < 0)
      return false;
    A.swap_rows(l, i);
    E.make_identity_matrix();
    E.swap_rows(l, i);
    if (!double_matrix_multiply(E, *B, &T))
      return false;
    if (!double_matrix_copy(T, B))
      return false;
    for (int k = (i + 1); k < A.num_rows(); k++) {
      a = - A.get_entry(k, i) / A.get_entry(i, i);
      A.add_row_times_scaler_to_row(i, k, a);
      E.make_identity_matrix();
      E.set_entry(k, i, a);
      if (!double_matrix_multiply(E, *B, &T))
        return false;
      if (!double_matrix_copy(T, B))
        return false;
    }
  }

  // now get previous triangle to be 0
  for (int i = 1; i < A.num_rows(); i++) {
    if (A.get_entry(i, i) == 0.0)
      return false;
    for (int k = (i - 1); k >= 0; k--) {
      a = - A.get_entry(k, i) / A.get_entry(i, i);
      A.add_row_times_scaler_to_row(i, k, a);
      E.make_identity_matrix();
      E.set_entry(k, i, a);
      if (!double_matrix_multiply(E, *B, &T))
        return false;
      if (!double_matrix_copy(T, B))
        return false;
    }
  }

  // Multiply by inverse of diagonals
  for (int i = 0; i < A.num_rows(); i++) {
   B->multiply_row_by_scalar(i, 1.0 / A.get_entry(i, i));
  }
  return true;
}

bool double_matrix_transpose(double_matrix& A, double_matrix* B) {
  if (A.num_columns() != B->num_rows() || B->num_columns() != A.num_rows())
    return false;
  double t;
  for (int i = 0; i < A.num_rows(); i++) {
    for (int j = 0; j < A.num_columns(); j++) {
      t = A.get_entry(i, j);
      B->set_entry(j, i, t);
    }
  }
  return true;
}
