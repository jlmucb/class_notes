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

#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <ml.h>

using namespace std;
using std::vector;

/*
 * Input: Training data, (x(i), y(i)), i = 1, 2, ..., m
 * Initialize: w_1 = (1/m, 1/m, ..., 1/m)
 * for(t=1 to T) {
 *   Train ht on training data with weight w.
 *   Compute epsilon_t = sum {i:y(i) != h_t} w_{t, i}, beta_t = 1/2 (ln((1-epsilon_t)/epsilon_t)
 *   Update: w_{t+1, i} = w{t,i} exp(-beta_t y(i) h_t(x(i)) 
 *   Normalizer:  w_{t+1,i} = w_{t+1,i} / (sum over j w_{t+1,j})
 * }
 * return H(x)= sign( sum from 1 to T beta_t h_t
 */


bool adaboost_H(vector<double> x, vector<double>* theta, vector<double> beta) {
  double sum = 0.0;
  int nc = beta.size(); // number of classifiers

  for (int j = 0; j < nc; j++) {
    sum += h_logistic(x, theta[j]) * beta[j];
  }
  return sum >= 0.0;
}

// Update: w_{t+1, i} = w{t,i} exp(-beta_t y(i) h_t(x(i)) 
void ada_update(vector<double>& w_old, vector<double>& beta, int y, vector<double>& x,
      vector<double>* theta, vector<double>* w_new) {
  int m = w_old.size();
  for (int j = 0; j < m; j++) {
    (*w_new)[j] = w_old[j] * exp(- beta[j] * y * h_logistic(x, theta[j]));
  }
}

// Compute epsilon_t = sum {i:y(i) != h_t} w_{t, i}, beta_t = 1/2 (ln((1-epsilon_t)/epsilon_t)
void ada_adjustments(int n, int cn, vector<double>* x, int* y, vector<double>& w_old,
      vector<double>& th, vector<double>* epsilon, vector<double>* beta) {
  double sum = 0.0;

  for (int i = 0; i < n; i++) {
    if (y[i] == h_logistic(x[i], th)) {
      sum += w_old[cn];
    }
  }
  (*epsilon)[cn] = sum;
  (*beta)[cn] = 0.5 * log((1.0 - (*epsilon)[cn]) / (*epsilon)[cn]);
}

// Normalizer:  w_{t+1,i} = w_{t+1,i} / (sum over j w_{t+1,j})
void ada_normalize(vector<double>& w) {
  double sum = 0.0;

  for (int j = 0; j < w.size(); j++) {
    sum += w[j];
  }    
  for (int j = 0; j < w.size(); j++) {
    w[j] /= sum;
  }
}

// Adaboost
bool adaboost(int n, vector<double>* x, int* y, vector<double> w,
      int nc, vector<double>* theta, vector<double>* out_beta,
      bool initialized) {
  // There are m features and nc classifiers.
  int m = w.size();
  vector<double> old_w;
  allocate_double_vector(m, old_w);
  vector<double> new_w;
  allocate_double_vector(m, new_w);
  vector<double> epsilon;
  allocate_double_vector(m, epsilon);
  vector<double> beta;
  allocate_double_vector(m, beta);

  if (!initialized) {
    for (int j = 0; j < m; j++) {
      old_w[j] = 1.0 / ((double) m);
    }
  } else {
    copy_vector(w, old_w);
  }

  for (int k = 0; k < nc; k++) {
    ada_adjustments(n, k, x, y, old_w, theta[k], &epsilon, &beta);
    int i = 0; // fix this
    ada_update(old_w, beta, y[i], x[i], theta, &new_w);
    ada_normalize(new_w);
    copy_vector(new_w, old_w);
  }
  copy_vector(beta, *out_beta);
  return true;
}

