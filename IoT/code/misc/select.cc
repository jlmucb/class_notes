#include "sort.h"
#include <stdlib.h>
#include "stdio.h"

int select (int* data, int n, int k) {
  int i, n5;

  if (n < 75) {
    bubble_sort(data, n);
    return(data[k-1]);
  }

  n5 = n/5;
  for(i = 0; i < n5; i++)
    bubble_sort(&data[i*5], 5);

  int* sb = (int*)malloc(n5 * sizeof(int));
  for(i = 0; i < n5; i++)
    sb[i] = data[i * 5 + 2];
  int m = select(sb, n5, (n5 + 1) / 2);

  int* lt = (int*)malloc(n * sizeof(int));
  int* gt = (int*)malloc(n * sizeof(int));
  int* eq = (int*)malloc(n * sizeof(int));
  int num_gt = 0;
  int num_lt = 0;
  int num_eq = 0;

  for (i = 0; i < n; i++) {
    if (m > data[i]) {
      lt[num_lt++] = data[i];
    } else if (m == data[i]) {
      eq[num_eq++] = data[i];
    } else {
      gt[num_gt++] = data[i];
    }
  }

  if (num_lt >= k) {
    free(sb);
    free(gt);
    free(eq);
    int s = select(lt, num_lt, k);
    free(lt);
    return s;
    }
  if ((num_lt + num_eq) >= k) {
    free(sb);
    free(gt);
    free(eq);
    free(lt);
    return(m);
  }
  free(sb);
  free(eq);
  free(lt);
  int s = select(gt, num_gt, k - num_lt - num_eq);
  free(gt);
  return s;
}
  
