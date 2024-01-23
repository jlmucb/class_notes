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

const int MAX_ROUNDS = 200;

// find k-means for data
//  d is the dimension of the observations
//  k is number of clusters desired
//  n is number of input pts
//  in_data is an array of size n of vectors of dimension d
//  out_data is an array of size k of vectors of dimension d
bool k_means_with_guess(int k, int n, vector<double>* in_data, 
        vector<double>* initial_guess, vector<double>* out_data) {
  int d = in_data[0].size();

  // current_guess is current guess of cluster centers
  vector<double>* current_guess = new vector<double>[k];
  allocate_double_vectors(k, d, current_guess);

  // next_guess is new guess of cluster centers
  vector<double>* next_guess = new vector<double>[k];
  allocate_double_vectors(k, d, next_guess);

  // num_pts[j] is number of pts in cluster j
  vector<int> num_pts;
  allocate_int_vector(k, num_pts);

  // set_membership[j] is the cluster point j is assigned to
  vector<int> old_set_membership;
  allocate_int_vector(n, old_set_membership);
  vector<int> set_membership;
  allocate_int_vector(n, set_membership);

  // Initial guess of cluster centers is first k points of input data
  copy_double_vectors(k, initial_guess, current_guess);
  assign_to_sets(k, n, in_data, current_guess, old_set_membership);

  int rounds = 0;
  int m;
  for (;;) {
    rounds++;
    zero_int_vector(num_pts);
    zero_double_vectors(k, next_guess);

    // Find the cluster center for the current guess
    for (int i = 0; i < n; i++) {
      m = closest_vector(k, current_guess, in_data[i]);
      add_to_vector(in_data[i], next_guess[m]);
      num_pts[m]++;
    }
    for (int j = 0; j < k; j++) {
      if (num_pts[j] > 0)
        divide_vector_by_scalar((double) num_pts[j], next_guess[j]);
    }

    // assign data to clusters based on new cluster centers
    assign_to_sets(k, n, in_data, next_guess, set_membership);

    // is the new assignment the same as the old assignment?
    bool stop = match_sets(n, old_set_membership, set_membership);
    stop |= rounds > MAX_ROUNDS;
    copy_double_vectors(k, next_guess, current_guess);
    copy_int_vector(set_membership, old_set_membership);
    // Repeat until sets don't change
    if (stop)
      break;
  }

  // copy and delete temps
  copy_double_vectors(k, current_guess, out_data);
  delete []next_guess;
  delete []current_guess;
  return true;
}

bool k_means(int k, int n, vector<double>* in_data, vector<double>* out_data) {

  // Initial guess of cluster centers is first k points of input data
  return k_means_with_guess(k, n, in_data, in_data, out_data);
}
