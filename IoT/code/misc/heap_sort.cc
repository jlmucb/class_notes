#include "sort.h"
#include <stdlib.h>

void heapify(int* data, int size) {
  int i, j;

  if (size <= 1)
    return;
  if((data[0] >= data[1]) && (data[0]>data[2]))
    return;

  i = 1;
  if((size > 2) && (data[2] > data[1]))
    i= 2;
  j= data[0];
  data[0]= data[i];
  data[i]= j;
  heapify(&data[i], size - i);
  return;
}


void heap_sort(int* data, int size) {
  int i,j;

  // buildheap
  for(i = 1;i <= size; i++)
    heapify(&data[size - i], i);

  for(i = 1; i <= size; i++) {
    j= data[size - i];
    data[size - i]= data[0];
    data[0]= j;
    heapify(data, size - i);
    }
  return;
}



