#include "stdio.h"

typedef long long unsigned uint64_t;
#define NUMCUBES 40


int main(int an, char** av) 
{
  uint64_t  a, b;
  int       i, j, k,l;
  uint64_t  cubes[NUMCUBES];

  for(i=1;i<NUMCUBES;i++)
    cubes[i]= i*i*i;

  for(i=1;i<NUMCUBES; i++) {
    for(j=i+1;j<NUMCUBES; j++) {
      a= cubes[i]+cubes[j];
      for(k=i+1;k<NUMCUBES; k++) {
        for(l=i+2;l<NUMCUBES; l++) {
          b= cubes[k]+cubes[l];
          if(a==b)
            printf("i: %d, j: %d, k: %d, l: %d\n", i,j,k,l);
        }
      }
    }
  }

  return 0;
}


