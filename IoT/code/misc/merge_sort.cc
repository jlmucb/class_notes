#include "sort.h"
#include <stdlib.h>

void merge(int* d1, int n1, int* d2, int n2, int* d_out) {
  int i1 = 0;
  int i2 = 0;
  int n = n1 + n2;

  for(int i = 0; i < n; i++) {
    if (i1 >= n1) {
      d_out[i] = d2[i2++];
      continue;
    }
    if (i2 >= n2) {
      d_out[i] = d1[i1++];
      continue;
    }
    if (d1[i1] < d2[i2]) {
      d_out[i] = d1[i1++];
    } else {
      d_out[i] = d2[i2++];
    }
  }
}

void merge_sort(int* data, int size, int* out) {
  int i;

  if (size < 10) {
    for (i = 0; i < size; i++)
      out[i]= data[i];
    bubble_sort(out, size);
    return;
  }

  int k1 = size / 2;
  int k2 = size - k1;

  int* t_data1 = (int*)malloc(k1 * sizeof(int));
  int* t_data2 = (int*)malloc(k2 * sizeof(int));
  int* t_out1 = (int*)malloc(k1 * sizeof(int));
  int* t_out2 = (int*)malloc(k2 * sizeof(int));

  for (i = 0; i < k1; i++)
    t_data1[i] = data[i];
  for (i = 0; i < k2; i++)
    t_data2[i] = data[i + k1];
  merge_sort(t_data1, k1, t_out1);
  merge_sort(t_data2, k2, t_out2);
  merge(t_out1, k1, t_out2, k2, out);

  free(t_data1);
  free(t_data2);
  free(t_out1);
  free(t_out2);
}

