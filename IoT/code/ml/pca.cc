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


//  PCA
// returns largest eigenvector of covariance matrix

bool pca(int num_samples, vector<double>* x, vector<double>* out) {
  // Calculate the means
  vector<double> means;
  int num_features = x[0].size();

  for (int i = 0; i < num_features; i++) {
    means.push_back(0.0);
  }
  for (int i = 0; i < num_samples; i++) {
    for (int j = 0; j < num_features; j++) {
      means[j] += x[i][j];
    }
  }
  for (int i = 0; i < x[0].size(); i++) {
      means[i] /= ((double)num_samples);
  }

#if 0
  // answer is largest eigenvector
  printf("Means:\n");
  printvector(means);
  printf("\n");
#endif

  double* M = new double[num_features * num_features];
  bool ret = true;

  for (int i = 0; i < num_features; i++) {
    for (int j = 0; j < num_features; j++) {
      M[joint_index(i, j, num_features , num_features)]= 0.0;
    }
  }

  for (int i = 0; i < num_features; i++) {
    for (int j = 0; j < num_features; j++) {
      for (int k = 0; k < num_samples;  k++) {
        M[joint_index(i, j, num_features , num_features)] += (x[k][i]-means[i]) * (x[k][j]-means[j]);
      }
    }
  }

  for (int i = 0; i < num_features; i++) {
    for (int j = 0; j < num_features; j++) {
      M[joint_index(i, j, num_features , num_features)] /= ((double) (num_features * num_features));
    }
  }

#if 0
  printf("Covariance matrix:\n");
  print_double_matrix(num_features, M);
  printf("\n");
#endif

  if (!maximize_form_on_sphere(num_features, M, 1.0, 0.5, out)) {
    ret = false;
    goto done;
  }

done:
  delete []M;
  return ret;
}

