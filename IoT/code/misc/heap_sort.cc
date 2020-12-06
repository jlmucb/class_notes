#include "sort.h"
#include <stdio.h>

void heapify(byte** data, int size, int (*cmp)(byte*, byte*),
        void (*swap)(byte*, byte*)) {
  int i;

  if (size <= 1)
    return;
  if(((*cmp)(data[0], data[1]) >= 0) && 
     ((*cmp)(data[0], data[2]) > 0))
    return;

  i = 1;
  if((size > 2) && ((*cmp)(data[2], data[1]) > 0))
    i= 2;
  (*swap)(data[0], data[i]);
  heapify(&data[i], size - i, cmp, swap);
  return;
}


void heap_sort(byte** data, int size, int (*cmp)(byte*, byte*),
        void (*swap)(byte*, byte*)) {
  int i;

  // buildheap
  for(i = 1; i <= size; i++)
    heapify(&data[size - i], i, cmp, swap);

  for(i = 1; i <= size; i++) {
    (*swap)(data[size - i], data[0]);
    heapify(data, size - i, cmp, swap);
    }
  return;
}

