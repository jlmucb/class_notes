//
// Copyright 2014 John Manferdelli, All Rights Reserved.
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
// File: probability.cc

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <string>
#include <iostream>
#include <fstream>
#include <memory>

using std::string;


int main(int an, char** av) {
  int games[4];
  double  a_wins_prob[4], b_wins_prob[4];
  double p, q;
  double total_a_prob = 0.0;
  double total_b_prob = 0.0;
  double odds = 0.0;

  if (an < 2)
    return 1;

  sscanf(av[1],"%lf",&p);
  q = 1.0 - p;
  for (int i = 0; i < 4; i++)
    games[i]= i + 4;
  a_wins_prob[0] = pow(p, 4.0);
  a_wins_prob[1] =  4.0 * pow(p, 4.0) * pow(q, 1.0);
  a_wins_prob[2] =  10.0 * pow(p, 4.0) * pow(q, 2.0);
  a_wins_prob[3] =  20.0 * pow(p, 4.0) * pow(q, 3.0);
  b_wins_prob[0] = pow(q, 4.0);
  b_wins_prob[1] =  4.0 * pow(q, 4.0) * pow(p, 1.0);
  b_wins_prob[2] =  10.0 * pow(q, 4.0) * pow(p, 2.0);
  b_wins_prob[3] =  20.0 * pow(q, 4.0) * pow(p, 3.0);

  for (int i = 0; i < 4; i++) {
    total_a_prob += a_wins_prob[i];
    total_b_prob += b_wins_prob[i];
  }
  odds = total_a_prob / total_b_prob;

  printf("\n\np: %lf\n", p);
  printf("Number games        Prob(a wins)      Prob(b wins)\n");
  for (int i = 0; i < 4; i++) {
    printf("%d                   %6.4lf            %6.4lf\n",
          i, a_wins_prob[i], b_wins_prob[i]);
  }
  
  printf("Total               %6.4lf            %6.4lf\n", total_a_prob, total_b_prob);
  printf("Odds: %8.4lf\n", odds);
  

  return 0;
}

