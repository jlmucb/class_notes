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
#include <sys/types.h>

// Expectation

double expectation(int num_samp1, double* s1, double* p1) {
  double t = 0.0;

  for (int i = 0; i < num_samp1; i++) {
    t += s1[i] * p1[i];
  }
  return t;
}

// Variance

double variance(int num_samp1, double* s1, double* p1, double expectation) {
  double t = 0.0;
  double s;

  for (int i = 0; i < num_samp1; i++) {
    s = s1[i] - expectation;
    t += s * s * p1[i];
  }
  return t;
}

bool expectation_variance_from_joint(int num_samp1, double* s1, int num_samp2, double* s2,
                          double* pr, double* e1, double* v1, double* e2, double* v2) {

  double* p1 = new double[num_samp1];
  for (int i = 0; i < num_samp1; i++) {
    p1[i] = 0.0;
    for (int j = 0; j < num_samp2; j++) {
      p1[i] += pr[joint_index(i, j, num_samp1, num_samp2)];
    }
  }
  double* p2 = new double[num_samp2];
  for (int i = 0; i < num_samp2; i++) {
    p2[i] = 0.0;
    for (int j = 0; j < num_samp1; j++) {
      p2[i] += pr[joint_index(i, j, num_samp1, num_samp2)];
    }
  }
  *e1 = expectation(num_samp1, s1, p1);
  *v1 = variance(num_samp1, s1, p1, *e1);
  *e2 = expectation(num_samp2, s2, p2);
  *v2 = variance(num_samp2, s2, p2, *e2);
  delete []p1;
  delete []p2;
  return true;
}

bool expectation_variance(int num_samp1, double* s1, double* p1,
                          double* exp, double* var) {
  double e = expectation(num_samp1, s1, p1);
  double v = variance(num_samp1, s1, p1, e);

  *exp = e;
  *var = v;
  return true;
}

//  Correlation

double correlation(int num_samp1, double* s1,
                   int num_samp2, double* s2, double* pr) {
  double t = 0.0;
  double t1, t2, t3;
  double e1, e2;
  double v1, v2;


  expectation_variance_from_joint(num_samp1, s1, num_samp2, s2,
                          pr, &e1, &v1, &e2, &v2);
  for (int i = 0; i < num_samp1; i++) {
    t1 = s1[i] - e1;
    for (int j = 0; j < num_samp2; j++) {
      t2 = s2[j] - e2;
      t3 = pr[joint_index(i, j, num_samp1, num_samp2)];
      t += t1 * t2 * t3;
    }
  }
  return t / sqrt(v1 * v2);
}

//  Entropy

double ent_step(double a) {
  if (a == 0.0 || a == 1.0)
    return 0.0;
  return a * log(a);
}

double entropy(int num_samp, double* pr) {
  double t = 0.0;

  for (int i = 0; i < num_samp; i++) {
    t += ent_step(pr[i]);
  }
  return - t / log(2.0);
}

// print joint probbilities

void print_joint_probability(int n1, int n2, double* pr) {
  for (int i = 0; i < n1; i++) {
    for (int j = 0; j < n2; j++) {
      printf("%.3lf ", pr[joint_index(i, j, n1, n2)]);
    }
    printf("\n");
  }
}

// Joint entropy

double joint_entropy(int num_samp1, int num_samp2, double* pr) {
  double t = 0.0;

  for (int i = 0; i < num_samp1; i++) {
    for (int j = 0; j < num_samp2; j++) {
      t += ent_step(pr[joint_index(i, j, num_samp1, num_samp2)]);
    }
  }
  return - t / log(2.0);
}

double integrate(double (*f)(double& x), double& a, double& b, int N) {
  double s = 0.0;
  double d = (b - a) / ((double)N);
  double t = a + d/2;

  for (int i = 0; i < N; i++) {
    s += d * f(t);
    t += d;
  }
  return s;
}

double sample_from_uniform_distribution() {
  int fd = open("/dev/urandom", O_RDONLY);
  uint64_t val;
  double rval;

  read(fd, (char*) &val, sizeof(long unsigned));
  val &= ~(0x3L<<62);
  rval = ((double)val)/((double) (1L<<62));
  close(fd);
  return rval;
}

double rms_error(int m, vector<double>* x, double* y, vector<double>& w,
                double (*f)(vector<double>& x,vector<double>& w)) {
  double t = 0.0;
  double a;

  for (int i = 0; i < m; i++) {
    a = y[i] - f(x[i], w);
    t += a * a;
  }
  return sqrt(t)/((double)m);
}

double gaussian_with_parameters(double& x, double mu, double sigma) {
  double t = (x - mu) / sigma;
  return (1.0 / (sqrt(2.0 * pi) * sigma)) * exp(- (t * t) / 2.0);
}

