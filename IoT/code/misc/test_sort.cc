#include "stdio.h"
#include "sort.h"


void gen_array(int* data, int size) {
  int i;

  for(i = 0; i < size; i++)
    data[i] = (i * 37) % 30;
}

int main(int an, char** av) {
  int data[200];
  int out[200];

  gen_array(data, 200);
  printf("\nOriginal data\n");
  for(int i = 0; i < 200; i++)
    printf("%d  %d\n", i, data[i]);

  printf("Bubble sorted data\n");
  bubble_sort(data, 200);
  for(int i = 0; i < 200; i++)
    printf("%d  %d\n", i, data[i]);

  gen_array(data, 50);
  printf("Merge sorted data\n");
  merge_sort(data, 50, out);
  for(int i = 0; i < 50; i++)
    printf("  %d\n", out[i]);

  gen_array(data, 50);
  printf("Heap sorteddata\n");
  heap_sort(data, 50);
  for(int i = 0; i < 50; i++)
    printf("  %d\n", data[i]);

  gen_array(data, 200);
  printf("select data\n");
  int s = select(data, 200, 100);
  printf("100th element is %d\n", s);

  return 0;
}
