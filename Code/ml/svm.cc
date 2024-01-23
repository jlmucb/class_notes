//
// Copyright 2018 John Manferdelli, All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// or in the the file LICENSE-2.0.txt in the top level source directory
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include <stdio.h>
#include <string>
#include <vector>
#include <ml.h>

// f(x) = sum over i alpha[i](x[i], x) + b
double f(vector<double> x, int n, double* alpha, double b, svm_training_data* data) {
  double sum = 0.0;

  for (int i = 0; i < n; i++) {
    // Could replace dot product with kernel function
    sum +=  alpha[i] * data[i].outcome_ * dot_product(x, data[i].input_);
  }
  sum += b;
  return sum;
}

#if 1
// Select optimal second alpha to adjust
int select(int i, int n, double Ei, double* alpha, double b,
           svm_training_data* data) {
  int s;
  double Ej;
  double cur_diff = 0.0;

  if (i == 0)
    s = 1;
  else
    s = 0;
  for (int j = 0; j < n; j++) {
    if (j == i || j == s)
      continue;
    Ej = f(data[j].input_, n, alpha, b, data) - data[j].outcome_;
    if (std::abs(Ei - Ej) > cur_diff) {
      cur_diff = std::abs(Ei - Ej);
      s = j;
    }
  }
  return s;
}

#else

int select(int i, int n, double Ei, double* alpha, double b,
           svm_training_data* data) {
  int s = rand() % n;
  if (s == i) {
    if (i == (n-1))
      s--;
    else
      s++;
  }
  return s;
}
#endif

// Classifier is: w = (sum_i alpha[i]x[i]y[i])
bool calc_w(int n, double* alpha, svm_training_data* td, vector<double>* w) {
  vector<double> out(w->size());

  zero_vector(*w);
  for (int i = 0; i < n; i++) {
    mult_vector_by_const(td[i].input_, td[i].outcome_ * alpha[i], out);
    add_to_vector(out, *w);
  }
  return true;
}

