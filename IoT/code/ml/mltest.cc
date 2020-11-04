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


DEFINE_bool(printall, false, "printall flag");

class MLTest : public ::testing::Test {
 protected:
  virtual void SetUp();
  virtual void TearDown();
};

void MLTest::SetUp() {}

void MLTest::TearDown() {}

bool makeTestData(const char* filename, int sizeBytes) {
  byte* buf = new byte[sizeBytes];

  int randfd = open("/dev/urandom", O_RDONLY);
  int randfile = creat(filename, S_IRWXU | S_IRWXG);
  if (randfd < 0 || randfile < 0) {
    printf("makeTestData: cant create test file\n");
    delete []buf;
    return false;
  }
  int bytes_in_file = read(randfd, buf, sizeBytes);
  if (bytes_in_file != sizeBytes) {
    printf("makeTestData: Cant read %d bytes in data file\n", sizeBytes);
    delete []buf;
    return false;
  }
  if (write(randfile, buf, sizeBytes) < 0) {
    printf("makeTestData: Cant write test file\n");
    delete []buf;
    return false;
  }
  close(randfd);
  close(randfile);
  delete []buf;
  return true;
}

bool readTestData(const char* filename, int sizeBytes, byte* buf) {
  int randfd = open(filename, O_RDONLY);
  bool ret = true;
  int bytes_in_file;

  if (randfd < 0) {
    printf("readTestData: Cant open read file\n");
    ret = false;
    goto done;
  }
  bytes_in_file = read(randfd, buf, sizeBytes);
  if (bytes_in_file != sizeBytes) {
    printf("readTestData: bytes read/requested don't match %d, %d\n", sizeBytes,
           bytes_in_file);
    ret = false;
    goto done;
  }

done:
  close(randfd);
  return ret;
}

void print_linear_equation(double& y, vector<double> x) {
  printf("  %lf7.4 = ", y);
  for(int i = 0; i < (int)x.size(); i++) {
    if (i > 0)
      printf(" + ");
    printf("%lf7.4", x[i]);
  }
  printf("\n");
}

bool gaussian_elimination_test() {
    // Solve
    //  5.500 = 3.025a + 3.850b + 5.5c + 11d
    //  2.134 = 2.533a + 3.025b + 3.850c + 5.500d
    //  1.309 = 2.208a + 2.533b + 3.025c + 3.850d
    //  0.994 =  1.978a + 2.208b + 2.533c + 3.025d
  double y[4] = {
    5.5000, 2.1345, 1.3095, 0.9937,
  };
  vector<double> x[4];
  vector<double> sol;

  x[0].push_back(3.025); x[0].push_back(3.850); x[0].push_back(5.5000); x[0].push_back(11.000);
  x[1].push_back(2.5333); x[1].push_back(3.025); x[1].push_back(3.85); x[1].push_back(5.5);
  x[2].push_back(2.2083); x[2].push_back(2.5333); x[2].push_back(3.025); x[2].push_back(3.85);
  x[3].push_back(1.978); x[3].push_back(2.208); x[3].push_back(2.5333); x[3].push_back(3.025);
  sol.push_back(0.0); sol.push_back(0.0); sol.push_back(0.0); sol.push_back(0.0);

  if (!gaussian_elimination(4, (double*)y, (vector<double>*)x, sol))
    return false;

  printf("Solution to:\n");
  for (int i = 0; i < 4; i++) {
    printf(" %7.4lf = ", y[i]);
    printvector(x[i]);
    printf("\n");
  }
  printf("is: ");
  printvector(sol);
  printf("\n");
  printf("\n");

  // Solution should be:
  double checked_sol[4] = {
    8.6261, -12.9441, 4.3796, 0.4685
  };
  for (int i = 0; i < 4; i++) {
    if (fabs(sol[i] - checked_sol[i]) > 0.05)
      return false;
  }

  return true;
}

double power(int p, double a) {
  double t = 1.0;

  for (int i = 0; i < p; i++)
    t *= a;
  return t;
}

bool gaussian_least_squares_test() {
  bool ret = true;

  // Least squares cubic approximation of
  // f(x) = 1/2 + 2/5 sin(2x)

  // test data
  int m = 11;
  double y_t[11];
  double t;
  double pi = 3.1415926535;
  for (int i = 0; i < m; i++) {
    t = ((double)i)*0.1;
    y_t[i] = 0.5 + (2.0/5.0) * sin(2*pi*t);
  }

  vector<double>* x_t = new vector<double>[11];
  for (int i = 3; i >= 0; i--) {
    for (int k = 0; k < m; k++) {
      t = ((double)k) * 0.1;
      x_t[k].push_back(power(i, t));
    }
  }

  printf("ML data:\n");
  print_ml_data(m, y_t, x_t);

  // Solution should be: 9.6303 -14.5017   5.0075   0.4235
  double checked_sol[4] = {
    8.4776, -12.7164, 4.2896, 0.4746
  };

  vector<double> w;
  w.push_back(0.0); w.push_back(0.0); w.push_back(0.0); w.push_back(0.0);

  if (!gaussian_least_squares(m, y_t, x_t, &w)) {
    ret = false;
    goto done;
  }

  printf("linear coefficients: ");
  printvector(w);
  printf("\n");

  for (int i = 0; i < 4; i++) {
    if (fabs(w[i] - checked_sol[i]) > 0.05)
      ret = false;
      goto done;
  }

done:
  delete []x_t;
  return ret;
}

bool kmeans_test() {
  int n = 40;
  int k = 4;
  int d = 2;

  vector<double>* in_data = new vector<double>[n];
  vector<double>* out_data = new vector<double>[k];

  allocate_double_vectors(n, d, in_data);
  allocate_double_vectors(k, d, out_data);
  zero_double_vectors(n, in_data);
  zero_double_vectors(k, out_data);

  for (int i = 0; i < n; i++) {
    in_data[i][0] = 2.0 * ((double) (i/k)) + ((double)(1 - i%2))*.005 * ((double)i);
    in_data[i][1] = 4.0 * ((double) (i/k)) + ((double)(1 - i%2))*.005 * ((double)i);;
  }
  for (int i = 0; i < k; i++) {
    out_data[i][0] = ((double) (7 * i));
    out_data[i][1] = (double) (i%6);
  }
  bool out = k_means(k, n, in_data, out_data);
  if (out) {
    // print data and clusters
    printf("\nData: \n");
    print_data_set(n, in_data);
    printf("%d clusters: \n", k);
    print_data_set(k, out_data);
    printclusters(n, k, in_data, out_data);
  }

  delete []in_data;
  delete []out_data;
  return out;
}

