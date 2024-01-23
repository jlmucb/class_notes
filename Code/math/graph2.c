/*
 *  Shortest path: Dykstra and Floyd
 */

//   #define DEBUG


#ifndef INFCOST
#define INFCOST  8000
#endif

int dypath(int n,int i,int cm[],int tm[],int (* cost)(int,int)) {

   int j,k,m,l,lc;

   for(j=0;j<n;j++) {
     cm[j]= (*cost)(i,j);
     tm[j]= -1;
     }
   tm[i]= 0;

   for(l=0;l<n;l++) {
     k= -1;
     lc= INFCOST;
     for(j=0;j<=n;j++) {
       if((tm[j]<0)&&(cm[j]<lc)) {
         lc= cm[j];
         k= j;
         }
       }
     if(k<0)
       break;
     lc= INFCOST;
     for(j=0;j<=n;j++) {
       m= cm[k]+(*cost)(k,j);
       if((j!=k)&&(tm[j]<0)&&(cm[j]>(m=(cm[k]+(*cost)(k,j)))))
             cm[j]= m;
         if(cm[j]<lc)
           lc= cm[j];
         }
     tm[k]= lc;
     }
    return(1);
}


floyd(int n, int am[], int (*cost)(int,int), int (*indx)(int,int)) {

   int i,j,k,m;


    for(i=0;i<=n;i++)
     for(j=0;j<n;j++)
       am[(*indx)(i,j)]= (*cost)(i,j);


  for(k=0;k<=n;k++)
    for(i=0;i<=n;i++)
      for(j=0;j<=n;j++)
        if((m=(am[(*indx)(i,k)]+am[(*indx)(k,j)]))<am[(*indx)(i,j)])
          am[(*indx)(i,j)]= m;
  return(1);
}

#ifdef DEBUG
#include <stdio.h>


int cst1(int i, int j) {
  int i1,j1,i2,j2;

  i1= i&03;
  j1= (i>>2)&03;
  i2= j&03;
  j2= (j>>2)&03;
  if((i1==i2)&&(j1==j2))
    return(0);
  if(i1==i2) {
    if((j1==(j2+1))||((j1+1)==j2))
      return(1);
    }
  if(j1==j2) {
    if((i1==(i2+1))||((i1+1)==i2))
      return(1);
    }
  return(INFCOST);
}


int ind1(int i,int j) {
  return(4*i+j);
}


int tm1[16], tm2[16];


main(int argn, char** argv) {
    int i,j;

  dypath(16,0,tm1,tm2,cst1);
  for(i=0;i<16;i++)
    printf("%3d ",i);
  printf("\n");
  for(i=0;i<16;i++)
    printf("%3d ",tm1[i]);
  printf("\n");
  return(1);
}

#endif

