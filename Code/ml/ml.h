//
// Copyright 2016 John Manferdelli, All Rights Reserved.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <cmath>
#include <string>

#include <gtest/gtest.h>
#include <gflags/gflags.h>

#ifndef _ML_H_
#define _ML_H_

using std::string;
using std::vector;

typedef unsigned char byte;

const double pi = 3.14159265358979323446;

class svm_training_data {
public:
  vector<double> input_;
  double outcome_;
};

double sigmoid(double x);

inline double dist(vector<double>& a, vector<double>& b) {
  if (a.size() != b.size())
    return -1.0;
  double sum = 0.0;
  for (int i = 0; i < (int)a.size(); i++) {
    double d = a[i] - b[i];
    sum += d * d;
  }
  return sqrt(sum);
}

void allocate_int_vector(int d, vector<int>& out);
void zero_int_vector(vector<int>& out);
void copy_int_vector(vector<int>& from, vector<int>& to);

void allocate_double_vector(int d, vector<double>& vec);
void allocate_double_vectors(int n, int d, vector<double>* in_data);
void zero_double_vectors(int n, vector<double>* vectors);
void copy_double_vectors(int n, vector<double>* in_vectors, vector<double>* out_vectors);

void zero_vector(vector<double>& x);
void copy_vector(vector<double>& in, vector<double>& out);
void add_vector_to_vector(vector<double>& in1, vector<double>& in2, vector<double>& out);
void subtract_vector_from_vector(vector<double>& in1, vector<double>& in2, vector<double>& out);
void add_to_vector(vector<double>& in, vector<double>& out);
void subtract_from_vector(vector<double>& in, vector<double>& out);
void mult_vector_by_const(vector<double>& x, double a, vector<double>& out);
void multiply_vector_by_scalar(double d, vector<double>& out);
void divide_vector_by_scalar(double d, vector<double>& out);

void printvector(vector<double>& a);
bool match_sets(int n, vector<int>& set1, vector<int>& set2);
double my_max(double a, double b);
double my_min(double a, double b);
double dot_product(vector<double>& x, vector<double>& y);
void scale(vector<double>& v);
void probability_scale(vector<double>& v);

void print_double_matrix(int n, double* array);
void print_double_array(int n, double* array);

void gradient_step(double rate, int num_samples, vector<double>* x,
      double* y, vector<double>* w, double* b, bool descent);
int gradient_descent(int max_iter, double prec, double rate, int num_samples,
      vector<double>* x, double* y, vector<double>* w, double* b);
int gradient_ascent(int max_iter, double prec, double rate, int num_samples,
      vector<double>* x, double* y, vector<double>* w, double* b);

int closest_vector(int n, vector<double>* points, vector<double>& pt);
void printclusters(int n, int k, vector<double>* in_data, vector<double>* clusters);
void print_data_set(int n, vector<double>* in_data);
void assign_to_sets(int k, int n, vector<double>* in_data, vector<double>* clusters,
                    vector<int>& set);
bool k_means_with_guess(int k, int n, vector<double>* in_data,
             vector<double>* initial_guess, vector<double>* out_data);
bool k_means(int k, int n, vector<double>* in_data, vector<double>* out_data);

bool svm_train(double C, double t, int max_passes, int n, svm_training_data* data,
               vector<double>* w, double* b_out);
double svm_predict(vector<double>& w, double& b, vector<double>& x);

class double_matrix {
private:
  int m_, n_;
  double* M_;
  int index(int i, int j) {return i * n_ + j;};
public:
  double_matrix(int m, int n);
  ~double_matrix();
  double get_entry(int i, int j);
  void set_entry(int i, int j, double x);
  int num_rows() {return m_;};
  int num_columns() {return n_;};
  bool swap_rows(int i, int j);
  bool swap_columns(int i, int j);
  bool add_row_times_scaler_to_row(int i, int j, double a);
  bool add_column_times_scaler_to_column(int i, int j, double a);
  bool multiply_row_by_scalar(int i, double s);
  bool multiply_column_by_scalar(int i, double s);
  void make_zero_matrix();
  void make_identity_matrix();
  void print_matrix();
  bool is_equal(double_matrix& A);
};

class feature_descriptor {
private:
  string feature_name_;
  int feature_type_;
  int int_feature_top_range_;
  int int_feature_bottom_range_;
  double double_feature_top_range_;
  double double_feature_bottom_range_;

  vector<bool> bool_table_;
  vector<int> int_table_;
  vector<double> double_table_;

  bool bucketized_;
  int num_buckets_;
  double bucket_delta_;
public:
  static const int feature_type_none= 0;
  static const int feature_type_int = 1;
  static const int feature_type_double = 2;
  static const int feature_type_bool = 3;
  
  feature_descriptor(string feature_name, int feature_type);
  ~feature_descriptor();

  string get_feature_name();
  int get_feature_type();
  void copy_feature(feature_descriptor& from);

  int get_int_feature_top_range();
  void set_int_feature_top_range(int feature_top_range);
  int get_int_feature_bottom_range();
  void set_int_feature_bottom_range(int feature_bottom_range);
  double double_bucket_interval_high(int bucket_num);
  double double_bucket_interval_low(int bucket_num);

  double get_double_feature_top_range();
  void set_double_feature_top_range(double feature_top_range);
  double get_double_feature_bottom_range();
  void set_double_feature_bottom_range(double feature_bottom_range);

  bool make_buckets(int num_buckets);
  int get_bucket_number(double x);
  int get_number_of_buckets();

