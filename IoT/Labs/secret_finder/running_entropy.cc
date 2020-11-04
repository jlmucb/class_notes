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

#ifndef byte
typedef unsigned char byte;
#endif

// running_entropy.cc
// (c) copyright, 2019, John L. Manferdelli

#define MAX_SIZE 4096
#define MAX_SLOTS 10

void print_tally(int num_buckets, int* bucket) {
  printf("tally, %d buckets: ", num_buckets);
  for (int i = 0; i < num_buckets; i++)
    printf("%02d ", bucket[i]);
  printf("\n");
}

void tally_counts(byte* start, int byte_span_size, int bit_run_size, int num_buckets, int* bucket) {
  for (int i = 0; i < byte_span_size; i++) {
    byte to_count = start[i];
    int i1 = to_count >> 4;
    int i2 = to_count &0xff;
    bucket[i1]++;
    bucket[i2]++;
  }
}

void add_to_slots(int max_slots, int& num_slots_filled,
    int current_position,
    int* slot_position, double* slot_entropy, 
    double caclulated_entropy) {

  if (num_slots_filled < max_slots) {
      slot_entropy[num_slots_filled] = -1.0;
      num_slots_filled++;
  }
  int insert_position = 0;
  for (insert_position  = 0; insert_position < num_slots_filled; insert_position++) {
    if (caclulated_entropy > slot_entropy[insert_position])
      break;
  }

  if (insert_position >= (max_slots - 1))
    return;
  for (int j = (num_slots_filled - 1); j > insert_position; j--) {
    slot_position[j] = slot_position[j - 1];
    slot_entropy[j] = slot_entropy[j - 1];
  }
  slot_position[insert_position] =  current_position;
  slot_entropy[insert_position] = caclulated_entropy;
} 

void clear_buckets(int num_buckets, int* bucket) {
  for (int i = 0; i < num_buckets; i++)
    bucket[i] = 0;
}

double calculate_entropy(int total_examined, int num_buckets, int* bucket) {
  double ent = 0.0;
  for (int i = 0; i < num_buckets; i++) {
    if (bucket[i] == 0)
      continue;
    double t = ((double) bucket[i]) / ((double) total_examined);
    ent += - t * log(t);
  }
  ent /= log(2.0);
  return ent;
}

int main(int an, char** av) {
  int bit_run_size = 4;
  char* file_name = nullptr;
  int num_slots_filled = 0;
  int max_slots = MAX_SLOTS;
  int slot_position[MAX_SLOTS];
  double slot_entropy[MAX_SLOTS];
  int byte_span_size = 16;

  for (int i = 0; i < an - 1; i++) {
    if (strcmp(av[i], "--in") == 0) {
      file_name = av[++i];
    }
    if (strcmp(av[i], "--bit_run_size") == 0) {
      bit_run_size = atoi(av[++i]);
    }
    if (strcmp(av[i], "--byte_span_size") == 0) {
      byte_span_size = atoi(av[++i]);
    }
  }
 
  if (file_name == nullptr) {
    printf("No file specified\n");
    return 1;
  }
  printf("File: %s, %d bit multigrams, span: %d bytes\n", file_name, bit_run_size, byte_span_size);

  byte in[MAX_SIZE]; 
  int fd = open(file_name, O_RDONLY);
  if (fd < 0) {
    printf("Can't read %s\n", file_name);
    return 1;
  }
  int n = read(fd, in, MAX_SIZE);
  close(fd);

  int num_buckets = 1 << bit_run_size;
  int* bucket = new int[num_buckets];
  double calculated_entropy;

  int byte_position = 0;
  int total_examined = (8 * byte_span_size) / bit_run_size;
  for (byte_position = 0; byte_position < (n - byte_span_size - 1); byte_position++) {
    clear_buckets(num_buckets, bucket);
    tally_counts(&in[byte_position], byte_span_size, bit_run_size, num_buckets, bucket);
    calculated_entropy = calculate_entropy(total_examined, num_buckets, bucket);
#if 0
    print_tally(num_buckets, bucket);
    printf("Ent: %lf\n", calculated_entropy);
#endif
    add_to_slots(max_slots, num_slots_filled, byte_position, slot_position,
      slot_entropy, calculated_entropy);
  }

  printf("Largest entropy positions\n");
  for (int i = 0; i < num_slots_filled; i++) {
    printf("  position: %d(%x)  entropy: %07.4lf\n", slot_position[i],
      slot_position[i], slot_entropy[i]);
  }

  delete []bucket;
  return 0;
}