#define NUM_TRAIN 20 // must be even
bool svm_test() {
  printf("\nSVM test\n");
  double C = .01;
  double t = .9;
  int n = NUM_TRAIN;
  int max_passes = 5;
  vector<double> w(2);
  double b;
  svm_training_data data[NUM_TRAIN];

  // fill training data
  for (int i = 0; i < NUM_TRAIN; i++) {
    data[i].input_.push_back(0.0);
    data[i].input_.push_back(0.0);
  }
  for (int i = 0; i < NUM_TRAIN/2; i++) {
#if 1
    data[i].input_[0] = (((double) i) + 12);
#else
    data[i].input_[0] = 0.0;
#endif
    data[i].input_[1] = (((double) i) + 12);
    data[i].outcome_ = 1;

#if 1
    data[i + NUM_TRAIN/2].input_[0] = -(((double) i) + 12);
#else
    data[i].input_[0] = 0.0;
#endif
    data[i + NUM_TRAIN/2].input_[1] = -(((double) i) + 12);
    data[i + NUM_TRAIN/2].outcome_ = -1;
  }

  printf("Training data:\n");
  for (int i = 0; i < n; i++) {
    printf("\t%8.4f %8.4f, outcome: %2.0f\n", data[i].input_[0],
        data[i].input_[1], data[i].outcome_);
  }

  bool out = svm_train(C, t, max_passes, n, data, &w, &b);
  if (!out)
    return false;

  printf("w: ");
  printvector(w);
  printf(", b: %10.4f\n", b);

  double ans = 0.0;

  // should be 1
  vector<double> test_x1(2);
  test_x1[0] = 8.0;
  test_x1[1] = 8.0;
  ans = svm_predict(w, b, test_x1);
  printf("test point: "); printvector(test_x1);
  printf(", w: "); printvector(w); printf(", b: %8.4f\n", b);
  printf("ans1: %4.2f\n", ans);
  if (ans != 1.0)
    out = false;

  // should be -1
  vector<double> test_x2(2);
  test_x2[0] = -8.0;
  test_x2[1] = -8.0;
  ans = svm_predict(w, b, test_x2);
  printf("test point: "); printvector(test_x2);
  printf(", w: "); printvector(w); printf(", b: %8.4f\n", b);
  printf("ans2: %4.2f\n", ans);
  if (ans != -1.0)
    out = false;
  return out;
}

bool gradient_descent_test() {

#define NUM_SAMPLES 20
  int num_samples = NUM_SAMPLES;
  vector<double> w(1);
  vector<double> x[NUM_SAMPLES];
  double y[NUM_SAMPLES];
  double b = 0;

  // fill data: y = 5x+3
  printf("gradient descent training data\n");
  w[0] = 0.0;
  // double v = .1;

  for (int i = 0; i < NUM_SAMPLES; i++) {
    x[i].push_back((double)i);
    y[i] = 5.0 * (x[i])[0] + 3.0;
#if 0
    if ((i%2) == 1)
      y[i] += (-1.0) * v;
    else
      y[i] += v;
#endif
    printf("\t%8.4f %8.4f\n", x[i][0], y[i]);
  }
  zero_vector(w);

  int num_steps = gradient_descent(2000, .0000003, .9, num_samples,
              x, y, &w, &b);
  if (num_steps < 0)
    return false;
  printf("%d steps, w: ", num_steps); printvector(w); printf("b: %8.4f\n", b);
  double error = sqrt((w[0] - 5.0) * (w[0] - 5.0) + (b - 3.0) * (b - 3.0));
  printf("Estimation error: %8.4f\n", error);
  if (error > 3)
    return false;
  return true;
}

bool basic_matrix_test() {
  double_matrix A(10, 10);
  double_matrix B(10, 10);

  // make identity
  A.make_identity_matrix();
  printf("Identity:\n");
  A.print_matrix();
  printf("\n");

  // swap rows
  if (!A.swap_rows(2, 8))
    return false;
  printf("After swapping rows 2 and 8:\n");
  A.print_matrix();
  printf("\n");

  // swap columns
  if (!A.swap_columns(3, 7))
    return false;
  printf("After swapping columns 3 and 7:\n");
  A.print_matrix();
  printf("\n");

  // add to row
  if (!A.add_row_times_scaler_to_row(0, 1, 5.0))
    return false;
  printf("After adding 5 times row 0 to row 1:\n");
  A.print_matrix();
  printf("\n");

  B.make_identity_matrix();

  // add to column
  if (!B.add_column_times_scaler_to_column(3, 6, 8.0))
    return false;
  printf("After adding 8 times column 3 to column 6:\n");
  B.print_matrix();
  printf("\n");

  // multiply matricies
  double_matrix C(10, 10);
  if (!double_matrix_multiply(A, B, &C))
    return false;
  printf("After multiplying:\n");
  C.print_matrix();
  printf("\n");

  // inverses
  double_matrix D(5, 5);
  double_matrix F(5, 5);
  double_matrix I(5, 5);
  I.make_identity_matrix();

  double XX[25] = {
    2.0, 4.0, 3.2, 8.1, 1.0,
    7.0, 1.0, 2.9, 3.3, 8.0,
    1.5, 3.0, 5.0, 5.1, 9.0,
    8.8, 4.0, 4.7, 7.3, 3.0,
    5.5, 2.0, 4.4, 9.0, 2.0,
  };

  for (int i = 0; i < D.num_rows(); i++) {
    for (int j = 0; j < D.num_columns(); j++) {
      D.set_entry(i, j, XX[5 * i + j]);
    }
  }
  printf("Inverting:\n");
  D.print_matrix();
  printf("\n");

  if (!double_matrix_invert(D, &F))
    return false;
  printf("F:\n");
  F.print_matrix();

  double_matrix G(5, 5);
  if (!double_matrix_multiply(D, F, &G))
    return false;

  printf("\nG:\n");
  G.print_matrix();
  if (!I.is_equal(G))
    return false;
  return true;
}

bool gradient_ascent_test() {
  return true;
}

double f(vector<double> x) {
  return (x[0] - 4.0) * (x[0] - 4.0) - 2;
}

void f_prime(vector<double> in, vector<double>* out) {
  (*out)[0] = 2.0 * (in[0] - 4.0);
}

bool newton_test() {
  vector<double> x(1);
  vector<double> out(1);
  x[0] = 10.0;

  if (!newton(x, f, f_prime, .000001, 100, &out))
    return false;
  double error = fabs(out[0] - (sqrt(2) + 4.0));
  printf("\nProposed solution: %12.8f error: %12.8f\n", out[0], error);
  if (error > 2.0 * .0001)
    return false;
  return true;
}

// returns k between 0 and k-1 with probability, prob interval
int select(int n, double* prob_interval) {
  int i;
  const int scale = 100000;

  int r = rand() % scale;
  double x = r / ((double)scale);
  for (i = 0; i < n; i++) {
    if (x < prob_interval[i])
      return i;
    }
  return n - 1;
}

