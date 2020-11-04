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

// Do general and binary logistic regression.

double h_logistic(vector<double>& x, vector<double>& theta) {
  double t = dot_product(x, theta);
  return sigmoid(t);
}

// theta[0] is constant term.
bool sigmoid_gradient_step(double rate, int num_samples, vector<double>* x,
      int* y, vector<double>* theta) {
  double sum;
  vector<double> grad;
  allocate_double_vector((int)theta->size(), grad);

  for (int j = 0; j < (int)theta->size(); j++) {
    sum = 0.0;
    for (int i = 0; i < num_samples; i++) {
      if (j == 0)
        sum += (((double)y[i]) - h_logistic(x[i], *theta));
      else
        sum += (((double)y[i]) - h_logistic(x[i], *theta)) * x[i][j];
    }
    grad[j] = sum;
    (*theta)[j] += rate * grad[j];
  }
#if 0
  printf("grad: ");
  printvector(grad);
  printf("\n");
#endif
  return true;
}

bool binary_logistic_regression(int n, vector<double>* x, int* y, double& rate, vector<double>* theta, bool initialized) {
  vector<double> old_theta;
  allocate_double_vector(theta->size(), old_theta);
  const int MAX_STEPS = 500;
  const double MARGIN = .0001;
  int steps = 0;
  double old_dist = 0.0;
  double new_dist;

  // Initialize theta
  if (!initialized) {
    for (int j = 0; j < theta->size(); j++) {
      (*theta)[j] = ((double)j);
    }
    scale(*theta);
    (*theta)[0] = 1.0;
  }

  while (steps++ < MAX_STEPS) {
    copy_vector(*theta, old_theta);
    if (!sigmoid_gradient_step(rate, n, x, y, theta))
      return false;
    new_dist = dist(*theta, old_theta);
    if (fabs(old_dist - new_dist) < MARGIN)
      return true;
    old_dist = new_dist;
  }
  return false;
}

// Logistic regression
//   K is number of classifiers, x is an m-dimensional feature vector, theta is an
//   K, m-dimensional parameters for the classifier, pr is m dimensional it is
//   P(y=k|x)

void print_parameters(const char* msg, int K, vector<double>* th) {
  printf("%s: \n", msg);
  for (int k = 0; k < K; k++) {
    printvector(th[k]);
    printf("\n");
  }
  printf("\n");
}

void soft_max(int K, vector<double>& x, vector<double>* theta, vector<double>* out) {
  vector<double> a;
  allocate_double_vector(K, a);
// #define SMT
#ifdef SMT
  for (int k = 0; k < K; k++) {
    a[k] = dot_product(theta[k], x);
  }
  double sum = 0.0;
  for (int j = 0; j < K; j++) {
    sum += exp(a[j]);
  }
  for (int k = 0; k < K; k++) {
    (*out)[k] = exp(a[k]) / sum;
  }
#else
  for (int k = 0; k < K; k++) {
    a[k] = dot_product(theta[k], x);
  }
  for (int k = 0; k < K; k++) {
    (*out)[k] = sigmoid(a[k]);
  }
  probability_scale(*out);
#endif
}

// This calculates the y_{nk} matrix
void calculate_soft_max_predictor(int N, int K, vector<double>* x, vector<double>* theta, vector<double>* y) {
  for (int i = 0; i < N; i++) {
    soft_max(K, x[i], theta, &y[i]);
  }
}

void initialize_T_matrix(int N, int K, int* outcome, vector<double>* T) {
  for (int i = 0; i < N; i++) {
    T[i][outcome[i]] = 1;
  }
}

void calculate_wide_gradient(int N, int M, int K, vector<double>* x,
      vector<double>* y, vector<double>* t, vector<double>* gradient) {
  vector<double> a;
  allocate_double_vector(M, a);
  vector<double> b;
  allocate_double_vector(M, b);

  for (int j = 0; j < K; j++) {
    zero_vector(b);
    for (int i = 0; i < N; i++) {
      copy_vector(x[i], a);
      multiply_vector_by_scalar((y[i][j] - t[i][j]), a);
      add_to_vector(a, b);
    }
    copy_vector(b, gradient[j]);
  }
#if 0
  print_parameters("gradient", K, gradient);
#endif
}

