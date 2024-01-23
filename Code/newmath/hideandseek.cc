#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

// hideandseek.cc
// (c) copyright, 2017, John L. Manferdelli

#ifndef nullptr
#define nullptr NULL
#endif

#ifndef byte
typedef unsigned char byte;
#endif

byte getBit(int initial_bit_offset, int bit_num, byte* in) {
  int a = (bit_num + initial_bit_offset) / 8;
  int b = (bit_num + initial_bit_offset) % 8;
  byte x = in[a];
  return (x >> (7 - b)) & 1;
}

void printBits(int total_bits, int initial_bit_offset, byte* in) {
  byte x;

  for (int i = 0; i < total_bits; i++) {
    x = getBit(initial_bit_offset, i, in);
    printf("%1x", x);
  }
}

bool calc_freq(int letter_size, int num_letters, int offset, byte* in, double* f) {

  if (letter_size > (sizeof(unsigned) * 8))
    return false;

  int letter_space_size = 1 << letter_size;
  for (int i = 0; i < letter_space_size; i++)
    f[i] = 0.0;

  unsigned x;
  for (int j = 0; j < num_letters; j++) {
    x = 0;
    for (int k = 0; k < letter_size; k++) {
      x = (x << 1) | getBit(offset, (j * letter_size) + k, in);
    }
    f[x] += 1.0;
  }

  for (int i = 0; i < letter_space_size; i++)
    f[i] /= (double) num_letters;
  return true;
}

double distance_deviation(int letter_space_size, double* f) {
  double d = 0.0;
  double t;
  double nl = 1.0/((double) letter_space_size);

  for (int i = 0; i < letter_space_size; i++) {
    t = f[i] - nl;
    d += t * t;
  }
  return d;
}

// --------------------------------------------------------------------- 

#define MAX_SIZE 4096


int main(int an, char** av) {
  int letter_size = 1;
  int run_size = 0;
  int initial_bit_offset = 0;
  char* file_name = av[1];

  for (int i = 0; i < an - 1; i++) {
    if (strcmp(av[i], "--in") ==0) {
      file_name = av[++i];
    }
    if (strcmp(av[i], "--letter_size") ==0) {
      letter_size = atoi(av[++i]);
    }
    if (strcmp(av[i], "--run_size") ==0) {
      run_size = atoi(av[++i]);
    }
    if (strcmp(av[i], "--offset") ==0) {
      initial_bit_offset = atoi(av[++i]);
    }
  }
 
  byte in[MAX_SIZE]; 
  int fd = open(file_name, O_RDONLY);
  int n = read(fd, in, MAX_SIZE);
  close(fd);
  if (n <= 0)
    return 1;
  int total_bits = n * 8 - initial_bit_offset;

  int letter_space_size = 1 << letter_size;
  int total_num_letters = total_bits / letter_size;
  if (run_size == 0)
    run_size = total_num_letters;
  int run_length = letter_size * run_size;
  int num_runs = total_bits / run_length;

  printf("file: %s, offset: %d, total bits in file: %d\n", file_name, initial_bit_offset, total_bits);
  printf("letter_size: %d, letter_space_size: %d, number of letters: %d\n", 
    letter_size, letter_space_size, total_num_letters);
  printf("run_size: %d letters, %d, bits per run, %d runs\n\n", run_size, run_length, num_runs);

  double* f = new double[letter_space_size];

  int offset = initial_bit_offset;

  for (int k = 0; k < num_runs; k++) {
    int num_letters = run_length / letter_size;
    printf("\nrun %d, position: offset: %d\n", k, offset);
    printBits(run_length, offset, in);
    printf("\n");
    if (calc_freq(letter_size, num_letters, offset, in, f)) {
      printf("f:");
      for (int i = 0; i < letter_space_size; i++) {
        printf(" %6.4f", f[i]);
      }
      printf("\n");
      printf("Deviation: %10.8f\n", distance_deviation(letter_space_size, f));
    } else {
    printf("can't calc-freq\n");
    }
    offset += run_length;
  }

  delete []f;
  return 0;
}

// ----------------------------------------------------------------------

