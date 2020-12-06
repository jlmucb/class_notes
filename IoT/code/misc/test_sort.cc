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

  byte* item_pointer[200];
  for (int i = 0; i < 200; i++)
    item_pointer[i] = (byte*)(&data[i]);

  gen_array(data, 200);
  printf("\nOriginal data\n");
  for(int i = 0; i < 200; i++)
    printf("%d  %d\n", i, data[i]);

  printf("\nBubble sorted data\n");
  bubble_sort(item_pointer, 200, &int_cmp, &int_swap);
  for(int i = 0; i < 200; i++) {
    printf("%d  ", i);
    int_print(item_pointer[i]);
    printf("\n");
  }

  int to_find;
  int n;

  printf("\nlinear search\n");
  to_find = 9;
  n = linear_search((byte*) &to_find, item_pointer, 200, &int_cmp);
  if (n < 0) {
    printf("%d not in array\n", to_find);
  } else {
    printf("%d found at %d, ", to_find, n);
    int_print(item_pointer[n]);
    printf("\n");
  }

  printf("\nbinary search\n");
  to_find = 9;
  n = binary_search((byte*) &to_find, item_pointer, 0, 199, &int_cmp);
  if (n < 0) {
    printf("%d not in array\n", to_find);
  } else {
    printf("%d found at %d, ", to_find, n);
    int_print(item_pointer[n]);
    printf("\n");
  }

  printf("\n");
  gen_array(data, 50);
  printf("\nOriginal data\n");
  for(int i = 0; i < 50; i++)
    printf("%d  %d\n", i, data[i]);
  printf("\nHeap sorted data\n");
  heap_sort(item_pointer, 50, &int_cmp, &int_swap);
  for(int i = 0; i < 50; i++) {
    printf("%d  ", i);
    int_print(item_pointer[i]);
    printf("\n");
  }

  byte* out_item_pointer[200];
  for (int i = 0; i < 200; i++)
    out_item_pointer[i] = (byte*)(&out[i]);

  printf("\n");
  gen_array(data, 50);
  printf("\nOriginal data\n");
  for(int i = 0; i < 50; i++)
    printf("%d  %d\n", i, data[i]);
  printf("\nMerge sorted data\n");
  merge_sort(item_pointer, 50, sizeof(int), &int_cmp, &int_set, int_swap);
  for(int i = 0; i < 50; i++) {
    printf("%d  ", i);
    int_print(item_pointer[i]);
    printf("\n");
  }

#if 0
  gen_array(data, 200);
  printf("select data\n");
  int s = select(data, 200, 100);
  printf("100th element is %d\n", s);
#endif

  return 0;
}
