#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <stdlib.h>


//
// baum.cc
// (c) copyright, 2017, John L. Manferdelli


#ifndef nullptr
#define nullptr NULL
#endif


double rand_in_interval() {
  return drand48();
}

void copy_array(int size, double* from_a, double* to_a) {
  for (int i = 0; i < size; i++) {
    to_a[i] = from_a[i];
  }
}

// Pr(j|i)
double get_a(double* a, int num_states, int i, int j) {
  return a[num_states * (i - 1) + j - 1];
}

// Pr(o|s)
double get_b(double* b, int num_states, int num_outputs, int s, int o) {
  return b[num_outputs * (s - 1) + o - 1];
}

double get_p(double* p, int num_states, int s) {
  return p[(s - 1)];
}

void put_a(double* a, int num_states, int i, int j, double v) {
  a[num_states * (i - 1) + j - 1] = v;
}

void put_b(double* b, int num_states, int num_outputs, int s, int o, double v) {
  b[num_outputs * (s - 1) + o - 1] = v;
}

void put_p(double* p, int num_states, int s, double v) {
  p[(s - 1)] = v;
}

double get_dist(int N, double* dist, int i) {
  return dist[i - 1];
}

int select_from_distribution(int N, double* dist) {
  double z = 0.0;
  double x = rand_in_interval();

  for (int j = 1; j <= N; j++) {
    z += get_dist(N, dist, j);
    if (x <= z) {
      return j;
    }
  }
  return -1;
}

double simple_likelihood(int num_states, int num_outputs, double* a, double* b, double* p,
                         int T, int* states, int* observations) {
  double x_p = 1.0;
  int old_state, current_state;

  for (int i = 0; i < T; i++) {
    if (i == 0) {
      x_p *= get_p(p, num_states, states[i]);
    } else {
      x_p *= get_a(a, num_states, states[i - 1], states[i]);
    }
    x_p *= get_b(b, num_states, num_outputs, states[i], observations[i]);
  }
  return x_p;
}

bool gen_observations(int num_states, int num_outputs, int T, double* p,
                      double* a, double* b, int* states, int* observations) {
  int current_state, old_state;
  int o;

  for (int j = 0; j < T; j++) {
    if (j == 0) {
      current_state = select_from_distribution(num_states, p);
    } else {
      current_state = select_from_distribution(num_states, &a[(old_state - 1) * num_states]);
    }
    states[j] = current_state;
    o = select_from_distribution(num_outputs, &b[(current_state - 1) * num_outputs]); 
    observations[j] = o;
    old_state = current_state;
  }
  return true;
}

void print_a(int num_states, double* a) {
  for (int i = 1; i <= num_states; i++) {
    for (int j = 1; j <= num_states; j++) {
     printf("%e ", get_a(a, num_states, i, j)) ;
    }
    printf("\n");
  }
  printf("\n");
}

void print_b(int num_states, int num_outputs, double* b) {
  for (int i = 1; i <= num_states; i++) {
    for (int j = 1; j <= num_outputs; j++) {
     printf("%e ", get_b(b, num_states, num_outputs, i, j));
    }
    printf("\n");
  }
  printf("\n");
}

void print_p(int num_states, double* p) {
  for (int i = 1; i <= num_states; i++) {
     printf("%e ", get_p(p, num_states, i));
  }
  printf("\n");
}

void print_alpha(int num_states, double* alpha) {
  print_p(num_states, alpha);
}

void print_beta(int num_states, double* beta) {
  print_p(num_states, beta);
}

void print_gamma(int num_states, double* gamma) {
  print_p(num_states, gamma);
}

//  alpha_t(i) = Pr(o1 o2 ... Ot, i=q | lambda)
//  alpha_1(i) = p(i) b(i, O_1)
//  alpha_t+1(j) = sum j from 1 to N [alpha_t(i) a_ij] b_j,O_t+1
bool calc_alpha(int t, double* a, double* b, double* p, int num_states,
                  int num_out, int T, int ot, double* alpha_array) {
  double* new_alpha = &alpha_array[num_states * (t - 1)];
  if (t == 1) {
    for (int i = 1; i <= num_states; i++) {
      new_alpha[i - 1] = get_p(p, num_states, i) * get_b(b, num_states, num_out, i, ot);
    }
    return true;
  }
  // recursion
  double* prev_alpha = &alpha_array[num_states * (t - 2)];
  for (int i = 1; i <= num_states; i++) {
    double acc = 0.0;
    for (int k = 1; k <= num_states; k++) {
      acc += prev_alpha[k - 1] * get_a(a, num_states, k, i) * 
             get_b(b, num_states, num_out, i, ot);
    }
    new_alpha[i - 1] = acc;
  }
  return true;
}

