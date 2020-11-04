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

double sigmoid(double x) {
  return 1.0 / (1.0 + exp(-x));
}

double dot_product(vector<double>& x, vector<double>& y) {
  double sum = 0.0;

  for (int i = 0; i < (int)x.size(); i++) {
    sum += x[i] * y[i];
  }
  return sum;
}

void scale(vector<double>& v) {
  double s = sqrt(dot_product(v, v));
  for (int j = 0; j < v.size(); j++)
    v[j] /= s;
}

void probability_scale(vector<double>& v) {
  double sum = 0.0;

  for(int j = 0; j < v.size(); j++)
    sum += v[j];
  for(int j = 0; j < v.size(); j++)
    v[j] /= sum;
}

void zero_vector(vector<double>& x) {
  for (int i = 0; i < (int)x.size(); i++)
      x[i]= 0.0;
}

void copy_vector(vector<double>& in, vector<double>& out) {
  for (int i = 0; i < (int)in.size(); i++)
      out[i] = in[i];
}

void add_to_vector(vector<double>& in, vector<double>& out) {
  for (int i = 0; i < (int)in.size(); i++)
      out[i] += in[i];
}

void subtract_from_vector(vector<double>& in, vector<double>& out) {
  for (int i = 0; i < (int)in.size(); i++)
      out[i] -= in[i];
}

void divide_vector_by_scalar(double d, vector<double>& out) {
  for (int i = 0; i < (int)out.size(); i++)
      out[i] /= d;
}

void multiply_vector_by_scalar(double d, vector<double>& out) {
  for (int i = 0; i < (int)out.size(); i++)
      out[i] *= d;
}

void mult_vector_by_const(vector<double>& x, double a, vector<double>& out) {
  for (int i = 0; i < (int)x.size(); i++)
    out[i] = a * x[i];
}

void add_vector_to_vector(vector<double>& in1, vector<double>& in2, vector<double>& out) {
  for (int i = 0; i < (int)in1.size(); i++)
    out[i] = in1[i] + in2[i];
}

void subtract_vector_from_vector(vector<double>& in1, vector<double>& in2, vector<double>& out) {
  for (int i = 0; i < (int)in1.size(); i++)
    out[i] = in1[i] - in2[i];
}

void allocate_double_vector(int d, vector<double>& vec) {
  for (int j = 0; j < d; j++) {
    vec.push_back(0.0);
  }
}

void allocate_double_vectors(int n, int d, vector<double>* vectors) {
  for (int i = 0; i < n; i++) {
    allocate_double_vector(d,vectors[i]);
  }
}

void print_double_array(int n, double* array) {
  for (int i = 0; i < n; i++) {
    printf("%10.4lf ", array[i]);
    if ((i%8) == 7)
      printf("\n");
  }
}

void print_double_matrix(int n, double* a) {
  for (int i = 0; i < n; i++) {
    print_double_array(n, &a[i * n]);
    printf("\n");
  }
}

double my_max(double a, double b) {
  if (a > b)
    return a;
  return b;
}

double my_min(double a, double b) {
  if (a > b)
    return b;
  return a;
}

void allocate_vector(int d, vector<int>& out) {
  for (int i = 0; i < d; i++) {
      out.push_back(0);
  }
}

void zero_int_vector(vector<int>& out) {
  for (int i = 0; i < (int)out.size(); i++) {
      out[i] = 0;
  }
}

void copy_int_vector(vector<int>& from, vector<int>& to) {
  for (int i = 0; i < (int)from.size(); i++) {
      to[i] = from[i];
  }
}

void zero_double_vectors(int n, vector<double>* out_data) {
  int d = (int)out_data[0].size();
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < d; j++) {
      out_data[i][j] = 0.0;
    }
  }
}

void copy_double_vectors(int n, vector<double>* in_data, vector<double>* out_data) {
  int d = (int)in_data[0].size();
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < d; j++)
      out_data[i][j] = in_data[i][j];
  }
}

int closest_vector(int num_centers, vector<double>* centers, vector<double>& pt) {
  int nearest = 0;
  double my_dist;
  double closest_dist = dist(pt, centers[0]);

  for (int i = 1; i < num_centers; i++) {
    my_dist = dist(pt, centers[i]);
    if (my_dist < closest_dist) {
      closest_dist = my_dist;
      nearest = i;
    }
  }
  return nearest;
}


void allocate_int_vector(int d, vector<int>& out) {
  for (int i = 0; i < d; i++) {
    out.push_back(0);
  }
}

void printvector(vector<double>& a) {
  for (int i = 0; i < (int)a.size(); i++) {
    printf("%8.4f ", a[i]);
  }
}