bool em_test() {
  const int num_mixtures = 5;
  const int num_samples = 20000;

  // init rad
  unsigned seed = 53;
  srand(seed);

  // Model
  double pi[num_mixtures] = {.15, .25, .15, .25, .2};
  double mu[num_mixtures] = {0.0, 2.0, 6.0, 12.0, 20.0};
  double sigma[num_mixtures] = {1.0, 2.0, 1.0, 1.0, 1.0};;
 
  // generate samples
  double y[num_samples];

  double prob_interval[num_mixtures];
  double z = 0.0;
  for (int i = 0; i < num_mixtures; i++) {
    z += pi[i];
    prob_interval[i] = z;
  }

  for (int j = 0; j < num_samples; j++) {
    int selected_dist = select(num_mixtures, prob_interval);
    clear_gaussian_cdf_with_parameters();
    y[j] = sample_from_gaussian_distribution_with_parameters(mu[selected_dist], sigma[selected_dist]);
  }

  // print parameters
  printf("Generative model\n");
  for (int j = 0; j < num_mixtures; j++) {
    printf("pi[%d]: %8.4f,      mu[%d]: %8.4f,      sigma[%d]: %8.4f\n", j, pi[j],
      j, mu[j], j, sigma[j]);
  }
  printf("\n");

  // Guesses of parameters
  double pi_in[num_mixtures] = {.2, .2, .2, .2, .2};
  double mu_in[num_mixtures] = {0.0, 3.0, 7.0, 9.0, 18.0};
  double sigma_in[num_mixtures] = {1.0, 1.0, 1.0, 1.0, 1.0};
  printf("Guess of parameters\n");
  for (int j = 0; j < num_mixtures; j++) {
    printf("pi_in[%d]: %8.4f,      mu_in[%d]: %8.4f,      sigma_in[%d]: %8.4f\n", j, pi_in[j],
      j, mu_in[j], j, sigma_in[j]);
  }
  printf("\n");

  double pi_out[num_mixtures];
  double mu_out[num_mixtures];
  double sigma_out[num_mixtures];
  double prec = 1;

  if (!gaussian_em(num_samples, y, num_mixtures, pi_in, mu_in, sigma_in, prec,
          pi_out, mu_out, sigma_out))
    return false;

  printf("EM estimate\n");
  for (int j = 0; j < num_mixtures; j++) {
    printf("pi_out[%d]: %8.4f,      mu_out[%d]: %8.4f,      sigma_out[%d]: %8.4f\n", j, pi[j],
      j, mu_out[j], j, sigma_out[j]);
  }
  printf("\n");

  return true;
}

bool least_squares_test() {
  const int num_points= 20;
  vector<double> x[num_points];
  vector<double> y(num_points);
  vector<double> w(num_points + 1);

  for (int i = 0; i < num_points; i++) {
    x[i].reserve(2);
    if ((i%2) ==0) {
      x[i][0] = 3.0 + ((double) (i/2));
      x[i][1] = 3.0 + ((double) (i/2));
      y[i] = 1;
    } else {  
      x[i][0] = -(3.0 + ((double) (i/2)));
      x[i][1] = -(3.0 + ((double) (i/2)));
      y[i] = -1;
    }
  }

#if 0
  if (!least_squares(num_points, y, x, w))
    return false;
#endif
  return true;
}

//   table: owner marital_status  income  defaulted
class home_default_data {
public:
  bool owner_;
  int marital_status_;
  double income_;
  bool outcome_;
};
#define NUM_DEFAULT_DATA 10
home_default_data t_data[NUM_DEFAULT_DATA] = {
  // homeowner? marital  income?  defaulted?
  {true,  1, 125000.0, true},
  {false, 2, 100000.0, true},
  {false, 1, 60000.0,  true},
  {true,  2, 120000.0, true},
  {false, 3, 95000.0,  false},
  {false, 2, 60000.0,  true},
  {true,  3, 220000.0, true},
  {false, 1, 85000.0,  false},
  {false, 2, 75000.0,  true},
  {false, 1, 90000.0,  false},
};

bool naive_bayes_test() {
  feature_table table;

  int owner_feature = table.add_feature("owner", feature_descriptor::feature_type_bool); 
  int marital_status_feature = table.add_feature("marital_status", feature_descriptor::feature_type_int); 
  int income_feature = table.add_feature("income", feature_descriptor::feature_type_double); 
  int outcome_feature = table.add_feature("outcome", feature_descriptor::feature_type_bool); 
  table.get_feature(marital_status_feature)->set_int_feature_top_range(3);
  table.get_feature(marital_status_feature)->set_int_feature_bottom_range(1);

  for (int i = 0; i < NUM_DEFAULT_DATA; i++) {
    table.get_feature(owner_feature)->add_bool_feature_value(t_data[i].owner_);
    table.get_feature(marital_status_feature)->add_int_feature_value(t_data[i].marital_status_);
    table.get_feature(income_feature)->add_double_feature_value(t_data[i].income_);
    table.get_feature(outcome_feature)->add_bool_feature_value(t_data[i].outcome_);
  }
  table.get_feature(income_feature)->make_buckets(12);

  // print data table
  printf("\n");
  printf(  "Home Owner  Marital Status   Income      Paid loan\n");
  for (int i = 0; i < NUM_DEFAULT_DATA; i++) {
    printf("%s         %d              %8.0f(%2d)  %s\n",
      table.get_feature(owner_feature)->get_bool_feature_value(i)?"Yes":"No ",
      table.get_feature(marital_status_feature)->get_int_feature_value(i),
      table.get_feature(income_feature)->get_double_feature_value(i),
      table.get_feature(income_feature)->
          get_bucket_number(table.get_feature(income_feature)->get_double_feature_value(i)),
      table.get_feature(outcome_feature)->get_bool_feature_value(i)?"Yes":"No ");
  }
  printf("\n");

  // Home owner: false , marital status: 1, income: 85,000
  printf("\nPredict 1\n");
  bayes_probability_model model1(table);
  if (!model1.compute_bayes_probabilties())
    return false;
  printf("\n");
  model1.print_bayes_model();
  printf("\n");

  vector<feature_value> pv1;
  feature_value ho1;
  ho1.feature_type_ = feature_descriptor::feature_type_bool;
  ho1.bool_value_ = false;
  feature_value ms1;
  ms1.feature_type_ = feature_descriptor::feature_type_int;
  ms1.int_value_ = 1;
  feature_value sal1;
  sal1.feature_type_ = feature_descriptor::feature_type_double;
  sal1.double_value_ = 85000.0;

  pv1.push_back(ho1);
  pv1.push_back(ms1);
  pv1.push_back(sal1);

  double odds_to_pay = model1.naive_bayes_odds_yes(pv1);
  printf("Odds to pay: %8.4f\n", odds_to_pay);

  // Home owner: true, marital status: 2, income: 210,000
  printf("\nPredict 2\n");
  bayes_probability_model model2(table);
  if (!model2.compute_bayes_probabilties())
    return false;
  printf("\n");
  model2.print_bayes_model();
  printf("\n");

  // Home owner: true, marital status: 2, income: 210,000
  vector<feature_value> pv2;
  feature_value ho2;
  ho2.feature_type_ = feature_descriptor::feature_type_bool;
  ho2.bool_value_ = true;
  feature_value ms2;
  ms2.feature_type_ = feature_descriptor::feature_type_int;
  ms2.int_value_ = 2;
  feature_value sal2;
  sal2.feature_type_ = feature_descriptor::feature_type_double;
  sal2.double_value_ = 210000.0;

  pv2.push_back(ho2);
  pv2.push_back(ms2);
  pv2.push_back(sal2);

  odds_to_pay = model2.naive_bayes_odds_yes(pv2);
  printf("Odds to pay: %8.4f\n", odds_to_pay);


  return true;
}

