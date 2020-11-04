#include "stdio.h"
#include "sort.h"

void bubble_sort(int* data, int size) {
  int i, j, m;

  for(i = 0; i < size; i++) {
    m = i;
    for(j = i; j < size; j++) {
      if (data[j] < data[m])
        m = j;
    }
    if (m != i) {
      int t = data[m];
      data[m] = data[i];
      data[i] = t;
    }
  }
}
