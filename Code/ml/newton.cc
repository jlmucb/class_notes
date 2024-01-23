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

//  Newton's Method
//  x(i+1) = x(i) - (f(x(i))/f'(x(i)))

void scalar_multiply(double a, vector<double> b, vector<double>* out) {
  for (int i = 0; i < b.size(); i++)
    (*out)[i] = a * b[i];
}

void vector_add(vector<double> a, vector<double> b, vector<double>* out) {
  for (int i = 0; i < a.size(); i++)
    (*out)[i] = a[i] + b[i];
}

//
//  new_x = (new_x1,...,new_xn), x = (x1,...,xn),
//  new_xi = ((f_prime(x0), x0i)-f(x0)) * f_prime(x0)_i / (sum_j f_prime(x0)_i^2)
void newton_step(vector<double> x, double (*f)(vector<double>),
    void (*f_prime)(vector<double>, vector<double>*), vector<double>* out) {
  vector<double> t1(x.size());
  vector<double> t2(x.size());
  f_prime(x, &t1);
  double a = f(x);
  double b = dot_product(t1, x);
  double d = dot_product(t1, t1);
  double r = (b - a) / d;
  for(int j = 0; j < x.size(); j++) {
    (*out)[j] = t1[j] * r;
  }
}

bool newton(vector<double> x, double (*f)(vector<double>),
      void (*f_prime)(vector<double>, vector<double>*),
      double prec, int max_iter, vector<double>* out) {
  vector<double> x_next(x.size());
  int num_iter = 0;

  copy_vector(x, x_next);
  copy_vector(x, *out);
  while (num_iter++ < max_iter) {
    double t = f(x_next);
    if (fabs(t) < prec) {
      return true;
    }
    newton_step(x_next, f, f_prime, out);
    copy_vector(*out, x_next);
  }
  return true;
}