  int get_num_bool_entries();
  int get_num_int_entries();
  int get_num_double_entries();

  bool add_bool_feature_value(bool val);
  bool add_int_feature_value(int val);
  bool add_double_feature_value(double val);

  bool get_bool_feature_value(int entry);
  int get_int_feature_value(int entry);
  double get_double_feature_value(int entry);
};

class feature_table {
private:
  vector<feature_descriptor> features_;
public:
  feature_table();
  ~feature_table();
  int get_num_features();
  int add_feature(string feature_name, int feature_type);
  feature_descriptor* get_feature(int feature_number);
};

class bool_probs {
public:
  double prob_yes_;
  double prob_no_;
};

class int_probs {
public:
  vector<double> prob_;
};

class double_probs {
public:
  vector<double> prob_;
};

class probability_value {
public:
  int feature_type_;
  vector<double> pv_;
  void print_probability_value();
};


class feature_value {
public:
  int feature_type_;
  bool bool_value_;
  int int_value_;
  double double_value_;
  int bucket_;

  void print_feature_value();
};

class bayes_probability_model {
public:
  double prob_yes_;
  double prob_no_;

  // feature table
  feature_table* table_;
  // absolute probabilities
  vector<probability_value> absolute_true_;
  // conditional probabilities
  vector<probability_value> conditional_true_;
  vector<probability_value> conditional_false_;

  bayes_probability_model(feature_table& table);
  ~bayes_probability_model();
  bool compute_bayes_probabilties();
  double naive_bayes_odds_yes(vector<feature_value>& v);
  void print_bayes_model();
};

bool double_matrix_copy(double_matrix& A, double_matrix* C);
bool double_matrix_multiply(double_matrix& A, double_matrix& B, double_matrix* C);
bool double_matrix_invert(double_matrix& A, double_matrix* B);
bool double_matrix_transpose(double_matrix& A, double_matrix* B);

void newton_step(vector<double> x, double (*f)(vector<double>),
      void (*f_prime)(vector<double>, vector<double>* out));
bool newton(vector<double> x, double (*f)(vector<double>),
      vector<double> (*f_prime)(vector<double>), double prec,
      int max_iter, vector<double>* out);
bool newton(vector<double> x, double (*f)(vector<double>),
      void (*f_prime)(vector<double>, vector<double>*),
      double prec, int max_iter, vector<double>* out);

bool calc_grad(int d, double* M, vector<double>& x, vector<double>* grad);
void update_guess(vector<double>& x, double alpha, vector<double>& grad);
bool maximize_form_on_sphere(int d, double* M, double R, double alpha,
            vector<double>* w);
double eval_form(int d, double* M, vector<double>& x);
bool pca(int num_samples, vector<double>* x, vector<double>* out);

void copy_double_array(int n, double* in, double* out);
double gaussian(double x, double mu, double sigma);
bool gaussian_em(int m, double* y, int n,
      double* pi_in, double* mu_in, double* sigma_in, double prec,
      double* pi_out, double* mu_out, double* sigma_out);
double log_likelihood(int m, double* y, int n, double* mu,
      double* sigma, double* p);
bool apply_matrix(double_matrix& A, vector<double> x, vector<double>* y);
bool least_squares(int m, vector<double> y, vector<double>* x, vector<double>* w);
bool gaussian_least_squares(int m, double* y, vector<double>* x, vector<double>* w);
void print_equations(int num_unknowns, double* y, vector<double>* x);
bool gaussian_elimination(int num_unknowns, double* y,
    vector<double>* x, vector<double>& sol);
void print_ml_data(int m, double* y, vector<double>* x);

inline int joint_index(int i, int j, int n1, int n2) {
  return n2 * j + i;
}
void print_joint_probability(int n1, int n2, double* pr);
void print_array(int n, double* array);
double expectation(int num_samp, double* s1, double* p1);
double variance(int num_samp, double* s1, double* p1, double exp);
double correlation(int num_samp1, double* s1,
                   int num_samp2, double* s2, double* pr);

double entropy(int num_samp, double* pr);
double joint_entropy(int num_samp1, int num_samp2, double* p2);
bool expectation_variance(int num_samp1, double* s1, double* p1,
                          double* exp, double* var);
double basic_gaussian(double& x);
double gaussian_with_parameters(double& x, double mu, double sigma);
double integrate(double (*f)(double& x), double& a, double& b, int N = 1000);
double sample_from_uniform_distribution();
double sample_from_gaussian_distribution();
double sample_from_gaussian_distribution_with_parameters(double mu,
                                                         double sigma);
void clear_gaussian_cdf_with_parameters();
double gaussian_inverse_cdf(double x);
double gaussian_inverse_cdf_with_parameters(double x, double mu, double sigma);
bool MLE_Estimate(int n, double* pts, double* mu, double* sigma);

double rms_error(int m, vector<double>* x, double* y, vector<double>& w,
                double (*f)(vector<double>& x,vector<double>& w));

double h_logistic(vector<double>& x, vector<double>& theta);
bool binary_logistic_regression(int n, vector<double>* x, int* y,
      double& rate, vector<double>* theta, bool initialized);
bool logistic_regression(int N, int K, vector<double>* x, int* outcome, double rate, vector<double>* theta);

bool adaboost(int n, vector<double>* x, int* y, vector<double> w,
      int nc, vector<double>* theta, vector<double>* out_beta, bool initialized);
bool adaboost_H(vector<double> x, vector<double>* theta, vector<double> beta);

void soft_max(int K, vector<double>& x, vector<double>* theta, vector<double>* out);

#endif