bool simple_statistics_test() {
  double s1[20], s2[20], p1[20], p2[20], p3[2], p4[2];
  int num_samp1 = 20;
  int num_samp2 = 20;

  for (int i = 0; i < num_samp1; i++) {
      s1[i] = (double) (i + 1);
      s2[i] = (double) (20 - i);
  }

  double pr[20 * 20];
  for (int i = 0; i < 20; i++)
    for (int j = 0; j < 20; j++)
      pr[joint_index(i, j, num_samp1, num_samp2)] = 0.0;
  for (int i = 0; i < 20; i++)
    pr[joint_index(i, i, num_samp1, num_samp2)]= 1.0/20.0;
  printf("\nJoint probabilities:\n");
  print_joint_probability(num_samp1, num_samp2, pr);
  printf("\n");

  for (int i = 0; i < num_samp1; i++) {
    p1[i] = 0.0;
    for (int j = 0; j < num_samp1; j++) {
      p1[i] += pr[joint_index(i, j, num_samp1, num_samp2)];
    }
    p2[i] = p1[i];
  }

  printf("\n");
  printf("Distribition 1:\n");
  print_double_array(num_samp1, p1);
  printf("\n");
  printf("Distribition 2:\n");
  print_double_array(num_samp2, p2);
  printf("\n");
  printf("Values 1:\n");
  print_double_array(num_samp1, s1);
  printf("\n");
  printf("Values 2:\n");
  print_double_array(num_samp2, s2);
  printf("\n");
  p3[0] = 0.5;
  p3[1] = 0.5;
  p4[0] = 0.25;
  p4[1] = 0.75;

  double exp1, var1;
  double exp2, var2;

  if (!expectation_variance(num_samp1, s1, p1, &exp1, &var1))
    return false;
  printf("Distribution 1: expectation: %7.4lf, variance: %7.4lf\n", exp1, var1);
  if (fabs(exp1 - 10.5) > .001)
    return false;
  if (!expectation_variance(num_samp2, s2, p2, &exp2, &var2))
    return false;
  printf("Distribution 2: expectation: %7.4lf, variance: %7.4lf\n", exp2, var2);
  if (fabs(exp2 - 10.5) > .001)
    return false;

  double cor = correlation(num_samp1, s1, num_samp2, s2, pr);
  printf("Correlation: %7.4lf\n", cor);
  if (fabs(cor + 1.0) > .001)
   return false;

  printf("\n");
  printf("Entropy distribution 1: ");
  print_double_array(2, p3);
  printf("\n");
  printf("Entropy distribution 2: ");
  print_double_array(2, p4);
  printf("\n");
  double ent1 = entropy(2, p3);
  double ent2 = entropy(2, p4);
  printf("entropy 1: %7.4lf, entropy 2: %7.4lf\n", ent1, ent2);
  if (fabs(ent1 - 1.0) > .001)
    return false;
  if (fabs(ent2 - .8113) > .001)
    return false;

  double p5[4];
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      p5[joint_index(i, j, 2, 2)] = p3[i] * p4[j];
    }
  }
  printf("\nJoint entropy distribution\n");
  print_joint_probability(2, 2, p5);

  double cross = joint_entropy(2, 2, p5);
  printf("joint entropy: %7.4lf\n", cross);
  if (fabs(cross - (ent1 + ent2)) > .001)
    return false;
  
  return true;
}