// for class k: w^(r+1) = w^r -rate E_k
void update_parameters(int M, int K, double rate, vector<double>* theta_old,
      vector<double>* gradient,  vector<double>* theta_new) {
    for (int i = 0; i < K; i++) {
      mult_vector_by_const(gradient[i], (-rate), theta_new[i]);
      add_to_vector(theta_old[i], theta_new[i]);
    }
}

bool termination_condition(int M, int K, vector<double>* theta_old, vector<double>* theta_new,
    double& old_dist, double* new_dist) {
  const double MARGIN = 0.0001;
  vector<double> a;
  allocate_double_vector(M, a);
  *new_dist = 0.0;

  for (int i = 0; i < K; i++) {
      subtract_vector_from_vector(theta_new[i], theta_old[i], a);
      *new_dist += sqrt(dot_product(a, a));
  }
  if (fabs(old_dist - *new_dist) < MARGIN)
    return true;
  return false;
}

void copy_parameters(int K, vector<double>* calculated_parameters,  vector<double>* theta) {
  for (int i = 0; i < K; i++) {
    copy_vector(calculated_parameters[i], theta[i]);
  }
}

void initialize_parameters(int M, int K, vector<double>* theta) {
  for (int i = 0; i < K; i++) {
    for (int j = 0; j < M; j++) {
      theta[i][j] = 1.0 / ((double) M);
    }
  }
}

// N is number of training points.
// K is number of classes.
bool logistic_regression(int N, int K, vector<double>* x, int* outcome,
      double rate, vector<double>* theta) {
  const int MAX_STEPS = 1500;
  int steps = 0;
  vector<double>* t = new vector<double>[N];
  vector<double>* y = new vector<double>[N];
  vector<double>* theta_old = new vector<double>[K];
  vector<double>* theta_new = new vector<double>[K];
  vector<double>* gradient = new vector<double>[K];
  int M = x[0].size();  // number of features
  bool ret = false;

  for (int j = 0; j < N; j++) {
    allocate_double_vector(K, t[j]);
    allocate_double_vector(K, y[j]);
  }
  for (int j = 0; j < K; j++) {
    allocate_double_vector(M, theta_old[j]);
    allocate_double_vector(M, theta_new[j]);
    allocate_double_vector(M, gradient[j]);
  }

  // Initialize theta and T-matrix.
  initialize_parameters(M, K, theta_old);
  initialize_T_matrix(N, K, outcome, t);
  double old_dist = 0.0;
  double new_dist = 0.0;

  while (steps++ < MAX_STEPS) {
    calculate_soft_max_predictor(N, K, x, theta_old, y);
#if 0
    printf("After %d steps\n", steps - 1);
    print_parameters("Estimated parameters", K, theta_old);
    print_parameters("y likelihood", N, y);
#endif
    calculate_wide_gradient(N, M, K, x, y, t, gradient);
    update_parameters(M, K, rate, theta_old, gradient, theta_new);
    if (termination_condition(M, K, theta_old, theta_new, old_dist, &new_dist)) {
#if 1
      printf("Termination condition met %8.4lf, %8.4lf %d steps\n", old_dist, new_dist, steps);
#endif
      ret = true;
      goto done;
    }
#if 0
    printf("Termination condition failed %8.4lf, %8.4lf\n", old_dist, new_dist);
#endif
    old_dist = new_dist;
    copy_parameters(K, theta_new,  theta_old);
  }

done:
  if (ret) {
    copy_parameters(K, theta_new,  theta);
  }
#if 0
    print_parameters("final parameters", K, theta);
#endif
  delete []y;
  delete []t;
  delete []gradient;
  delete []theta_old;
  delete []theta_new;
  return ret;
}