//  beta_t(i) = sum j from 1 to N [a_ij b_j,O_t+1]
//  beta_T(i) = 1
//  beta_t(i) = sum j from 1 to N [a_ij b_jO_t+1
bool calc_beta(int t, double* a, double* b, double* p, int num_states,
                 int num_out, int T, int ot, double* beta_array) {
  double* new_beta = &beta_array[num_states * (t - 1)];
  if (t == T) {
    for (int i = 0; i < num_states; i++) {
      new_beta[i] = 1.0;
    }
    return true;
  }
  // recursion
  double* prev_beta = &beta_array[num_states * t];
  for (int i = 1; i <= num_states; i++) {
    double acc = 0.0;
    for (int k = 1; k <= num_states; k++) {
      acc += prev_beta[k - 1] * get_a(a, num_states, i, k) *
             get_b(b, num_states, num_out, k, ot);
    }
    new_beta[i - 1] = acc;
  }
  return true;
}

//  gamma_t(j) = Pr(s_t=i| O, lambda)
//  gamma_t(i) = (alpha_t(i) beta_t(i)) / P(O| lambda)
bool calc_gamma(int t, double* alpha_array, double* beta_array, int num_states,
                int T, double* gamma_array) {
  double* alpha_p = &alpha_array[t - 1];
  double* beta_p = &beta_array[t - 1];
  double* gamma_p = &gamma_array[t - 1];
  double acc = 0.0;

  for (int k = 0; k < num_states; k++) {
    acc += alpha_p[k] * beta_p[k];
  }
  for (int i = 0; i < num_states; i++) {
    gamma_p[i] = alpha_p[i] * beta_p[i] / acc;
  }
  return true;
}

//  chi_t(i, j) = Pr(s_t=i, s_t+1 = j, | O, lambda)
//  chi_t(i, j) = [alpha_t(i) aij b_j,t+1 beta_t+1(j)] / Pr(O | lambda)
bool calc_chi(int t, double* alpha, double* beta, double likelihood, double* chi_out) {
  return true;
}