void printclusters(int n, int k, vector<double>* in_data, vector<double>* clusters) {
  vector<int> set;
  allocate_int_vector(n, set);
  assign_to_sets(k, n, in_data, clusters, set);
  
  for (int i = 0; i < k; i++) {
    printf("Cluster %d at ", i);
    printvector(clusters[i]);
    printf(":");
    for (int j = 0; j < n; j++) {
      if (set[j] == i)
      printf(" %d ", j);
    }
    printf("\n");
  }
}

void print_data_set(int n, vector<double>* in_data) {
  for (int i = 0; i < n; i++) {
    printf("\tPoint %3d: ", i);
    printvector(in_data[i]);
    printf("\n");
  }
}

void assign_to_sets(int k, int n, vector<double>* in_data, vector<double>* clusters, vector<int>& set) {
  for (int j = 0; j < n; j++) {
    set[j] = closest_vector(k, clusters, in_data[j]);
  }
}

bool match_sets(int n, vector<int>& set1, vector<int>& set2) {
  for (int j = 0; j < n; j++) {
    if (set1[j] != set2[j])
      return false;
  }
  return true;
}

void copy_double_array(int n, double* in, double* out) {
  for (int i = 0; i < n; i++)
    out[i] = in[i];
}

bool apply_matrix(double_matrix& A, vector<double> x, vector<double>* y) {
  if ((int)x.size() != A.num_columns() || (int)y->size() != A.num_rows())
    return false;
  double a;

  for (int i = 0; i < A.num_rows(); i++) {
    a= 0.0;
    for (int j = 0; j < A.num_columns(); j++) {  
      a += A.get_entry(i, j) * x[j];
    }
    (*y)[i] = a;
  }
  return true;
}

feature_descriptor::feature_descriptor(string feature_name, int feature_type) {
  feature_name_ = feature_name;
  feature_type_ = feature_type;
  bucketized_ = false;
}

feature_descriptor::~feature_descriptor() {
}

void feature_descriptor::copy_feature(feature_descriptor& from){
  feature_name_ = from.feature_name_;
  feature_type_ = from.feature_type_;
  int_feature_top_range_ = from.int_feature_top_range_;
  int_feature_bottom_range_ = from.int_feature_top_range_;
  double_feature_top_range_ = from.double_feature_top_range_;
  double_feature_bottom_range_ = from.double_feature_bottom_range_;
}

string feature_descriptor::get_feature_name() {
  return feature_name_;
}

int feature_descriptor::get_feature_type() {
  return feature_type_;
}

int feature_descriptor::get_int_feature_top_range() {
  return int_feature_top_range_;
}

void feature_descriptor::set_int_feature_top_range(int feature_top_range) {
  int_feature_top_range_ = feature_top_range;
}

int feature_descriptor::get_int_feature_bottom_range() {
  return int_feature_bottom_range_;
}

void feature_descriptor::set_int_feature_bottom_range(int feature_bottom_range) {
  int_feature_bottom_range_ = feature_bottom_range;
}

double feature_descriptor::get_double_feature_top_range() {
  return double_feature_top_range_;
}

void feature_descriptor::set_double_feature_top_range(double feature_top_range) {
  double_feature_top_range_ = feature_top_range;
}

double feature_descriptor::get_double_feature_bottom_range() {
  return double_feature_bottom_range_;
}

void feature_descriptor::set_double_feature_bottom_range(double feature_bottom_range) {
  double_feature_bottom_range_ = feature_bottom_range;
}

bool feature_descriptor::add_bool_feature_value(bool val) {
  if (feature_type_ != feature_type_bool)
    return false;
  bool_table_.push_back(val);
  return true;
}

bool feature_descriptor::add_int_feature_value(int val) {
  if (feature_type_ != feature_type_int)
    return false;
  int_table_.push_back(val);
  return true;
}

bool feature_descriptor::add_double_feature_value(double val) {
  if (feature_type_ != feature_type_double)
    return false;
  double_table_.push_back(val);
  return true;
}

feature_table::feature_table() {
}

feature_table::~feature_table() {
}

int feature_table::get_num_features() {
  return features_.size();
}

feature_descriptor* feature_table::get_feature(int feature_number) {
  if (feature_number < 0 || feature_number >= (int)features_.size())
    return nullptr;
  return &(features_[feature_number]);
}

int feature_table::add_feature(string feature_name, int feature_type) {
  int n = features_.size();
  feature_descriptor* fd = new feature_descriptor(feature_name, feature_type);
  features_.push_back(*fd);
  return n;
}

bool feature_descriptor::get_bool_feature_value(int entry) {
  if (entry < 0 || entry >= (int)bool_table_.size())
    return false;
  return bool_table_[entry];
}

int feature_descriptor::get_int_feature_value(int entry) {
  if (entry < 0 || entry >= (int)int_table_.size())
    return -1;
  return int_table_[entry];
}

double feature_descriptor::get_double_feature_value(int entry) {
  if (entry < 0 || entry >= (int)double_table_.size())
    return 0.0;
  return double_table_[entry];
}