// C: regularization parameter
// t: tolerance
// max_passes: Maximum passes in Platt's SMO
// n: number of training vectors
// training_data: input
// w, b_out: hyperplane coefficients - output
//
// Original optimizaiton problem:
//    g[i]= y[i]{(w/||w||, x[i]) + b/||w||}
//    g = min_{g, w, b} g[i], ||w|| = 1
//    Find max(g) subject to  h[i]= y[i]((w, x[i]) + b) >= g using
//    Lagrange multipliers alpha[0],...,alpha[n-1]
//
// We solve dual:
//    max_{alpha} sum_i alpha[i] - 
//                  1/2 sum_i sum_j y[i] y[j] alpha[i] alpha[j] (x[i], x[j])
//    subject to: sum_i alpha[i] y[i] = 0
//
//    KKT conditions:
//      alpha[i] = 0 --> y[i]((w, x[i]) + b) >= 1
//      alpha[i] = C --> y[i]((w, x[i]) + b) <= 1
//      0 < alpha[i] < C --> y[i]((w, x[i]) + b) = 1
//
// We actually solve dual problem:
//
bool svm_train(double C, double t, int max_passes, int n, svm_training_data* data,
               vector<double>* w, double* b_out) {

  double* alpha = new double[n];
  double* alpha_old = new double[n];
  double L, H, Ei, Ej, b;
  double eta, b1, b2;
  int num_passes = 0;
  int num_changed_alphas = 0;
  int i, j;
  const int crazy_loop_limit = 5000;
  int loop_count = 0;

  // Platt's SMO

  //b_old = 0;
  b = 0;
  for (i = 0; i < n; i++) {
    alpha[i] = 0;
    alpha_old[i] = 0;
  }

  while (num_passes < max_passes && loop_count++ < crazy_loop_limit) {
    num_changed_alphas = 0;

    for (i = 0; i < n; i++) {

      double f_i_evaluated = f(data[i].input_, n, alpha, b, data);
      Ei = f_i_evaluated - data[i].outcome_;

      // Does Ei violate KKT?
      if (((data[i].outcome_ * Ei) < -t && alpha[i] < C) ||
           (data[i].outcome_ * Ei > t && alpha[i] > 0)) {
        j = select(i, n, Ei, alpha, b, data);
        double f_j_evaluated = f(data[j].input_, n, alpha, b, data);
        Ej= f_j_evaluated - data[j].outcome_;

        alpha_old[i] = alpha[i];
        alpha_old[j] = alpha[j];
        if (data[i].outcome_ != data[j].outcome_) {
          L = my_max(0.0, alpha[j] - alpha[i]);
          H = my_min(C, C + alpha[j] - alpha[i]);
        } else {
          L = my_max(0.0, alpha[i] + alpha[j] - C);
          H = my_min(C, alpha[i] + alpha[j]);
        }
        if (L == H)
          continue;
        eta = 2 * dot_product(data[i].input_, data[j].input_) -
              dot_product(data[i].input_, data[i].input_) -
              dot_product(data[j].input_, data[j].input_);
        if (eta >= 0)
          continue;
        alpha[j] = alpha[j] - data[j].outcome_ * (Ei - Ej) / eta;
        if (alpha[j] > H) {
          alpha[j] = H;
        } 
        if (alpha[j] < L) {
          alpha[j] = L;
        }
        if (std::abs(alpha[i]-alpha_old[j]) < .000001)
          continue;

        alpha[i] = alpha[i] + data[i].outcome_ * data[j].outcome_ * (alpha_old[j] - alpha[j]);
        b1 = b - Ei - data[i].outcome_ * (alpha[i] - alpha_old[i]) * 
             dot_product(data[i].input_, data[i].input_) - 
             data[j].outcome_ * (alpha[j] - alpha_old[j]) *
             dot_product(data[i].input_, data[j].input_);
        b2 = b - Ej - data[i].outcome_ * (alpha[i] - alpha_old[i]) * 
             dot_product(data[i].input_, data[j].input_) - 
             data[j].outcome_ * (alpha[j] - alpha_old[j]) *
             dot_product(data[j].input_, data[j].input_);
        if (0 < alpha[i] && alpha[i] < C) {
          b = b1;
        } else if (0 < alpha[j] && alpha[j] < C) {
          b = b2;
        } else {
          b = (b1 + b2) / 2.0;
        }
#ifdef DEBUG
        printf("b: %8.4f, b1: %8.4f, b2: %8.4f\n", b, b1, b2);
        printf("new alpha[%d]: %8.4f, new alpha[%d]: %8.4f\n", i, alpha[i], j, alpha[j]);
#endif
        num_changed_alphas++;
      }
    }
  if (num_changed_alphas == 0)
    num_passes++;
  else
    num_passes = 0;
#ifdef DEBUG
  printf("num_changed_alphas: %d, num_passes: %d\n", num_changed_alphas, num_passes);
#endif
  }

  calc_w(n, alpha, data, w);
  *b_out = b;

  double normalizer = 0.0;

  for (int k = 0; k < (int)w->size(); k++) {
    normalizer += (*w)[k] * (*w)[k];
  }
  normalizer = sqrt(normalizer);
  divide_vector_by_scalar(normalizer, *w);
  *b_out /= normalizer;

#ifdef DEBUG
printf("\nLoops: %d\n\n", loop_count);
#endif
  delete []alpha;
  delete []alpha_old;

  return true;
}

double svm_predict(vector<double>& w, double& b, vector<double>& x) {
  double  sum = dot_product(w, x);

  sum += b;
#ifdef DEBUG
  printf("\tpredict sum (b:%10.4f)  %8.4f\n", b, sum);
#endif
  if (sum >= 0)
    return 1.0;
  return -1.0;
}