bool reestimate(int num_states, int num_outputs, int T, int* obs, double* p,
                double* a, double* b, double* new_p, double* new_a, double* new_b) {

  double* alpha_array = new double[num_outputs * T];
  double* beta_array = new double[num_outputs * T];
  double* gamma_array = new double[num_outputs * T];
  double* alpha_beta_acc = new double[num_outputs];
  bool fResult = true;

  // calculate alpha_T, beta_T, alpha_(T-1), beta_(t-1)
  if (!calc_alpha(1, a, b, p, num_states, num_outputs, T, obs[0], alpha_array)) {
    fResult = false;
    goto done;
  }
  for (int t = 2; t <= T; t++) {
    if (!calc_alpha(t, a, b, p, num_states, num_outputs, T, obs[t - 1], alpha_array)) {
      fResult = false;
    goto done;
    }
  }
  if (!calc_beta(T, a, b, p, num_states, num_outputs, T, obs[T - 1], beta_array)) {
    fResult = false;
    goto done;
  }
  for (int t = (T - 1); t > 0; t--) {
    if (!calc_beta(t, a, b, p, num_states, num_outputs, T, obs[t], beta_array)) {
      fResult = false;
      goto done;
    }
  }
#if 1
    printf("alpha\n");
    for (int t = 0; t < T; t++) {
      printf("t: %d  ", t);
      print_alpha(num_states, &alpha_array[num_states * t]);
    }
    printf("\n");
    printf("beta\n");
    for (int t = 0; t < T; t++) {
      printf("t: %d  ", t);
      print_beta(num_states, &beta_array[num_states * t]);
    }
    printf("\n");
#endif

  // calculate gamma
  if (!calc_gamma(1, alpha_array, beta_array, num_states, T, gamma_array)) {
      fResult = false;
      goto done;
  }
#if 1
  printf("gamma_%02d: ", 0);
  print_gamma(num_states, &gamma_array[0]);
#endif

  // reestimated p
  for (int i = 0; i < num_states; i++) {
    new_p[i] = gamma_array[i];
  }
  printf("\n");

  // reestimated a
  for (int i = 1; i <= num_states; i++) {
    alpha_beta_acc[i - 1] = 0.0;
    for(int t = 0; t < (T - 1); t++) {
      alpha_beta_acc[i - 1] += alpha_array[num_states * t + i - 1] * beta_array[num_states * t + i - 1];
    }
  }
#if 1
    printf("alpha_beta_acc: ");
    print_gamma(num_states, alpha_beta_acc);
#endif

  for (int i = 1; i <= num_states; i++) {
    for (int j = 1; j <= num_states; j++) {
      double temp = 0.0;
      for(int t = 0; t < (T - 1); t++) {
        temp += alpha_array[num_states * t + i - 1] * 
            get_a(a, num_states, i, j) * get_b(b, num_states, num_outputs, j, obs[t + 1]) *
            beta_array[num_states * t + j - 1];
      }
      put_a(new_a, num_states, i, j, temp / alpha_beta_acc[i - 1]);
    }
    double temp2 = 0.0;
    for (int j = 1; j <= num_states; j++) {
      temp2 += get_a(new_a, num_states, i, j);
    }
    double temp1;
    for (int j = 1; j <= num_states; j++) {
      temp1 = get_a(new_a, num_states, i, j);
      put_a(new_a, num_states, i, j, temp1 / temp2);
    }
  }

  // reestimated b
  for (int i = 1; i <= num_states; i++) {
    for (int j = 1; j <= num_outputs; j++) {
      double temp1 = 0.0;
      double temp2 = 0.0;
      for (int t = 0; t < T; t++) {
        temp2 += alpha_array[num_states * t + i - 1] * beta_array[num_states * t + i - 1];
      }
      for (int t = 0; t < (T - 1); t++) {
        if (obs[t + 1] == j) {
          temp1 += alpha_array[num_states * t + i - 1] * beta_array[num_states * t + i - 1];
        }
      }
      put_b(new_b, num_states, num_outputs, i, j, temp1 / temp2);
    }
    double temp2 = 0.0;
    for (int j = 1; j <= num_outputs; j++) {
      temp2 += get_b(new_b, num_states, num_outputs, i, j);
    }
    double temp1;
    for (int j = 1; j <= num_outputs; j++) {
      temp1 = get_b(new_a, num_states, num_outputs, i, j);
      put_b(new_b, num_states, num_outputs, i, j, temp1 / temp2);
    }
  }

done:
  delete []alpha_beta_acc;
  delete []alpha_array;
  delete []beta_array;
  delete []gamma_array;
  return fResult;
}

bool stopping_rule(int num_states, int num_outputs, double* old_p, double* p,
                   double* old_a, double* a, double* old_b, double* b) {
  return true;
}

void test_select(double* test_dist, int num_outcomes, int num_selects) {
  printf("Distribution: ");
  print_p(num_outcomes, test_dist);
  printf("\n");

  int *count = new int[num_outcomes];
  for (int i = 0; i < num_outcomes; i++)
    count[i] = 0;

  int out;
  for (int i = 0; i < num_selects; i++) {
    out = select_from_distribution(num_outcomes, test_dist);
    count[out - 1]++;
  }

  for (int i = 0; i < num_outcomes; i++) {
    double x = ((double) count[i])/((double)num_selects);
    printf("%e  ", x);
  }
  printf("\n");
  delete []count;
}

// A[3*i+j] = Pr(j|i)
double A[] = {
  0.1, 0.5, 0.4,
  0.2, 0.2, 0.6,
  0.1, 0.1, 0.8
  };

// b[3*i+j] = Pr(o=j|s=i)
double B[] = {
     .8, .1, .1,
     .1, .8, .1, 
     .1, .2, .7
  };

// P[i] = pi(i)
double P[] = {
  .35, .35, .3
  };

