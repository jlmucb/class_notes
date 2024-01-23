#define DEBUG

#ifdef DEBUG
#include "stdio.h"
#endif


// ----------------------------------------------------------------------------------



heapify(int a[], int n) {
  int i,j;

  if(n<=1)
    return;
  if((a[0]>=a[1])&&(a[0]>a[2]))
    return;
  i= 1;
  if((n>2)&&(a[2]>a[1]))
    i= 2;
  j= a[0]; a[0]= a[i]; a[i]= j;
  heapify(&a[i],n-i);
  return;
}


heapsort(int a[], int n) {
  int   i,j;

  /* buildheap */
  for(i=1;i<=n;i++)
    heapify(&a[n-i],i);

  for(i=1;i<=n;i++) {
    j= a[n-i];
    a[n-i]= a[0];
    a[0]= j;
    heapify(a, n-i);
    }
  return;
}


#ifdef DEBUG

void printa(char* szTitle, int a[], int n) {
  int i;

  printf("\n%s\n", szTitle);
  for(i=0;i<n;i++) {
    if((i%10)==0)
      printf("\n\t");
    printf("%3d ", a[i]);
  }

  printf("\n");
  }

#define ASIZE 14
int a[ASIZE]= {
  14,11,10,8,6,4,2,
  1,3,5,7,9,11,13
}
;

int main(int an, char* av) {
  printa("Before sort", a, ASIZE);
  heapsort(a, ASIZE);
  printa("After sort", a, ASIZE);
  return(1);
}
#endif


// ----------------------------------------------------------------------------------