bool support_test() {
  double t = 0.0;
  double r;
  const int N_tests = 1000;

  for (int i = 0; i < N_tests; i++) {
    r = sample_from_uniform_distribution();
    t += r;
  }
  t /= ((double)N_tests);
  printf("\nuniform average: %10.6lf\n", t);
  if (fabs(t - .5)> .05)
    return false;

  double mu1 = 4.0; 
  double sigma1 = 2.0; 
  const int N_GAUSS_PTS = 20;
  double base1 = mu1 - 10.0;
  double delta1 = 20 / ((double) N_GAUSS_PTS);
  clear_gaussian_cdf_with_parameters();
  printf("\ngaussian parameters, mu: %8.6lf, sigma: %8.6lf\n", mu1, sigma1);
  printf("Distribution:\n");
  for (int i = 0; i <= N_GAUSS_PTS; i++) {
    double s = base1 + delta1 * ((double) i);
    double r = gaussian_with_parameters(s, mu1, sigma1);
    printf("(%8.6lf, %8.6lf)  ", s, r);
    if ((i%5) == 4)
      printf("\n");
  }
  printf("\n");
  printf("inverse CDF:\n");
  for (int i = 0; i <= N_GAUSS_PTS; i++) {
    double s = ((double) i) /((double) N_GAUSS_PTS);
    double r = gaussian_inverse_cdf_with_parameters(s, mu1, sigma1);
    printf("(%8.6lf, %8.6lf)  ", s, r);
    if ((i%5) == 4)
      printf("\n");
  }
  printf("\n");

  double ll = -10.0;
  double ul = 10.0;
  double area = integrate(basic_gaussian, ll, ul, 10000);
  printf("Area of gaussian from %8.4lf to %8.4lf is %8.6lf\n", ll, ul, area);
  ul = 1.0;
  ll = 0.0;
  area = integrate(basic_gaussian, ll, ul, 10000);
  printf("Area of gaussian from %8.4lf to %8.4lf is %8.6lf\n", ll, ul, area);

  double r_x = .44;
  double r_y = gaussian_inverse_cdf(r_x);
  double r_z = sample_from_gaussian_distribution();
  printf("r_x: %8.6lf, r_y: %8.6lf, r_z: %8.6lf\n", r_x, r_y, r_z);
  double r_av = 0.0;
  int in_one_sigma = 0;
  for (int i = 0; i < N_tests; i++) {
    r_z = sample_from_gaussian_distribution();
    if ((r_z <= 1.0) && (r_z) >= (-1.0))
      in_one_sigma++;
    r_av += r_z;
  }
  r_av /= ((double)N_tests);
  double ratio_in_one_sigma = ((double)in_one_sigma) / ((double) N_tests);
  printf("r_av: %8.6lf, ratio in one sigma: %8.6lf\n", r_av, ratio_in_one_sigma);
  if ((fabs(r_av) - .5) > .1)
     return false;
  // 2/3 of point should be between -1 and 1
  if (fabs(ratio_in_one_sigma - .66) > .09)
    return false;

  double mu = 3;
  double sigma = 3.0;
  double s;
  clear_gaussian_cdf_with_parameters();
  in_one_sigma = 0;
  printf("\nGaussian with mu = %8.6lf, sigma = %8.6lf\n", mu, sigma);
  for (int i = 0; i < N_tests; i++) {
    r = sample_from_gaussian_distribution_with_parameters(mu, sigma);
    s = r - mu;
    if (fabs(r - mu) < sigma) {
      in_one_sigma++;
    }
  }
  ratio_in_one_sigma = ((double) in_one_sigma) / ((double) N_tests);
  printf("ratio in one sigma: %8.6lf\n", ratio_in_one_sigma);
  if (fabs(ratio_in_one_sigma - .66) > .09)
    return false;

  clear_gaussian_cdf_with_parameters();
  mu = -3.0;
  in_one_sigma = 0;
  printf("Gaussian with mu = %8.6lf, sigma = %8.6lf\n", mu, sigma);
  for (int i = 0; i < N_tests; i++) {
    r =sample_from_gaussian_distribution_with_parameters(mu, sigma);
    if (fabs(r - mu) < sigma) {
      in_one_sigma++;
    }
  }
  ratio_in_one_sigma = ((double) in_one_sigma) / ((double) N_tests);
  printf("ratio in one sigma: %8.6lf\n", ratio_in_one_sigma);

  vector<double> x, y;
  x.push_back(1.0);
  x.push_back(0.0);
  y.push_back(0.0);
  y.push_back(1.0);

  double a = dist(x, y);
  printf("\nsupport distance: %10.6lf\n", a);

  double M[4];
  M[joint_index(0, 0, 2, 2)] = 13.0;
  M[joint_index(1, 1, 2, 2)] = 13.0;
  M[joint_index(0, 1, 2, 2)] = 7.0;
  M[joint_index(1, 0, 2, 2)] = 7.0;
  printf("Form:\n");
  print_double_matrix(2, M);

  double b1 = eval_form(2, M, x);
  printf("Form evaluated on ");
  printvector(x);
  printf(", %10.6lf\n", b1);
  double b2 = eval_form(2, M, y);
  printf("Form evaluated on ");
  printvector(y);
  printf(", %10.6lf\n", b2);

  vector<double> test_w;
  test_w.push_back(0.0);
  test_w.push_back(0.0);

  if (!maximize_form_on_sphere(2, M, 1.0, 0.5, &test_w))
    return false;
  printf("w: ");
  printvector(test_w);
  printf("\n");
  double c = eval_form(2, M, test_w);
  printf("max: %10.6lf\n", c);
  return true;
}

bool cluster_test() {
  // generate clusters in (0,10)^2 with 5 centers normally distributed
  // around (1,8), (2,2), (5.5, 5.5), (7,7), (8.5,3).  100 points total.

  const int N_TEST_POINTS = 60;
  const int N_TEST_CENTERS= 5;
  const int N_STRIDES = N_TEST_POINTS / N_TEST_CENTERS;

  vector<double> points[N_TEST_POINTS];
  vector<double> centers[N_TEST_CENTERS];
  vector<double> computed_centers[N_TEST_CENTERS];
  double r;

  centers[0].push_back(2.0); centers[0].push_back(8.0);
  centers[1].push_back(2.0); centers[1].push_back(2.0);
  centers[2].push_back(5.5); centers[2].push_back(5.5);
  centers[3].push_back(7.0); centers[3].push_back(7.0);
  centers[4].push_back(8.5); centers[4].push_back(3.0);

  for (int i = 0; i < N_TEST_CENTERS; i++) {
    for (int j = 0; j < N_STRIDES; j++) {
      r = centers[i][0]+ sample_from_gaussian_distribution()/10.0 * 4.0;
      points[N_STRIDES * i + j].push_back(r);
      r = centers[i][1] + sample_from_gaussian_distribution()/10.0 * 4.0;
      points[N_STRIDES * i + j].push_back(r);
    }
  }
  allocate_double_vectors(N_TEST_CENTERS, 2, computed_centers);

  if (!k_means(N_TEST_CENTERS, N_TEST_POINTS, (vector<double> *)&points, (vector<double> *)&computed_centers))
    return false;

  printf("\n");
  printf("Actual clusters at: ");
  for (int i = 0; i < N_TEST_CENTERS; i++) {
    printf("(");
    printvector(centers[i]);
    printf(") ");
  }
  printf("\n");

  printf("Computed clusters at: ");
  for (int i = 0; i < N_TEST_CENTERS; i++) {
    printf("(");
    printvector(computed_centers[i]);
    printf(") ");
  }
  printf("\n");

  printclusters(N_TEST_POINTS, N_TEST_CENTERS, (vector<double>*)points, (vector<double>*)computed_centers);

  vector<double> initial_guess[N_TEST_CENTERS];
  initial_guess[0].push_back(2.0); initial_guess[0].push_back(6.0);
  initial_guess[1].push_back(1.5); initial_guess[1].push_back(3.0);
  initial_guess[2].push_back(4.0); initial_guess[2].push_back(5.0);
  initial_guess[3].push_back(6.5); initial_guess[3].push_back(7.0);
  initial_guess[4].push_back(9.0); initial_guess[4].push_back(5.0);
  printf("\nWith initial guess: ");
  for (int i = 0; i < N_TEST_CENTERS; i++) {
    printf("(");
    printvector(initial_guess[i]);
    printf(") ");
  }
  printf("\n");

  if (!k_means_with_guess(N_TEST_CENTERS, N_TEST_POINTS, (vector<double> *)points, 
        (vector<double> *)initial_guess, (vector<double> *)computed_centers))
    return false;

  printf("Actual clusters at: ");
  for (int i = 0; i < N_TEST_CENTERS; i++) {
    printf("(");
    printvector(centers[i]);
    printf(") ");
  }
  printf("\n");
  printf("Computed clusters at: ");
  for (int i = 0; i < N_TEST_CENTERS; i++) {
    printf("(");
    printvector(computed_centers[i]);
    printf(") ");
  }
  printf("\n");

  printclusters(N_TEST_POINTS, N_TEST_CENTERS, (vector<double>*)points, (vector<double>*)computed_centers);

  for (int i = 0; i < N_TEST_CENTERS; i++) {
    if (fabs(centers[i][0] - computed_centers[i][0]) > .25)
      return false;
    if (fabs(centers[i][1] - computed_centers[i][1]) > .25)
      return false;
  }

  printf("\npoints:\n");
  for (int i = 0; i < N_TEST_POINTS; i++) {
    printf("%8.5lf, %8.5lf\n", points[i][0], points[i][1]);
  }
  return true;
}


