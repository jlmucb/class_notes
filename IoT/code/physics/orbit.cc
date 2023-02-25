#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
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
  void clear();
};

void space_time_point::replace(const space_time_point& r) {
  x_ = r.x_;
  y_ = r.y_;
  z_ = r.z_;
  t_ = r.t_;
}


void space_time_point::clear() {
  x_ = 0.0;
  y_ = 0.0;
  z_ = 0.0;
  t_ = 0.0;
}

void print_point(const space_time_point& r) {
  printf("(%.2lf, %.2lf, %.2lf, %.2lf)", r.x_, r.y_, r.z_, r.t_);
}

class state {
public:
  space_time_point r_;
  space_time_point v_;
  void replace(const state& s);
  void clear();
};

void state::replace(const state& s) {
	r_.replace(s.r_);
	v_.replace(s.v_);
}

void state::clear() {
	r_.clear();
	v_.clear();
}

double dist(const space_time_point& r) {
  return sqrt(r.x_ * r.x_ + r.y_ * r.y_ + r.z_ * r.z_);
}

void print_state(const state& s) {
  printf("r: "); print_point(s.r_); printf("\n");
  printf("v: "); print_point(s.v_); printf("\n");
  printf("Height above center of earth: %.2lf km\n", dist(s.r_) / 1000.0);
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

bool normalize(const space_time_point& p, space_time_point* np) {
  double d = dist(p);
  np->x_ = p.x_ / d;
  np->y_ = p.y_ / d;
  np->z_ = p.z_ / d;
  return true;
}

bool get_acc(const double start_burn_t, const double stop_burn_t, const double delta_t,
          const double delta_v, const space_time_point& r, const space_time_point& v,
          space_time_point* a) {
  // Gravity down plus burn
  normal(r, a);
  double b = acc_grav(r);
  a->x_ *= -b;
  a->y_ *= -b;
  a->z_ *= -b;
  if (r.t_ >= start_burn_t && r.t_ <= stop_burn_t) {
    space_time_point tn;
    normalize(v, &tn);
    tn.x_ *= delta_v;
    tn.y_ *= delta_v;
    tn.z_ *= delta_v;
    a->x_ -= tn.x_;
    a->y_ -= tn.y_;
    a->z_ -= tn.z_;
  }
  return true;
}

int main(int an, char** av) {

  state cur;
  state next;

  double h =   160000;
  double T = 15000.0;
  double v0 = 17500.00 * 5280.0 * 12.0 / (39.37 * 60.0 * 60.0);
  double start_burn_t = 0.0;
  double stop_burn_t = 150.0;
  double delta_v = 100.0;
  double delta_t = .05;  // seconds

  for (int i = 0; i < an; i++) {
    if (strncmp("--h=", av[i], 4) == 0) {
      sscanf(av[i]+4, "%lf", &h);
    }
    if (strncmp("--T=", av[i], 4) == 0) {
      sscanf(av[i]+4, "%lf", &h);
    }
    if (strncmp("--v0=", av[i], 5) == 0) {
      sscanf(av[i]+5, "%lf", &v0);
    }
    if (strncmp("--start=", av[i], 8) == 0) {
      sscanf(av[i]+8, "%lf", &start_burn_t);
    }
    if (strncmp("--stop=", av[i], 7) == 0) {
      sscanf(av[i]+7, "%lf", &stop_burn_t);
    }
    if (strncmp("--delta_t=", av[i], 10) == 0) {
      sscanf(av[i]+10, "%lf", &delta_t);
    }
    if (strncmp("--delta_v=", av[i], 10) == 0) {
      sscanf(av[i]+10, "%lf", &delta_v);
    }
  }

  cur.r_.t_ = 0.0;
  cur.r_.y_ = 0.0;
  cur.r_.z_ = 0.0;
  cur.r_.x_ = h + r0;

  cur.v_.t_ = 0.0;
  cur.v_.y_ = v0;
  cur.v_.z_ = 0.0;
  cur.v_.x_ = 0.0;

  printf("\nOriginal state: %.2lf meter orbit, v0: %.2lf m/s, distances in meters, speeds in meters/second\n",
      h / 1000.0, v0);
  print_state(cur);
  for (int i = 0; (int) (T / delta_t); i++) {
    space_time_point a;
    get_acc(start_burn_t, stop_burn_t, delta_t, delta_v, cur.r_, cur.v_, &a);
    update_state(delta_t, a, cur, &next);
#if 0
    if ((i%100) == 0) {
      printf("\nTime step %d:\n", i);
      print_state(next);
      printf("\n");
      //printf("a: "); print_point(a);printf("\n");
    }
#endif
    cur.replace(next);
    if (dist(cur.r_) <= r0) {
      printf("\nBurn %.2lf m/s delta-v during (%.2lf, %.2lf) seconds\n",
          delta_v, start_burn_t, stop_burn_t);
      printf("Time step %d, %.2lf seconds to landing\n",
          i, ((double)i)*delta_t);
      print_state(next);
      printf("\n");
      // printf("a: "); print_point(a);printf("\n");
      break;
    }
  }

  return 0;
}