int feature_descriptor::get_num_bool_entries() {
  return bool_table_.size();
}

int feature_descriptor::get_num_int_entries() {
  return int_table_.size();
}

int feature_descriptor::get_num_double_entries() {
  return double_table_.size();
}

bool feature_descriptor::make_buckets(int num_buckets) {
  if (feature_type_ != feature_descriptor::feature_type_double)
    return false;
  double_feature_top_range_ = -10000000;
  double_feature_bottom_range_ = 10000000;
  for (int i = 0; i < (int)double_table_.size(); i++) {
    if (double_table_[i] > double_feature_top_range_)
      double_feature_top_range_ = double_table_[i];
    if (double_table_[i] < double_feature_bottom_range_)
      double_feature_bottom_range_ = double_table_[i];
  }
  bucket_delta_ = (double_feature_top_range_ - double_feature_bottom_range_) / ((double)num_buckets);
  num_buckets_ = num_buckets;
  bucketized_ = true;
  return true;
}

double feature_descriptor::double_bucket_interval_high(int bucket_num) {
  return bucket_delta_ * ((double)(bucket_num + 1)) + double_feature_bottom_range_;
}

double feature_descriptor::double_bucket_interval_low(int bucket_num) {
  return bucket_delta_ * ((double)bucket_num) + double_feature_bottom_range_;
}

int feature_descriptor::get_bucket_number(double x) {
  if (!bucketized_)
    return -1;

  double t = double_feature_bottom_range_; 
  for (int i = 0; i < num_buckets_; i++) {
    if (x >= t && x <= (t + bucket_delta_))
      return i;
    t += bucket_delta_;
  }
  return -1;
}

void feature_value::print_feature_value() {
  printf("feature type: %d, Value: ", feature_type_);
  switch (feature_type_) {
    case feature_descriptor::feature_type_int:
      printf("%d\n", int_value_);
      return;
    case feature_descriptor::feature_type_bool:
      printf("%s\n", bool_value_?"true":"false");
      return;
    case feature_descriptor::feature_type_double:
      printf("%8.4f\n", double_value_);
      return;
  }
}

void probability_value::print_probability_value() {
  printf("Type: %d\n", feature_type_);
  for (int i = 0; i < (int)pv_.size(); i++) {
    printf("%8.5f, ", pv_[i]);
  }
  printf("\n");
}

int feature_descriptor::get_number_of_buckets() {
  return num_buckets_;
}

void print_ml_data(int m, double* y, vector<double>* x) {
  for (int i = 0; i < m; i++) {
    printf("[%7.4lf, ( ", y[i]);
    for (int j = 0; j < (int) x[0].size(); j++)
      printf("%7.4lf ", x[i][j]);
    printf(")]\n");
  }
}

bool calc_grad(int d, double* M, vector<double>& x, vector<double>* grad) {
  for (int i = 0; i < d; i++) {
    (*grad)[i] = 2.0 * M[joint_index(i, i, d, d)] * x[i];
    for (int j = 0; j < d; j++) {
      if (i == j)
        continue;
      (*grad)[i] += M[joint_index(i, j, d, d)] * x[j] + M[joint_index(j, i, d, d)] * x[i];
    }
  }
  return true;
}

double eval_form(int d, double* M, vector<double>& x) {
  double t = 0.0;

  for (int i = 0; i < d; i++) {
    for (int j = 0; j < d; j++) {
      t += M[joint_index(i, j, d, d)] * x[i] * x[j];
    }
  }
  return t;
}

void update_guess(vector<double>& x, double alpha, vector<double>& grad) {

  for (int i = 0; i < x.size(); i++)
    x[i] += alpha * grad[i];
}

bool maximize_form_on_sphere(int d, double* M, double R, double alpha,
            vector<double>* w) {
  vector<double> grad;
  vector<double> old_guess;

  for (int i = 0; i < d; i++) {
    grad.push_back(0.0);
    old_guess.push_back(0.0);
  }

  // initialize  
  for (int i = 0; i < d; i++)
    (*w)[i] = 0.0;
  (*w)[0] = R;

  const int MAX_N = 10000;
  int N = 0;
  while (N++ < MAX_N) {
   
    copy_vector(*w, old_guess) ;
    if(!calc_grad(d, M, *w, &grad))
      return false;
    update_guess(*w, alpha, grad);

    // Normalize
    double t = 0.0;
    for (int i = 0; i < d; i++) {
      t += (*w)[i] * (*w)[i];
    }
    t = R / sqrt(t);
    for (int i = 0; i < d; i++) {
      (*w)[i] *= t;
    }
    if (dist(*w, old_guess) < (R / 100000.0))
      break;
  }

  if (N >= MAX_N)
    return false;
  return true;
}

