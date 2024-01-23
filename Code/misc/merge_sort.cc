#include "sort.h"
#include <stdlib.h>

void merge(byte** first_data_half, int size_first_data_half,
           byte** second_data_half, int size_second_data_half, int size_data,
           int (*cmp)(byte*, byte*), void (*set)(byte*, byte*)) {

  int i= 0;
  int j= 0;

  int n = size_first_data_half + size_second_data_half;
  byte* ptr_out[n];
  byte out[n * size_data];
  for (int k = 0; k < n; k++)
    ptr_out[k] = (byte*) (out + size_data * k);

  for (int k = 0; k < n; k++) {
    if (i >= size_first_data_half) {
      (*set)(ptr_out[k], second_data_half[j++]);
      continue;
    }
    if (j >= size_second_data_half) {
      (*set)(ptr_out[k], first_data_half[i++]);
      continue;
    }
    if ((*cmp)(first_data_half[i], second_data_half[j]) <= 0) {
      (*set)(ptr_out[k], first_data_half[i++]);
    } else {
      (*set)(ptr_out[k], second_data_half[j++]);
    }
  }

  // copy back
  for (int k = 0; k < n; k++)
      (*set)(first_data_half[k], ptr_out[k]);
}

void merge_sort(byte** data, int size, int size_data,
        int (*cmp)(byte*, byte*), void (*set)(byte*, byte*),
        void (*swap)(byte*, byte*)) {
  int i;

  if (size < 10) {
    bubble_sort(data, size, cmp, swap);
    return;
  }

  int mid = size / 2;
  merge_sort(&data[0], mid, size_data, cmp, set, swap);
  merge_sort(&data[mid], size - mid, size_data, cmp, set, swap);
  merge(data, mid, &data[mid], size - mid, size_data, cmp, set);
}

