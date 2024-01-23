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

// If X = [ x[0]^T, x[1]^T,...,x[m-1]^T]^T
// vec(y) = [y[0], ..., y[m-1]]^t
// h_theta(x[i])= (x[i])^T theta
// J(theta) = 1/2 (Xtheta - y)^T (Xtheta-y)
// theta = (X^TX)^(-1) X^T vec(y)
//

bool least_squares(int m, vector<double> y, vector<double>* x, vector<double>* w) {
  double_matrix X(m, x[0].size());
  double_matrix XT(x[0].size(), m);
  double_matrix C(x[0].size(), x[0].size());
  double_matrix CInverse(x[0].size(), x[0].size());
  double_matrix D(x[0].size(), m);

  for (int i = 0; i < m; i++) {
    for (int j = 0; j < (int)x[0].size(); j++) {
      X.set_entry(i, j, (x[i])[j]);
    }
  }
  double_matrix_transpose(X, &XT);
  double_matrix_multiply(XT, X, &C);
  double_matrix_invert(C, &CInverse);
  double_matrix_multiply(CInverse, XT, &D);
  apply_matrix(D, y, w);
  return true;
}

double x_y_accum(int num_unknowns, int m, int i, vector<double>* x, double* y) {
  double t = 0.0;

  for (int k = 0; k < m; k++) {
    t += y[k]*x[k][i];
  }
  return t;
}

double x1_x2_accum(int num_unknowns, int m, int i, int j, vector<double>* x) {
  double t = 0.0;

  for (int k = 0; k < m; k++) {
    t += x[k][i] * x[k][j];
  }
  return t;
}


// Input: (y(k), x(k)), k = 1,2,...,m
// Output: w  where wx is least squares approximation, last element of x is 1 (const)
bool gaussian_least_squares(int m, double* y, vector<double>* x, vector<double>* w) {
  int num_unknowns = (int)x[0].size();
  double* y_t = new double[num_unknowns];
  vector<double>* x_t = new vector<double>[num_unknowns];
  bool ret = true;

  // set up equations
  for (int i = 0; i < num_unknowns; i++) {
    y_t[i] = x_y_accum(num_unknowns, m, i, x, y);
    for (int j = 0; j < num_unknowns; j++) {
      x_t[i].push_back(x1_x2_accum(num_unknowns, m, i, j, x));
    }
  }

#if 1
  printf("\nGaussian least squares equations\n");
  print_equations(num_unknowns, y_t, x_t);
#endif

  // solve the equations
  if(!gaussian_elimination(num_unknowns, y_t, x_t, *w)) {
    ret = false;
    goto done;
  }

done:
  delete []x_t;
  delete []y_t;
  return ret;
}