bool pca_test() {

  // 10 samples, 4 features
  int num_samples = 10;
  int num_features = 3;
  vector<double> samples[10];
  double y, z;

  for (int i = 0; i < num_samples; i++) {
    y = 1.0 /((double) (i + 1));
    z = (double) (i + 1);
    for (int j = 0; j < num_features; j++) {
      samples[i].push_back(z);
      z *= y;
    }
  }

  printf("\npca data\n");
  for (int i = 0; i < num_samples; i++) {
    printvector(samples[i]);
    printf("\n");
  }
  printf("\n");

  vector<double> out;
  for (int i = 0; i < num_features; i++) {
    out.push_back(0.0);
  }

  if (!pca(num_samples, samples, &out))
    return false;

  printf("Line with greatest variation: ");
  printvector(out);
  printf("\n");

  // double err = rms_error(m, x, y, w, func);

  return true;
}

bool MLE_test() {
  const int N_POINTS = 400;
  double pts[N_POINTS];
  double mu = 4.0;
  double sigma = 2.0;
  double computed_mu;
  double computed_sigma;

  clear_gaussian_cdf_with_parameters();
  for (int i = 0; i < N_POINTS; i++) {
    pts[i] = sample_from_gaussian_distribution_with_parameters(mu, sigma);
  }

  if (!MLE_Estimate(N_POINTS, pts, &computed_mu, &computed_sigma))
    return false;

  for (int i = 0; i < N_POINTS; i++) {
    printf("%8.5lf ", pts[i]); 
    if ((i%10) == 9)
      printf("\n");
  }
  printf("\n");
  printf("Real mu: %8.3lf, real sigma: %8.3lf, computed mu: %8.3lf, computed sigma: %8.3lf\n",
      mu, sigma, computed_mu, computed_sigma);
  return true;
}

bool binary_logistic_regression_test() {
  double rate = 0.85;
  const int NUM_EXAMPLES = 60;
  vector<double> x[NUM_EXAMPLES];
  int y[NUM_EXAMPLES];
  vector<double> theta;
  allocate_double_vector(4, theta);

  // probability of passing class
  // features: grade in previous class, hours of study, number of lectures attended
  class features {
  public:
    int previous_grade_;
    int hours_of_study_;
    int lectures_attended_;
    int passed_;
  };
  features examples[NUM_EXAMPLES] = {
    {9, 40, 30, 1},
    {9, 40, 25, 1},
    {9, 20, 25, 1},
    {9, 30, 25, 1},
    {8, 40, 30, 1},
    {8, 30, 25, 1},
    {8, 40, 22, 1},
    {8, 25, 20, 1},
    {8, 10, 30, 1},
    {8, 10, 10, 1},
    {8, 40, 30, 1},
    {8, 30, 30, 1},
    {7, 33, 25, 1},
    {7, 25, 30, 1},
    {7, 20, 25, 1},
    {7, 25, 30, 1},
    {7, 30, 20, 1},
    {7, 20, 25, 1},
    {7, 25, 30, 1},
    {3, 10, 15, 0},
    {6, 30, 30, 1},
    {6, 25, 30, 1},
    {6, 30, 30, 1},
    {6, 25, 30, 1},
    {6, 30, 30, 1},
    {6, 25, 30, 1},
    {5, 20, 30, 1},
    {6, 20, 30, 1},
    {6, 18, 30, 1},
    {6, 10, 30, 1},
    {6, 30, 10, 1},
    {4, 10, 16, 0},
    {5, 30, 30, 1},
    {5, 30, 30, 1},
    {4, 30, 30, 1},
    {4, 30, 30, 1},
    {4, 30, 30, 1},
    {4, 30, 30, 1},
    {4, 30, 30, 1},
    {4, 10, 15, 0},
    {3, 30, 28, 1},
    {3, 28, 28, 1},
    {3, 20, 20, 0},
    {3, 15, 20, 0},
    {3, 30, 20, 0},
    {3, 5, 20, 0},
    {2, 40, 28, 1},
    {2, 20, 20, 0},
    {2, 10, 20, 0},
    {2, 40, 28, 1},
    {2, 20, 20, 0},
    {2, 10, 20, 0},
    {1, 10, 20, 0},
    {1, 10, 20, 0},
    {1, 10, 20, 0},
    {1, 10, 20, 0},
    {0, 10, 20, 0},
    {0, 10, 20, 0},
    {0, 10, 20, 0},
    {0, 10, 20, 0},
  };
  for (int i = 0; i < NUM_EXAMPLES; i++) {
    x[i].push_back(1.0);
    x[i].push_back((double)examples[i].previous_grade_ / 9.0);
    x[i].push_back((double)examples[i].hours_of_study_ / 40.0);
    x[i].push_back((double)examples[i].lectures_attended_ / 30.0);
    x[i][0] = 1.0;
    y[i] = examples[i].passed_;
  }

  theta[0] = 1;
  theta[1] = 1;
  theta[2] = 1;
  theta[3] = 1;
  for (int i = 0; i < NUM_EXAMPLES; i++) {
    printvector(x[i]);
    printf(", outcome: %d\n", y[i]); 
  }

  if (!binary_logistic_regression(NUM_EXAMPLES, x, y, rate, &theta, true))
    return false;

  printf("Parameters: ");
  printvector(theta);
  printf("\n");

  vector<double> tc1;
  tc1.push_back(1.0);
  tc1.push_back(7.0/9.0);
  tc1.push_back(33.0 / 40.0);
  tc1.push_back(27.0/ 30.0);
  vector<double> tc2;
  tc2.push_back(1.0);
  tc2.push_back(2.0/9.0);
  tc2.push_back(13.0 / 40.0);
  tc2.push_back(17.0 / 30.0);

  double guess1 = h_logistic(tc1, theta);
  double guess2 = h_logistic(tc2, theta);
  printf("features test 1: ");
  printvector(tc1);
  printf(", guess: %8.4lf\n", guess1);
  printf("features test 2: ");
  printvector(tc2);
  printf(", guess: %8.4lf\n", guess2);
  return true;
}

