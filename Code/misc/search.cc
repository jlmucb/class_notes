#include "sort.h"
#include <stdio.h>

int linear_search(byte* target_data, byte** data, int size, int (*cmp)(byte*, byte*)) {
  for (int i = 0; i < size; i++) {
    if ((*cmp)(target_data, data[i]) == 0)
      return i;
  }
  return -1;
}

int binary_search(byte* target_data, byte** sorted_data, int start, int end,
     int (*cmp)(byte*, byte*)) {
  
  if ((*cmp)(target_data, sorted_data[start]) == 0)
    return start;
  if ((*cmp)(target_data, sorted_data[end]) == 0)
    return end;

  if (start == end)
    return -1;

  int mid = (end - start) / 2;
  int n = 0;
  int new_end_first_half = start + mid;
  int new_start_second_half = new_end_first_half + 1;
  n = binary_search(target_data, sorted_data, new_start_second_half, end, cmp);
  if (n >= 0)
    return n;
  return binary_search(target_data, sorted_data, start, new_end_first_half, cmp);
}
