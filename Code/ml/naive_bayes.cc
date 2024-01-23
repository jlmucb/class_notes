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

//  Naive Bayes
//  Pr(yes|d) = (Pr(d.f1|yes) Pr(yes) / Pr(d.f1)) (Pr(d.f2|yes) Pr(yes) / Pr(d.f2))
//      ... (Pr(d.fn|yes) Pr(yes) / Pr(d.fn))
//  Pr(no|d) = (Pr(d.f1|no) Pr(no) / Pr(d.f1)) (Pr(d.f2|no) Pr(no) / Pr(d.f2)) 
//     ... (Pr(d.fn|no) Pr(no) / Pr(d.fn))
//  odds = Pr(yes|d)/ (Pr(yes|d) + Pr(no|d))
//  be sure to laplace smooth

// Model is Prob(yes), Prob(no)
// vector of features
//  bool:
//    Prob(yes)
//    Prob(no)
//  int
//    Prob (f = 1), Prob (f = 2), ...
//  bucketized_double
//    Prob (f is in bucket 1), Prob (f is in bucket 2), ...

#define SMOOTH
#define SMOOTHING_FACTOR 4.0

feature_descriptor* find_outcome_feature(feature_table& table) {
  feature_descriptor* fd;
  for (int i = 0; i < table.get_num_features(); i++) {
    fd = table.get_feature(i);
    if (fd->get_feature_name() == string("outcome"))
      return fd;
  }
  return nullptr;
}

probability_value* calculate_absolute_bool_probs(feature_descriptor& fd) {
  probability_value* pv = new probability_value();
  pv->feature_type_ = feature_descriptor::feature_type_bool;

  int yes = 0;
  int no = 0;
  int total = 0;
  for (int j = 0; j < fd.get_num_bool_entries(); j++) {
      total++;
    if (fd.get_bool_feature_value(j)) {
      yes++;
    } else {
      no++;
    }
  }
  double prob_yes = ((double)yes) / ((double)total);
  double prob_no = ((double)no) / ((double)total);
  pv->pv_.push_back(prob_yes);
  pv->pv_.push_back(prob_no);
#ifdef SMOOTH
  // smoothing
  double factor = 1.0 / (((double)2) * SMOOTHING_FACTOR);
  double scale = 0.0;
  for (int i = 0; i < 2; i++) {
    pv->pv_[i] += factor;
    scale += pv->pv_[i];
  }
  for (int i = 0; i < 2; i++) {
    pv->pv_[i] /= scale;
  }
#endif
  return pv;
}

probability_value* calculate_absolute_int_probs(feature_descriptor& fd) {
  probability_value* pv = new probability_value();
  pv->feature_type_ = feature_descriptor::feature_type_int;
  vector<int> counts;
  int n = fd.get_int_feature_top_range() - fd.get_int_feature_bottom_range() + 1;
  for (int i = 0; i < n; i++)
    counts.push_back(0);

  // tally
  int k;
  int total = 0;
  for (int j = 0; j < fd.get_num_int_entries(); j++) {
    k = fd.get_int_feature_value(j);
    k -= fd.get_int_feature_bottom_range();
    total++;
    (counts[k])++;
  }
  for (int i = 0; i < n; i++) {
    double ans;
    if (total > 0) 
      ans = ((double)counts[i]) / ((double)total);
    else
      ans = 0;
    pv->pv_.push_back(ans);
  }
#ifdef SMOOTH
  // smoothing
  double factor = 1.0 / (((double)n) * SMOOTHING_FACTOR);
  double scale = 0.0;
  for (int i = 0; i < n; i++) {
    pv->pv_[i] += factor;
    scale += pv->pv_[i];
  }
  for (int i = 0; i < n; i++) {
    pv->pv_[i] /= scale;
  }
#endif
  return pv;
}

probability_value* calculate_absolute_double_probs(feature_descriptor& fd) {
  probability_value* pv = new probability_value();
  pv->feature_type_ = feature_descriptor::feature_type_double;
  vector<int> counts;
  int n = fd.get_number_of_buckets();
  for (int i = 0; i < n; i++)
    counts.push_back(0);

  // tally
  int k;
  int total = 0;
  for (int j = 0; j < fd.get_num_double_entries(); j++) {
    k = fd.get_bucket_number(fd.get_double_feature_value(j));
    if (k > 0) {
      total++;
      (counts[k])++;
    }
  }
   
  for (int i = 0; i < n; i++) {
    pv->pv_.push_back(((double) counts[i]) / ((double)total));
  }
#ifdef SMOOTH
  // smoothing
  double factor = 1.0 / (((double)n) * SMOOTHING_FACTOR);
  double scale = 0.0;
  for (int i = 0; i < n; i++) {
    pv->pv_[i] += factor;
    scale += pv->pv_[i];
  }
  for (int i = 0; i < n; i++) {
    pv->pv_[i] /= scale;
  }
#endif
  return pv;
}

