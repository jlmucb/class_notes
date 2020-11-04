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
#include <string>
#include <vector>
#include <ml.h>

//  Simple Gaussian elimination

void print_equations(int num_unknowns, double* y, vector<double>* x) {
  for (int i = 0; i < num_unknowns; i++) {
    printf(" %7.4lf = ", y[i]);
    printvector(x[i]);
    printf("\n");
  }
}

void switch_equation(int num_unknowns, int i, int j, double* y, vector<double>* x) {
  if (i ==j)
    return;
  if (i >= num_unknowns || j >= num_unknowns)
    return;
  double t;
  t = y[i];
  y[i] = y[j];
  y[j] = t;
  for (int k = 0; k < num_unknowns; k++) {
    t= x[i][k];
    x[i][k]= x[j][k];
    x[j][k] = t;
  }
}

// divide equation i by d
void divide_equation(int num_unknowns, int i, double& d, double* y, vector<double>* x) {
  if (i >= num_unknowns)
    return;
  y[i] /= d;
  for (int j = 0; j < num_unknowns; j++) {
    x[i][j] /= d;
  }
}

// subtract equation i from equation j
void subtract_equations(int num_unknowns, int i, int j, double* y, vector<double>* x) {
  if (i >= num_unknowns || j >= num_unknowns)
    return;
  y[j] -= y[i];
  for (int k = 0; k < num_unknowns; k++) {
    x[j][k] -= x[i][k];
  }
}

bool gaussian_elimination(int num_unknowns, double* y,
      vector<double>* x, vector<double>& sol) {
  double* y_t = new double[num_unknowns]; 
  vector<double>* x_t = new vector<double>[num_unknowns];
  bool ret = true;

  for (int i = 0; i < num_unknowns; i++) {
    y_t[i]= y[i];
    for (int j = 0; j < num_unknowns; j++) {
      x_t[i].push_back(x[i][j]);
    }
  }

  // reduce to echelon form
  for (int eq = 0; eq < num_unknowns; eq++) {
    // get 1 in position eq
    // first make sure coeff != 0
    int k = eq;
    for (int i = eq; i < num_unknowns; i++) {
      if (x_t[i][i] != 0.0)
        break;
    }
    if (x_t[eq][eq] == 0.0) {
      ret = false;
      goto done;
    }
    switch_equation(num_unknowns, eq, k, y_t, x_t);
    double d;
    for (int i = eq; i < num_unknowns; i++) {
      d = x_t[i][eq];
      divide_equation(num_unknowns, i, d, y_t, x_t);
    }
    for (int i = (eq + 1); i < num_unknowns; i++) {
      subtract_equations(num_unknowns, eq, i, y_t, x_t);
    }
  }

  // calculate solutions
  double t;
  for (int eq = (num_unknowns - 1); eq >= 0; eq--) {
    t = y_t[eq];
    for (int j = (eq + 1); j < num_unknowns; j++) {
      t -= x_t[eq][j]*sol[j];
    }
    sol[eq] = t;
  }

done:
  delete []y_t;
  delete []x_t;
  return ret;
}

