#ifndef __SORT_H__
#define __SORT_H__

#include <stdio.h>

#ifndef byte
typedef unsigned char byte;
#endif

void bubble_sort(byte** data, int size,
        int (*cmp)(byte*, byte*), void (*swap)(byte*, byte*));
void merge_sort(byte** data, int size, int size_data,
        int (*cmp)(byte*, byte*), void (*set)(byte*, byte*),
        void (*swap)(byte*, byte*));
void heap_sort(byte** data, int size, int (*cmp)(byte*, byte*),
        void (*swap)(byte*, byte*));
int select (byte** a, int n, int k, int (*cmp)(byte*, byte*),
        void (*swap)(byte*, byte*));
int linear_search(byte* target_data, byte** data, int size,
        int (*cmp)(byte*, byte*));
int binary_search(byte* target_data, byte** sorted_data, int start, int end, 
     int (*cmp)(byte*, byte*));


inline int int_cmp(byte* a, byte* b) {
  if (*((int*)a) > *((int*)b))
    return 1;
  if (*((int*)a) == *((int*)b))
    return 0;
  return -1;
}

inline void int_swap(byte* a, byte* b) {
  int t = (int)*((int*)a);
  *((int*)a) = *((int*)b);
  *((int*)b) = t;
}

inline void int_set(byte* a, byte* b) {
  *((int*)a) = *((int*)b);
}

inline void int_print(byte* a) {
  printf("%d", *(int*)a);
}
#endif
