#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>

const double pi = 3.14159265359;

bool generate_samples(int num_samples, double* samples, double f1, double f2, double f3) {
  double x;

  for (int i = 0; i < num_samples; i++) {
    x = cos(2 * pi * ((double) i) * f1);
    x += cos(2 * pi * ((double) i) * f2);
    x += cos(2 * pi * ((double) i) * f3);
    samples[i] = x;
  }
  return true;
}

bool filter(int num_samples, double* samples_in,
            int num_taps, double* taps, double* samples_out) {
  double y;

  for (int i = num_taps - 1; i < num_samples; i++) {
    y = 0.0;
    for (int j = 0; j < num_taps; j++) {
      y += taps[j] * samples_in[i - j];
    }
    samples_out[i] = y;
  }
  for (int i = 0; i < num_taps; i++) {
    samples_out[i] = 0.0;
  }
  return true;
}

bool low_pass_filter(int num_samples, double* samples_in, int f_d,
                     double* samples_out) {
  int num_taps = f_d;
  double* taps = new double[f_d];

  for (int i = 0; i < num_taps; i++) {
    taps[i] = 1.0 / ((double) num_taps);
  }
  
  bool ret = filter(num_samples, samples_in, num_taps, taps, samples_out);
  delete []taps;
  return ret;
}

bool high_pass_filter(int num_samples, double* samples_in, 
                      int f_d, double* samples_out) {
  int num_taps = f_d;
  double* taps = new double[num_taps];

  taps[0] = ((double) (num_taps - 1)) / ((double)num_taps);
  for (int i = 1; i < num_taps; i++) {
    taps[i] = - 1.0 / ((double) num_taps);
  }
  bool ret = filter(num_samples, samples_in, num_taps, taps, samples_out);
  delete []taps;
  return ret;
}

bool real_fft(int num_samples, double* in, double* out) {
  double a, y;
  int N = num_samples;

  for (int j = 0; j < N; j++) {
    y = 0.0;
    for (int k = 0; k < N; k++) {
      a = - 2.0 * pi * ((double) j) * ((double) k) / ((double) N);
      y += in[k] * cos(a);
    }
    out[j] =  y / ((double) N);
  }
  return true;
}

bool real_ifft(int num_samples, double* in, double* out) {
  double a, y;
  int N = num_samples;

  for (int j = 0; j < N; j++) {
    y = 0.0;
    for (int k = 0; k < N; k++) {
      a = 2.0 * pi * ((double) j) * ((double) k) / ((double) N);
      y += in[k] * cos(a);
    }
    out[j] = y;
  }
  return true;
}

int main(int an, char** av) {
  const int num_samples = 200;
  const int num_taps = 10;
  double samples_1[num_samples], samples_2[num_samples], samples_3[num_samples];
  double samples_4[num_samples], samples_5[num_samples];
  double scale = 40.0;
  
  printf("Frequency (in sample cycles): %8.4lf, %8.4lf, %8.4lf\n", 2.0/scale, 4.0/scale, 16.0/scale);

  if (!generate_samples(num_samples, samples_1, 2.0/scale, 4.0/scale, 20.0/scale)) {
    printf("Can't generate samples\n");
    return 1;
  }
  if (!low_pass_filter(num_samples, samples_1, 10, samples_2)) {
    printf("Can't filter samples\n");
    return 1;
  }
  if (!high_pass_filter(num_samples, samples_1, 10, samples_3)) {
    printf("Can't filter samples\n");
    return 1;
  }
  if (!real_fft(num_samples, samples_1, samples_4)) {
    printf("Can't fft samples\n");
    return 1;
  }
  if (!real_ifft(num_samples, samples_4, samples_5)) {
    printf("Can't fft samples\n");
    return 1;
  }

  printf("Original sample\n");
  for (int i = 0; i < num_samples; i++) {
    printf("%03d, %08.5lf\n", i, samples_1[i]);
  }
  printf("\n");
  printf("Low pass sample\n");
  for (int i = 0; i < num_samples; i++) {
    printf("%03d, %08.5lf\n", i, samples_2[i]);
  }
  printf("\n");
  printf("High pass sample\n");
  for (int i = 0; i < num_samples; i++) {
    printf("%03d, %08.5lf\n", i, samples_3[i]);
  }
  printf("\n");
  printf("fft\n");
  for (int i = 0; i < num_samples; i++) {
    printf("%03d, %08.5lf\n", i, samples_4[i]);
  }
  printf("\n");
  printf("ifft\n");
  for (int i = 0; i < num_samples; i++) {
    printf("%03d, %08.5lf\n", i, samples_5[i]);
  }
  return 0;
}