bool logistic_regression_test() {
  double rate = 0.25;
  const int NUM_EXAMPLES = 60;
  const int NUM_CLASSES = 10;
  const int NUM_FEATURES = 4;
  vector<double> x[NUM_EXAMPLES];
  int outcome[NUM_EXAMPLES];
  vector<double> theta[NUM_CLASSES];

  for (int i = 0; i < NUM_CLASSES; i++) {
    allocate_double_vector(NUM_FEATURES, theta[i]);
  }

  // features: grade in previous class, hours of study, number of lectures attended
  class features {
  public:
    int previous_grade_;
    int hours_of_study_;
    int lectures_attended_;
    int grade_;
  };
  features examples[NUM_EXAMPLES] = {
    {9, 40, 30, 9},
    {9, 40, 25, 9},
    {9, 20, 25, 8},
    {9, 30, 25, 8},
    {8, 40, 30, 8},
    {8, 30, 25, 9},
    {8, 40, 22, 7},
    {8, 25, 20, 6},
    {8, 10, 30, 5},
    {8, 10, 10, 5},
    {8, 40, 30, 8},
    {8, 30, 30, 7},
    {7, 33, 25, 7},
    {7, 25, 30, 7},
    {7, 20, 25, 7},
    {7, 25, 30, 6},
    {7, 30, 20, 6},
    {7, 20, 25, 6},
    {7, 25, 30, 6},
    {3, 10, 15, 3},
    {6, 30, 30, 7},
    {6, 25, 30, 6},
    {6, 30, 30, 6},
    {6, 25, 30, 6},
    {6, 30, 30, 7},
    {6, 25, 30, 7},
    {5, 20, 30, 7},
    {6, 20, 30, 7},
    {6, 18, 30, 6},
    {6, 10, 30, 5},
    {6, 30, 10, 5},
    {4, 10, 16, 3},
    {5, 30, 30, 7},
    {5, 30, 30, 8},
    {4, 30, 30, 7},
    {4, 30, 30, 7},
    {4, 30, 30, 7},
    {4, 30, 30, 7},
    {4, 30, 30, 7},
    {4, 10, 15, 2},
    {3, 30, 28, 5},
    {3, 28, 28, 5},
    {3, 20, 20, 3},
    {3, 15, 20, 3},
    {3, 30, 20, 4},
    {3,  5, 20, 3},
    {2, 40, 28, 4},
    {2, 20, 20, 4},
    {2, 10, 20, 3},
    {2, 40, 28, 5},
    {2, 20, 20, 4},
    {2, 10, 20, 2},
    {1, 10, 20, 1},
    {1, 10, 20, 2},
    {1, 10, 20, 2},
    {1, 10, 20, 1},
    {0, 10, 20, 1},
    {0, 10, 20, 1},
    {0, 10, 20, 0},
    {0, 10, 20, 0},
  };
  for (int i = 0; i < NUM_EXAMPLES; i++) {
    outcome[i] = examples[i].grade_;
  }
  for (int i = 0; i < NUM_EXAMPLES; i++) {
    x[i].push_back(1.0);
    x[i].push_back((double)examples[i].previous_grade_ / 9.0);
    x[i].push_back((double)examples[i].hours_of_study_ / 40.0);
    x[i].push_back((double)examples[i].lectures_attended_ / 30.0);
  }

  for (int i = 0; i < NUM_EXAMPLES; i++) {
    printvector(x[i]);
    printf(", outcome: %d\n", outcome[i]); 
  }

  if (!logistic_regression(NUM_EXAMPLES, NUM_CLASSES, x, outcome, rate, theta))
    return false;

  printf("Parameters: \n");
  for (int i = 0; i < NUM_CLASSES; i++) {
    printvector(theta[i]);
    printf("\n");
  }

  vector<double> tc1;
  tc1.push_back(1.0);
  tc1.push_back(7.0/9.0);
  tc1.push_back(33.0 / 40.0);
  tc1.push_back(27.0/ 30.0);
  vector<double> tc2;
  tc2.push_back(1.0);
  tc2.push_back(2.0/9.0);
  tc2.push_back(13.0 / 40.0);
  tc2.push_back(17.0 / 30.0);

  vector<double> guess1;
  allocate_double_vector(NUM_CLASSES, guess1);
  vector<double> guess2;
  allocate_double_vector(NUM_CLASSES, guess2);
  for (int i = 0; i < NUM_CLASSES; i++) {
    guess1[i]= h_logistic(tc1, theta[i]);
    guess2[i]= h_logistic(tc2, theta[i]);
  }
  probability_scale(guess1);
  probability_scale(guess2);
  printf("features test 1: ");
  printvector(tc1);
  printf(", guess: ");
  printvector(guess1);
  printf("\n");
  printf("features test 2: ");
  printvector(tc2);
  printf(", guess: ");
  printvector(guess2);
  printf("\n");
  return true;
}

bool adaboost_test() {
  return true;
}

bool svm_test2() {
  printf("\nSVM test\n");
  double C = .01;
  double t = .9;
  int n = NUM_TRAIN;
  int max_passes = 5;
  vector<double> w(2);
  double b;
  svm_training_data data[NUM_TRAIN];

  // fill training data
  for (int i = 0; i < NUM_TRAIN; i++) {
    data[i].input_.push_back(0.0);
    data[i].input_.push_back(0.0);
  }
  for (int i = 0; i < NUM_TRAIN/2; i++) {
#if 1
    data[i].input_[0] = (((double) i) + 12);
#else
    data[i].input_[0] = 0.0;
#endif
    data[i].input_[1] = (21 - ((double) i) );
    data[i].outcome_ = 1;

#if 1
    data[i + NUM_TRAIN/2].input_[0] = -(((double) i) + 12);
#else
    data[i].input_[0] = 0.0;
#endif
    data[i + NUM_TRAIN/2].input_[1] = -(21.0 -((double) i));
    data[i + NUM_TRAIN/2].outcome_ = -1;
  }

  printf("Training data:\n");
  for (int i = 0; i < n; i++) {
    printf("\t%8.4f %8.4f, outcome: %2.0f\n", data[i].input_[0],
        data[i].input_[1], data[i].outcome_);
  }

  bool out = svm_train(C, t, max_passes, n, data, &w, &b);
  if (!out)
    return false;

  printf("w: ");
  printvector(w);
  printf(", b: %10.4f\n", b);

  double ans = 0.0;

  // should be 1
  vector<double> test_x1(2);
  test_x1[0] = 8.0;
  test_x1[1] = 8.0;
  ans = svm_predict(w, b, test_x1);
  printf("test point: "); printvector(test_x1);
  printf(", w: "); printvector(w); printf(", b: %8.4f\n", b);
  printf("ans1: %4.2f\n", ans);
  if (ans != 1.0)
    out = false;

  // should be -1
  vector<double> test_x2(2);
  test_x2[0] = -8.0;
  test_x2[1] = -8.0;
  ans = svm_predict(w, b, test_x2);
  printf("test point: "); printvector(test_x2);
  printf(", w: "); printvector(w); printf(", b: %8.4f\n", b);
  printf("ans2: %4.2f\n", ans);
  if (ans != -1.0)
    out = false;
  return out;
  return true;
}

class house_data {
public:
  int size_;
  int bedrooms_;
  int price_;
};