probability_value* calculate_conditional_bool_probs(bool supposition,
      feature_descriptor& fd, feature_descriptor& od) {
  probability_value* pv = new probability_value();
  pv->feature_type_ = feature_descriptor::feature_type_bool;

  int yes = 0;
  int no = 0;
  int total = 0;

  for (int j = 0; j < fd.get_num_bool_entries(); j++) {
    if (supposition == od.get_bool_feature_value(j)) {
      total++;
      if (fd.get_bool_feature_value(j)) {
        yes++;
      } else {
        no++;
      }
    }
  }
  double prob_yes = ((double)yes) / ((double)total);
  double prob_no = ((double)no) / ((double)total);
#ifdef SMOOTH
  // smoothing
  double factor = 1.0 / (((double)2) * SMOOTHING_FACTOR);
  prob_yes += factor;
  prob_no += factor;
  prob_yes /= (prob_yes + prob_no);
  prob_no /= (prob_yes + prob_no);
#endif
  pv->pv_.push_back(prob_yes);
  pv->pv_.push_back(prob_no);
  return pv;
}

probability_value* calculate_conditional_int_probs(bool supposition,
    feature_descriptor& fd, feature_descriptor& od) {
  probability_value* pv = new probability_value();
  pv->feature_type_ = feature_descriptor::feature_type_int;
  vector<int> counts;
  int n = fd.get_int_feature_top_range() - fd.get_int_feature_bottom_range() + 1;
  for (int i = 0; i < n; i++)
    counts.push_back(0);

  // tally
  int k;
  int total = 0;
  for (int j = 0; j < fd.get_num_int_entries(); j++) {
    k = fd.get_int_feature_value(j);
    k -= fd.get_int_feature_bottom_range();
    if (supposition == od.get_bool_feature_value(j)) {
      total++;
      (counts[k])++;
    }
  }
  for (int i = 0; i < n; i++) {
    double ans;
    if (total > 0) 
      ans = (double)(counts[i]) / (double)(total); // smoothing
    else
      ans = 0;
    pv->pv_.push_back(ans);
  }
#ifdef SMOOTH
  // smoothing
  double factor = 1.0 / (((double)n) * SMOOTHING_FACTOR);
  double scale = 0.0;
  for (int i = 0; i < n; i++) {
    pv->pv_[i] += factor;
    scale += pv->pv_[i];
  }
  for (int i = 0; i < n; i++) {
    pv->pv_[i] /= scale;
  }
#endif
  return pv;
}

probability_value* calculate_conditional_double_probs(bool supposition,
    feature_descriptor& fd, feature_descriptor& od) {
  probability_value* pv = new probability_value();
  pv->feature_type_ = feature_descriptor::feature_type_double;
  vector<int> counts;
  int n = fd.get_number_of_buckets();
  for (int i = 0; i < n; i++)
    counts.push_back(0);

  // tally
  int k;
  int total = 0;
  for (int j = 0; j < fd.get_num_double_entries(); j++) {
    if (supposition == od.get_bool_feature_value(j)) {
      k = fd.get_bucket_number(fd.get_double_feature_value(j));
      if (k > 0) {
        total++;
        (counts[k])++;
      }
    }
  }
  for (int i = 0; i < n; i++) {
    pv->pv_.push_back(((double) counts[i]) / ((double) total));
  }
#ifdef SMOOTH
  // smoothing
  double factor = 1.0 / (((double)n) * SMOOTHING_FACTOR);
  double scale = 0.0;
  for (int i = 0; i < n; i++) {
    pv->pv_[i] += factor;
    scale += pv->pv_[i];
  }
  for (int i = 0; i < n; i++) {
    pv->pv_[i] /= scale;
  }
#endif
  return pv;
}

bayes_probability_model::bayes_probability_model(feature_table& table) {
  table_ = &table;
}

bayes_probability_model::~bayes_probability_model() {
}

void bayes_probability_model::print_bayes_model() {
  printf("Bayes Probability Model\n");
  printf("Prob(yes): %8.4f, Prob(no): %8.4f\n", prob_yes_, prob_no_);
  printf("\nConditional probabilities:\n");
  for (int i = 0; i < (int)conditional_true_.size(); i++) {
    printf("given true, "); conditional_true_[i].print_probability_value();
    printf("given false, "); conditional_false_[i].print_probability_value();
  }
  printf("\nAbsolute probabilities:\n");
  for (int i = 0; i < (int)absolute_true_.size(); i++) {
    absolute_true_[i].print_probability_value();
  }
}

