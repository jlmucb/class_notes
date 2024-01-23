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

#define DEBUG

double gaussian(double x, double mu, double sigma) {
  double t = (x - mu ) / sigma;
  return (1.0 / (sigma * sqrt(2.0 * pi))) * exp(- t * t / 2.0);
}

// Compute ln(y1,...,ym| mu, sigma, pi)
double log_likelihood(int m, double* y, int K, double* mu, double* sigma, double* p) {
  double a, b;

  a = 0.0;
  for (int i = 0; i < m; i++) {
    b = 0.0;
    for (int j = 0; j < K; j++) {
      b += p[j] * gaussian(y[i], mu[j], sigma[j]);
    }
    a += log(b);
  }
  return a;
}

#ifdef DEBUG
void print_gamma_matrix(double_matrix& gamma, int m, int K) {
  printf("gamma(%d, %d):\n", m, K);
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < K; j++) {
      printf("%6.4lf ", gamma.get_entry(i, j));
    }
    printf("\n");
  }
  printf("\n");
}
#endif

// gamma_i(j) = (p_i(j) gaussian(y[i], mu[j], sigma[j])/(sum_j (p_i[j] * gaussian(y[i], mu[j], sigma[j])
bool compute_E_step(int m, double* y, int K, double* pi_in, double* mu_in,
      double* sigma_in, double_matrix* gamma) {
  double a;

  for(int j = 0; j < K; j++) {
    for (int i = 0; i < m; i++) {
      a = 0.0;
      for (int l = 0; l < K; l++) {
        a += pi_in[l] * gaussian(y[i], mu_in[l], sigma_in[l]);
      }
    gamma->set_entry(i, j, (pi_in[j] * gaussian(y[i], mu_in[j], sigma_in[j])) / a);
    }
  }
  return true;
}

bool compute_M_step(int m, double* y, int K, double_matrix& gamma,
      double* pi_out, double* mu_out, double* sigma_out) {

  double* gamma_d = new double[K];
  double t;

  // compute gamma_d and pi_out
  for (int j = 0; j < K; j++) {
    gamma_d[j] = 0.0;
    for (int i = 0; i < m; i++) {
      gamma_d[j] += gamma.get_entry(i, j);
    }
    pi_out[j] = gamma_d[j] / ((double)m);
  }

  // new mu
  for (int j = 0; j < K; j++) {
    t = 0.0;
    for (int i = 0; i < m; i++) {
      t += gamma.get_entry(i, j) * y[i];
    }
    mu_out[j] = t / gamma_d[j];
  }

  // new covariance
  double r;
  for (int j = 0; j < K; j++) {
    t = 0.0;
    for (int i = 0; i < m; i++) {
      r = y[i] - mu_out[j];
      t += gamma.get_entry(i, j) * r * r ;
    }
    sigma_out[j] = t / gamma_d[j];
  }

  delete []gamma_d;
  return true;
}

// m: # of samples
// K: # of (mu, sigma) pairs
// y[i] are the mixture samples
const int max_iter = 150;
bool gaussian_em(int m, double* y, int K,
      double* pi_in, double* mu_in, double* sigma_in, double prec,
      double* pi_out, double* mu_out, double* sigma_out) {

  double_matrix gamma(m, K);
  double  old_likelihood;
  double  new_likelihood;
  int num_iter = 0;

  old_likelihood = log_likelihood(m, y, K, mu_in, sigma_in, pi_in);
  while (num_iter++ < max_iter) {
#ifdef DEBUG
    printf("Interation %d\n", num_iter);
#endif
    if (!compute_E_step(m, y, K, pi_in, mu_in, sigma_in, &gamma))
      return false;
    if (!compute_M_step(m, y, K, gamma, pi_out, mu_out, sigma_out))
      return false;
    new_likelihood = log_likelihood(m, y, K, mu_out, sigma_out, pi_out);
#ifdef DEBUG
    printf("old likelihood: %8.2lf, new likelihood: %8.2lf\n", old_likelihood, new_likelihood);
#endif
    if (new_likelihood < old_likelihood || fabs(old_likelihood - new_likelihood) < prec) {
      printf("%d iterations, old_likelihood: %.2lf,  new_likelihood: %.2lf\n",
             num_iter, old_likelihood, new_likelihood);
      break;
    }
    old_likelihood = new_likelihood;
    copy_double_array(K, pi_out, pi_in);
    copy_double_array(K, mu_out, mu_in);
    copy_double_array(K, sigma_out, sigma_in);
  }
  return true;
}


