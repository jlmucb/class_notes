
#ifdef DEBUG
#include <stdio.h>

int np={0};


int f(int n, int* a) {
  int i;

  printf("%d:",++np);
  for(i=0;i<n;i++)
    printf(" %d",a[i]);
  printf("\n");
}


#endif



/*
 *  generate permutations
 */

#define N 20


rperm( int n, int m, int a[], int  (*f)()) {
  int i,j,k;
  int b[N];

  if(m>=n) {
    (*f)(n,a);
    return(1);
    }
  for(i=0;i<=(n-m);i++)
    b[i]= a[m-1+i];
  for(i=0;i<=(n-m);i++) {
    a[m-1]= b[i];
    k= 0;
    for(j=0;j<=(n-m);j++) {
      if(i!=j) {
        a[m+k]= b[j];
        k++;
        }
      }
    rperm(n,m+1,a,f);
    }
  return(1);
}


#ifdef DEBUG

main(int argn, char** argv) {
  int i,j,n;
  int aa[N];

  n= atoi(argv[1]);
  for(i=0;i<n;i++)
    aa[i]= i+1;
  rperm(n,1,aa,f);
  exit();
}

#endif