bool bayes_probability_model::compute_bayes_probabilties() {
  // find outcome feature and tally yes/no probabilities
  if (table_ == nullptr)
    return false;
  feature_descriptor* out = find_outcome_feature(*table_);
  if (out == nullptr)
    return false;

  int yes = 0;
  int no = 0;
  for (int j = 0; j < out->get_num_bool_entries(); j++) {
    if (out->get_bool_feature_value(j)) {
      yes++;
    } else {
      no++;
    }
  }
  prob_yes_ = ((double)yes) / ((double) out->get_num_bool_entries());
  prob_no_ = ((double)no) / ((double) out->get_num_bool_entries());

  // go thru features and calculate probabilities
  feature_descriptor* fd = nullptr;
  for (int i = 0; i < table_->get_num_features(); i++) {
    fd = table_->get_feature(i);
    // skip outcome
    if (fd->get_feature_name() == string("outcome"))
      continue;

    // Calculate probabilities
    probability_value* pv;
    switch(fd->get_feature_type()) {
      case feature_descriptor::feature_type_bool:
        pv = calculate_conditional_bool_probs(true, *fd, *out);
        if (pv != nullptr)
          conditional_true_.push_back(*pv);
        pv = calculate_conditional_bool_probs(false, *fd, *out);
        if (pv != nullptr)
          conditional_false_.push_back(*pv);
        pv = calculate_absolute_bool_probs(*fd);
        if (pv != nullptr)
          absolute_true_.push_back(*pv);
        break;
      case feature_descriptor::feature_type_int:
        pv = calculate_conditional_int_probs(true, *fd, *out);
        if (pv != nullptr)
          conditional_true_.push_back(*pv);
        pv = calculate_conditional_int_probs(false, *fd, *out);
        if (pv != nullptr)
          conditional_false_.push_back(*pv);
        pv = calculate_absolute_int_probs(*fd);
        if (pv != nullptr)
          absolute_true_.push_back(*pv);
        break;
      case feature_descriptor::feature_type_double:
        pv = calculate_conditional_double_probs(true, *fd, *out);
        if (pv != nullptr)
          conditional_true_.push_back(*pv);
        pv = calculate_conditional_double_probs(false, *fd, *out);
        if (pv != nullptr)
          conditional_false_.push_back(*pv);
        pv = calculate_absolute_double_probs(*fd);
        if (pv != nullptr)
          absolute_true_.push_back(*pv);
        break;
    }
  }
  return true;
}

double bayes_probability_model::naive_bayes_odds_yes(vector<feature_value>& v) {
  double prob_yes_given_pv = 1.0;
  double prob_no_given_pv = 1.0;

  printf("naive_bayes_odds_yes, input:\n");
  for (int i = 0; i < (int)v.size(); i++) {
    printf("\tindex: %d, ", i);
    v[i].print_feature_value();
  }
  if (v.size() != absolute_true_.size()) {
    printf("Bad input value\n");
    return -1.0;
  }

  double p_v_given_true;
  double p_v_given_false;
  double p_v;
  int bucket_num;
  // prob_yes_given_pv = prod P[v1|true] * P(true) / P(v1)
  // prob_no_given_pv = prod P[v1|false] * P(false) / P(v1)
  for (int i = 0; i < (int)v.size(); i++) {
    if (v[i].feature_type_ == feature_descriptor::feature_type_bool) {
      p_v = absolute_true_[i].pv_[0];
      p_v_given_true = conditional_true_[i].pv_[0];
      p_v_given_false = conditional_false_[i].pv_[0];
    } else if (v[i].feature_type_ == feature_descriptor::feature_type_int) {
      int k = v[i].int_value_ - 1;
      p_v = absolute_true_[i].pv_[k];
      p_v_given_true = conditional_true_[i].pv_[k];
      p_v_given_false = conditional_false_[i].pv_[k];
    } else if (v[i].feature_type_ == feature_descriptor::feature_type_double) {
      bucket_num = table_->get_feature(i)->get_bucket_number(v[i].double_value_);
      p_v = absolute_true_[i].pv_[bucket_num];
      p_v_given_true = conditional_true_[i].pv_[bucket_num];
      p_v_given_false = conditional_false_[i].pv_[bucket_num];
    } else {
      continue;
    }
printf("\tfeature %d, p_v_given_true: %8.6f, p_v_given_false: %8.6f, p_v: %8.6f\n",
  i, p_v_given_true, p_v_given_false, p_v);
    if (p_v == 0.0)
      continue;
#if 1
    prob_yes_given_pv *= p_v_given_true / p_v;
    prob_no_given_pv *= p_v_given_false / p_v;
#else
    prob_yes_given_pv *= p_v_given_true * prob_yes_ / p_v;
    prob_no_given_pv *= p_v_given_false * prob_no_ / p_v;
#endif
  }
printf("\tprob_yes_given_pv: %8.6f, prob_no_given_pv: %8.6f\n", prob_yes_given_pv, prob_no_given_pv);
  double t = prob_yes_given_pv + prob_no_given_pv;
  if (t == 0)
    return -1.0;
  return prob_yes_given_pv / t;
}