double basic_gaussian(double& x) {
  return (1.0 / sqrt(2.0 * pi)) * exp(- x * x / 2.0);
}

const int N_Inv = 40000;
bool Inverse_intialized = false;
double Cdf_inv[N_Inv];
double Lower_range = -10.0;
double Upper_range = 10.0;

void calculate_cdf_table() {
  double delta = (Upper_range - Lower_range) / ((double)N_Inv);
  double current_base = Lower_range;
  double current_area = 0.0;
  double arg;

  for (int i = 0; i < N_Inv; i++) {
    Cdf_inv[i] = current_area;
    arg = current_base + delta / 2.0;
    current_area += delta * basic_gaussian(arg);
    current_base += delta;
  }
}

double gaussian_inverse_cdf(double x) {
  if (!Inverse_intialized) {
    Inverse_intialized = true;
    calculate_cdf_table();
  }
  if (x >= 1.0) {
    return Upper_range;
  }
  if (x <= 0.0) {
    return Lower_range;
  }
  double delta = (Upper_range - Lower_range) / ((double)N_Inv);
  int index = 0;
  while (Cdf_inv[index] < x) {
    index++;
  }
  index--;

  if (index < 0)
    return Lower_range;
  if (index >= N_Inv)
    return Lower_range;
  double current_base = Lower_range + ((double) index) * delta;
  double t1 = (x - Cdf_inv[index]) / (Cdf_inv[index + 1] - Cdf_inv[index]);
  return current_base + t1 * delta;
}

// F(X<=x) = CDF(N(0,1))
// Sample u uniformly.  x= F^(-1)(u)
double sample_from_gaussian_distribution() {
  double a = sample_from_uniform_distribution();
  return gaussian_inverse_cdf(a);
}

bool Inverse_intialized_with_parameters = false;
double Cdf_inv_with_parameters[N_Inv];

void clear_gaussian_cdf_with_parameters() {
  Inverse_intialized_with_parameters = false;
}

void calculate_cdf_table_with_parameters(double mu, double sigma) {
  double Lower_range_with_parameters = Lower_range + mu;
  double Upper_range_with_parameters = Upper_range + mu;
  
  double delta = (Upper_range_with_parameters - Lower_range_with_parameters) / ((double)N_Inv);
  double current_base = Lower_range_with_parameters;
  double current_area = 0.0;
  double arg;

  for (int i = 0; i < N_Inv; i++) {
    Cdf_inv_with_parameters[i] = current_area;
    arg = current_base + delta / 2.0;
    current_area += delta * gaussian_with_parameters(arg, mu, sigma);
    current_base += delta;
  }
}

double gaussian_inverse_cdf_with_parameters(double x, double mu, double sigma) {
  double Lower_range_with_parameters = Lower_range + mu;
  double Upper_range_with_parameters = Upper_range + mu;
  
  if (!Inverse_intialized_with_parameters) {
    Inverse_intialized_with_parameters = true;
    calculate_cdf_table_with_parameters(mu, sigma);
  }
  if (x >= 1.0) {
    return Upper_range_with_parameters;
  }
  if (x <= 0.0) {
    return Lower_range_with_parameters;
  }
  double delta = (Upper_range_with_parameters - Lower_range_with_parameters) / ((double)N_Inv);
  int index = 0;
  while (Cdf_inv_with_parameters[index] < x) {
    index++;
  }
  index--;
  if (index < 0)
    return Lower_range_with_parameters;
  if (index >= (N_Inv - 1)) {
    return Upper_range_with_parameters;
  }

  double current_base = Lower_range_with_parameters + ((double) index) * delta;
  // double t1 = (x - Cdf_inv_with_parameters[index]) / (Cdf_inv_with_parameters[index + 1] - Cdf_inv_with_parameters[index]);
  double t1 = 0.0;
  return current_base + t1 * delta;
}

double sample_from_gaussian_distribution_with_parameters(double mu, double sigma) {
  double a = sample_from_uniform_distribution();
  return gaussian_inverse_cdf_with_parameters(a, mu, sigma);
}

bool MLE_Estimate(int n, double* pts, double* mu, double* sigma) {
  double mu_computed = 0.0;
  double sigma_computed = 0.0;
  double t;

  for (int i = 0; i < n; i++) {
    mu_computed += pts[i];
  }
  mu_computed /= ((double) n);
  for (int i = 0; i < n; i++) {
    t = mu_computed - pts[i];
    sigma_computed += t * t;
  }
  sigma_computed /= ((double) n);
  sigma_computed = sqrt(sigma_computed);

  *mu = mu_computed;
  *sigma = sigma_computed;
  return true;
}
