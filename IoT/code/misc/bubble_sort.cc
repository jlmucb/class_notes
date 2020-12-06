#include "stdio.h"
#include "sort.h"

void bubble_sort(byte** data, int size,  int (*cmp)(byte* a, byte* b),
        void (*swap)(byte* a, byte* b)) {
  int i, j, m;

  for(i = 0; i < size; i++) {
    m = i;
    for(j = i; j < size; j++) {
      if ((*cmp)(data[j], data[m]) < 0)
        m = j;
    }
    if (m != i) {
      (*swap)(data[m], data[i]);
    }
  }
}
