#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "tgmath.h"
#include <vector>
using std::vector;
using std::string;


//  Copyright (c) 2021-22, John Manferdelli.  All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

class space_time_point {
public:
  double x_;
  double y_;
  double z_;
  double t_;
  void replace(const space_time_point& r);
};

void space_time_point::replace(const space_time_point& r) {
  x_ = r.x_;
  y_ = r.y_;
  z_ = r.z_;
  t_ = r.t_;
}

void print_point(const space_time_point& r) {
  printf("(%10.2lf, %10.2lf, %10.2lf, %10.2lf)", r.x_, r.y_, r.z_, r.t_);
}

class state {
public:
  space_time_point r_;
  space_time_point v_;
};

double dist(const space_time_point& r) {
  return sqrt(r.x_ * r.x_ + r.y_ * r.y_ + r.z_ * r.z_);
}

void print_state(const state& s) {
  printf("r: "); print_point(s.r_); printf("\n");
  printf("v: "); print_point(s.v_); printf("\n");
  printf("Distance: %lf km\n", dist(s.r_) / 1000.0);
}

const double G = 6.743e-11;
const double M = 6e24;

double acc_grav(const space_time_point& r) {
  double d2 = (r.x_ * r.x_ + r.y_ * r.y_ + r.z_ * r.z_);
  return G * M / d2;
}

bool tangent(const double theta, space_time_point* t) {
  t->x_ = -sin(theta);
  t->y_ = cos(theta);
  t->z_ = 0.0;
  return true;
}

bool normal(const double theta, space_time_point* t) {
  t->x_ = cos(theta);
  t->y_ = sin(theta);
  t->z_ = 0.0;
  return true;
}

bool tangent(const space_time_point& u, space_time_point* t) {
  double d = sqrt(u.x_ * u.x_ + u.y_ * u.y_ + u.z_ * u.z_);
  t->x_ = -u.y_ / d;
  t->y_ = u.x_ / d;
  t->z_ = u.z_ / d;
  return true;
}

bool normal(const space_time_point& u, space_time_point* t) {
  double d = sqrt(u.x_ * u.x_ + u.y_ * u.y_ + u.z_ * u.z_);
  t->x_ = u.x_ / d;
  t->y_ = u.y_ / d;
  t->z_ = u.z_ / d;
  return true;
}

bool update_state(double delta_t, const space_time_point& acc, const state& old, state* next) {
  next->r_.t_ = old.r_.t_ + delta_t;
  next->v_.t_ = old.v_.t_ + delta_t;

  next->v_.x_ = old.v_.x_ + delta_t * acc.x_;
  next->v_.y_ = old.v_.y_ + delta_t * acc.y_;
  next->v_.z_ = old.v_.z_ + delta_t * acc.z_;

  next->r_.x_ = old.r_.x_ + delta_t * (old.v_.x_ + next->v_.x_) / 2.0;
  next->r_.y_ = old.r_.y_ + delta_t * (old.v_.y_ + next->v_.y_) / 2.0;
  next->r_.z_ = old.r_.z_ + delta_t * (old.v_.z_ + next->v_.z_) / 2.0;
  return true;
}

const double r0 = 6378000;
const double h = 160000;
const double v0 = 17500.00 * 5280.0 * 12.0 / 39.37;
const double T = 12000.0;

const double start_burn_t = 0.0;
const double stop_burn_t = 200.0;
const double delta_v = 10000.0;

bool get_acc(const space_time_point& r, space_time_point* a) {
  // Gravity down plus burn
  normal(r, a);
  double x = acc_grav(r);
  a->x_ *= -x;
  a->y_ *= -x;
  a->z_ *= -x;
  if (r.t_ >= start_burn_t && r.t_ <= stop_burn_t) {
    space_time_point tn;
    tangent(r, &tn);
    tn.x_ *= delta_v;
    tn.y_ *= delta_v;
    tn.z_ *= delta_v;
    a->x_ -= tn.x_;
    a->y_ -= tn.y_;
    a->z_ -= tn.z_;
  }
  printf("a: "); print_point(*a);printf("\n");
  return true;
}

int main(int an, char** av) {

  state cur;
  state next;
  double delta_t = .05;  // seconds

  cur.r_.t_ = 0.0;
  cur.r_.y_ = 0.0;
  cur.r_.z_ = 0.0;
  cur.r_.x_ = h + r0;

  cur.v_.t_ = 0.0;
  cur.v_.y_ = v0;
  cur.v_.z_ = 0.0;
  cur.v_.x_ = 0.0;

  printf("v0: %10.2lf\n", v0);
  printf("Original state:\n");
  print_state(cur);
  printf("\n");
  for (int i = 0; (int) (T / delta_t); i++) {
    space_time_point a;
    get_acc(cur.r_, &a);
    update_state(delta_t, a, cur, &next);
    printf("Time step %d:\n", i);
    print_state(next);
    printf("\n");
  }

  return 0;
}