// Pr(O|lambda) = sum from 1 to N [alpha_T(i)]

#define NT 30
#define NUM_STATES 3
#define NUM_OUTPUTS 3

int main(int an, char** av) {
  int num_states = NUM_STATES;
  int num_outputs = NUM_OUTPUTS;
  int T = NT;
  double a[NUM_STATES * NUM_STATES], b[NUM_STATES * NUM_OUTPUTS], p[NUM_STATES];
  double new_a[NUM_STATES * NUM_STATES], new_b[NUM_STATES * NUM_OUTPUTS], new_p[NUM_STATES];
  int states[NT];
  int observations[NT];
  long int rand_seed = 19;

  if (an > 1) {
    rand_seed = atoi(av[1]);
  }

#ifdef TESTDIST
  double test_dist[3] = {.3, .2, .5};
  test_select(test_dist, 3, 100);
  return 0;
#endif

  // Print original distributions
  printf("Original parameters\n");
  printf("a:\n");
  print_a(num_states, A);
  printf("b:\n");
  print_b(num_states, num_outputs, B);
  printf("p:\n");
  print_p(num_states, P);
  printf("\n");

  // init rng
  srand48(rand_seed);

  // Generate observations
  if (!gen_observations(num_states, num_outputs, T, P,
                      A, B, states, observations)) {
    printf("Can't generate sequence\n");
    return false;
  }

  // Print observations
  printf("\nStates: ");
  for (int n = 0; n < T; n++) {
    printf("%d ", states[n]);
  }
  printf("\n");
  printf("Obs   : ");
  for (int n = 0; n < T; n++) {
    printf("%d ", observations[n]);
  }
  printf("\n");
  double likelihood = simple_likelihood(num_states, num_outputs, A, B, P, T, states, observations);
  printf("Likelihood = %e\n\n", likelihood);

  // guess distributions

  // set initial guess at a, b, p
  put_a(a, num_states, 1, 1, .2); put_a(a, num_states, 1, 2, .4); put_a(a, num_states, 1, 3, .6);
  put_a(a, num_states, 2, 1, .2); put_a(a, num_states, 2, 2, .3); put_a(a, num_states, 2, 3, .5);
  put_a(a, num_states, 3, 1, 0); put_a(a, num_states, 3, 2, .1); put_a(a, num_states, 3, 3, .9);
  put_b(b,num_states,num_outputs,1,1,0.5); put_b(b,num_states,num_outputs,1,2,0.2); put_b(b,num_states,num_outputs,1,3,0.3);
  put_b(b,num_states,num_outputs,2,1,0.1); put_b(b,num_states,num_outputs,2,2,0.6); put_b(b,num_states,num_outputs,2,3,0.3);
  put_b(b,num_states,num_outputs,3,1,0.3); put_b(b,num_states,num_outputs,3,2,0.2); put_b(b,num_states,num_outputs,3,3,0.5);
  put_p(p, num_states, 1, 0.33); put_p(p, num_states, 2, 0.63); put_p(p, num_states, 3, 0.04);

  // Print initial guess
  printf("Initial guess of distributions\n");
  printf("a:\n");
  print_a(num_states, a);
  printf("b:\n");
  print_b(num_states, num_outputs, b);
  printf("p:\n");
  print_p(num_states, p);
  printf("\n");

  for (int n = 0; n < 50; n++) {
    //  reestimate
    if (!reestimate(num_states, num_outputs, T, observations, p,
                    a, b, new_p, new_a, new_b)) {
      printf("reestimate failed\n");
      return 1;
    }

    // print reestimate
    printf("\nReestimated parameters, round %d\n", n);
    printf("a:\n");
    print_a(num_states, new_a);
    printf("b:\n");
    print_b(num_states, num_outputs, new_b);
    printf("p:\n");
    print_p(num_states, new_p);
    printf("\n");

    if (n > 2){
// (stopping_rule(num_states, num_outputs, p, new_p, a, new_a, b, new_b)) ||
      break;
    }

    //  copy
    copy_array(num_states * num_states, new_a, a);
    copy_array(num_states * num_outputs, new_b, b);
    copy_array(num_states, new_p, p);
  }
  
  return 0;
}

// ---------------------------------------------------------------------------

