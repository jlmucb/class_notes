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

//  Gradient Descent
double J(int num_samples, vector<double>* x, double* y, vector<double>& w, double& b) {
  double a = 0.0;
  double t;

  for (int i = 0; i < num_samples; i++) {
    t = dot_product(w, x[i]) + b - y[i];
    a += t * t;
  }
  return a / (2.0 * ((double)num_samples));
}

void gradient_step(double rate, int num_samples, vector<double>* x, double* y,
        vector<double>* w, double* b, bool descent) {
  vector<double> w_direction(w->size());
  double b_direction = 0.0;
  zero_vector(w_direction);
  double s = 0.0;
  double r;
  double grad_size;
  double scale;

#ifdef DEBUG
  printf("step, incoming w: "); printvector(*w); printf(", b: %8.4f\n", *b);
#endif

  for (int i = 0; i < num_samples; i++) {
    for (int j = 0; j < (int)w->size(); j++) {
      r = (dot_product(x[i], *w) + *b) - y[i];
      s += r * r;
      w_direction[j] += r * x[i][j];
    }
    b_direction += r;
  }
  grad_size = dot_product(w_direction, w_direction) + b_direction * b_direction;
  scale = s / (2.0 * grad_size);
  multiply_vector_by_scalar(rate * scale, w_direction);
  b_direction *= rate * scale;
  if (descent) {
    subtract_from_vector(w_direction, *w);
    *b -= b_direction;
  } else {
    add_to_vector(w_direction, *w);
    *b += b_direction;
  }
#ifdef DEBUG
  printf("step, outgoing w: "); printvector(*w); printf(", b: %8.4f\n\n", *b);
#endif
}

int gradient_descent(int max_iter, double prec, double rate, int num_samples,
      vector<double>* x, double* y, vector<double>* w, double* b) {
  vector<double> w_old(w->size());
  double b_old;
  vector<double> temp(w->size());
  double a;

  // do steps until distance between (new, b) and (old w, b) < prec
  int num_iter = 0;
  while((num_iter++) < max_iter) {
    copy_vector(*w, w_old);
    b_old = *b;
    gradient_step(rate, num_samples, x, y, w, b, true);
    subtract_vector_from_vector(w_old, *w, temp); 
    a = dot_product(temp, temp) + ((b_old - *b) * (b_old - *b));
    if (a < prec)
      return num_iter;
  }
  return -1;
}

int gradient_ascent(int max_iter, double prec, double rate, int num_samples,
      vector<double>* x, double* y, vector<double>* w, double* b) {
  vector<double> w_old(w->size());
  vector<double> temp(w->size());
  double a, b_old;

  // do steps until distance between new and old w, b  and new w, b is < prec
  int num_iter = 0;
  while(num_iter++ < max_iter) {
    copy_vector(*w, w_old);
    b_old = *b;
    gradient_step(rate, num_samples, x, y, w, b, false);
        return num_iter;
    subtract_vector_from_vector(w_old, *w, temp); 
    a = dot_product(temp, temp) + ((b_old - *b) * (b_old - *b));
    if (a < prec)
      break;
  }
  return -1;
}