house_data h_d[] = {
  {2104,3,399900},
  {1600,3,329900},
  {2400,3,369000},
  {1416,2,232000},
  {3000,4,539900},
  {1985,4,299900},
  {1534,3,314900},
  {1427,3,198999},
  {1380,3,212000},
  {1494,3,242500},
  {1940,4,239999},
  {2000,3,347000},
  {1890,3,329999},
  {4478,5,699900},
  {1268,3,259900},
  {2300,4,449900},
  {1320,2,299900},
  {1236,3,199900},
  {2609,4,499998},
  {3031,4,599000},
  {1767,3,252900},
  {1888,2,255000},
  {1604,3,242900},
  {1962,4,259900},
  {3890,3,573900},
  {1100,3,249900},
  {1458,3,464500},
  {2526,3,469000},
  {2200,3,475000},
  {2637,3,299900},
  {1839,2,349900},
  {1000,1,169900},
  {2040,4,314900},
  {3137,3,579900},
  {1811,4,285900},
  {1437,3,249900},
  {1239,3,229900},
  {2132,4,345000},
  {4215,4,549000},
  {2162,4,287000},
  {1664,2,368500},
  {2238,3,329900},
  {2567,4,314000},
  {1200,3,299000},
  // {852,2,179900},
  {1852,4,299900},
  {1852,4,299900},
  {1203,3,239500},
};

double f1(vector<double>& x, vector<double>& w) {
  return w[2] + w[0] * x[0] + w[1] * x[1];
}

double f2(vector<double>& x, vector<double>& w) {
  return w[0] + w[1] * x[1] + w[2] * x[2];
}

bool ng_house_test() {

  double y[47];
  vector<double> x[47];
  vector<double> w;
  int m = 47;

  for (int i = 0; i < m; i++) {
    x[i].push_back((double)h_d[i].size_);
    x[i].push_back((double)h_d[i].bedrooms_);
    x[i].push_back(1.0);
    y[i] = (double)h_d[i].price_;
  }
  w.push_back(0.0);
  w.push_back(0.0);
  w.push_back(0.0);
  if (!gaussian_least_squares(m, y, x, &w)) {
    return false;
  }

  printf("Training data:\n");
  printf("size bdrms price\n");
  for (int i = 0; i < 47; i++) {
    printf("%5d %5d %7d\n", h_d[i].size_, h_d[i].bedrooms_, h_d[i].price_);
  }
  printf("b: %8.2lf, w: (%8.5lf, %8.5lf)\n", w[2], w[0], w[1]);
  // 1458,3,464500
  int nb =3;
  int sz = 1500;
  double price = w[2] + ((double) sz) * w[0] + ((double) nb) * w[1];
  printf("Prediction: %d square ft, %d bedrooms, predicted price: %8.2lf (272500)\n", sz, nb, price);
  double rms_err = rms_error(47, x, y, w, f1);
  printf("rms: %8.2lf\n", rms_err);

  vector<double> xx[47];
  for (int i = 0; i < 47; i++) {
    xx[i].push_back((double)h_d[i].size_ / 100.0);
    xx[i].push_back((double) h_d[i].bedrooms_ / 5.0);
    y[i] /= 10000.0;
  }
#if 0
  printf("Training data:\n");
  printf("size bdrms price\n");
  for (int i = 0; i < 47; i++) {
    printf("%8.2lf %8.2lf %8.2lf\n", xx[i][0], xx[i][1], y[i]);
  }
#endif

  vector<double> ww;
  double b = 0.0;
  ww.push_back(0.0);
  ww.push_back(0.0);
  ww[0] = 8.0;
  ww[1] = -0.8;
  b  = 1.0;
  int num_steps = gradient_descent(1000, .09, .5, 47, xx, y, &ww, &b);
  if (num_steps < 0) {
    printf("Gradiant descent failed\n");
    return false;
  }

  b *= 10000.0;
  ww[0] *= 100.00;
  ww[1] *= 2000.00;
  double price2 = ww[0] * ((double) sz ) + ww[1] * ((double) nb)  + b;
  printf("num_steps: %d, b: %8.5lf, w: (%8.5lf, %8.5lf)\n", num_steps, b, ww[0], ww[1]);
  printf("Prediction: %d square ft, %d bedrooms, predicted price: %8.2lf (272500)\n", sz, nb, price2);

  vector<double> w_t;
  w_t.push_back(b);
  w_t.push_back(ww[0]);
  w_t.push_back(ww[1]);
  rms_err = rms_error(47, x, y, w_t, f2);
  printf("rms: %8.2lf\n", rms_err);
  return true;
}

TEST(ML0, gradient_descent_test) {
  EXPECT_TRUE(gradient_descent_test());
}

TEST(ML1, gradient_ascent_test) {
   EXPECT_TRUE(gradient_ascent_test());
}

TEST(ML2, kmeans_test) {
  EXPECT_TRUE(kmeans_test());
}

TEST(ML3, svm_test) {
  EXPECT_TRUE(svm_test());
}

TEST(ML4, basic_matrix_test) {
  EXPECT_TRUE(basic_matrix_test());
}

TEST(ML5, newton_test) {
  EXPECT_TRUE(newton_test());
}

TEST(ML6, em_test) {
  EXPECT_TRUE(em_test());
}

TEST(ML7, least_squares_test) {
  EXPECT_TRUE(least_squares_test());
}

TEST(ML8, naive_bayes_test) {
  EXPECT_TRUE(naive_bayes_test());
}

TEST(GE, gaussian_elimination_test) {
  EXPECT_TRUE(gaussian_elimination_test());
}

TEST(GLS, gaussian_least_squares_test) {
  EXPECT_TRUE(gaussian_least_squares_test());
}

TEST(S, simple_statistics_test) {
  EXPECT_TRUE(simple_statistics_test());
}

TEST(P, support_test) {
  EXPECT_TRUE(support_test());
}

TEST(P, pca_test) {
  EXPECT_TRUE(pca_test());
}

TEST(C, cluster_test) {
  EXPECT_TRUE(cluster_test());
}

TEST(M, MLE_test) {
  EXPECT_TRUE(MLE_test());
}

TEST(R, binary_logistic_regression_test) {
  EXPECT_TRUE(binary_logistic_regression_test());
}

TEST(R, logistic_regression_test) {
  EXPECT_TRUE(logistic_regression_test());
}

TEST(EL, adaboost_test) {
  EXPECT_TRUE(adaboost_test());
}

TEST(EL, svm_test2) {
  EXPECT_TRUE(svm_test2());
}

TEST(ML, ng_house_test) {
  EXPECT_TRUE(ng_house_test());
}

int main(int an, char** av) {
  ::testing::InitGoogleTest(&an, av);
#ifdef __linux__
  gflags::ParseCommandLineFlags(&an, &av, true);
#else
  google::ParseCommandLineFlags(&an, &av, true);
#endif

  int result = RUN_ALL_TESTS();
  printf("\nTESTS ENDED\n");
  return result;
}

