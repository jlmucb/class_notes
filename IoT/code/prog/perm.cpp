#include <stdio.h>
#include <string.h>


typedef short int short_t;
typedef char byte_t;
typedef int int_t;


// Front Rotation, clockwise (F)

short_t iiF0[]={ 
      9, 12, 15, 16, 17, 14, 11, 10, 13,
     18, 21, 24, 51, 48, 45, 33, 30, 27, 36, 39, 42,-1,
      11, 10,  9, 12, 15, 16, 17, 14, 13,
     36, 39, 42, 18, 21, 24, 51, 48, 45, 33, 30, 27};

short_t iiF1[]={
     19, 22, 25, 52, 49, 46, 34, 31, 28, 37, 40, 43,-1,
     37, 40, 43, 19, 22, 25, 52, 49, 46, 34, 31, 28};

short_t iiF2[]= {   
      0,  1,  2,  3,  4,  5,  6,  7,  8,
     20, 23, 26, 53, 50, 47, 35, 32, 29, 38, 41, 44,-1,
      2,  5,  8,  1,  4,  7,  0,  3,  6,
     38, 41, 44, 20, 23, 26, 53, 50, 47, 35, 32, 29};



// Right Rotation, clockwise (R)

short_t iiR0[]= {
      42, 43, 44, 39, 40, 41, 36, 37, 38,
       0,  3,  6, 20, 19, 18, 15, 12,  9, 27, 28, 29,-1,
          44, 41, 38, 43, 40, 37, 42, 39, 36,
      27, 28, 29,  0,  3,  6, 20, 19, 18, 15, 12,  9};

short_t iiR1[]= {
    23, 22, 21, 16, 13, 10, 30, 31, 32,  1,  4,  7,-1,
     1,  4,  7, 23, 22, 21, 16, 13, 10, 30, 31, 32};

short_t iiR2[]= {
     53, 52, 51, 50, 49, 48, 47, 46, 45,
     2,  5,  8, 26, 25, 24, 17, 14, 11, 33, 34, 35,-1,
    47, 50, 53, 46, 49, 52, 45, 48, 51,
    33, 34, 35,  2,  5,  8, 26, 25, 24, 17, 14, 11 };


// Top Rotation, Clockwise (T)

short_t iiT0[]= {
    27, 30, 33, 34, 35, 32, 29, 28, 31,
    0,  1,  2, 36, 37, 38,  9, 10, 11, 45, 46, 47,-1,
    29, 28, 27, 30, 33, 34, 35, 32, 31,
    47, 46, 45,  0,  1,  2, 38, 37, 36,  9, 10, 11};

short_t iiT1[]= {
    12, 13, 14, 48, 49, 50,  5,  4,  3, 41, 40, 39,-1,
    41, 40, 39, 12, 13, 14, 48, 49, 50,  5,  4,  3};

short_t iiT2[]= {
    18, 21, 24, 25, 26, 23, 20, 19, 22,
    15, 16, 17, 51, 52, 53,  8,  7,  6, 44, 43, 42,-1,
    20, 19, 18, 21, 24, 25, 26, 23, 22,
    44, 43, 42, 15, 16, 17, 51, 52, 53,  8,  7,  6};



short_t F0[54], F0I[54], F1[54], F1I[54], F2[54], F2I[54];
short_t T0[54], T0I[54], T1[54], T1I[54], T2[54], T2I[54];
short_t R0[54], R0I[54], R1[54], R1I[54], R2[54], R2I[54];

short_t gin[54], gout[54];



 // ----------------------------------------------------------------------



void initp(short_t in[], short_t out[],short_t outi[])

{
  int i,n;

  for(n=0;in[n]>=0;n++);
  for(i=0;i<54;i++) {
    out[i]= i;
    outi[i]= i;
    }
  for(i=0;i<n;i++) {
    out[in[i]]= in[n+1+i];
    outi[in[n+1+i]]= in[i];
    }
}


void pcopy(int n, short_t in[], short_t out[])

{
  int i;

  for(i=0;i<n;i++)
    out[i]= in[i];
}


void applyperm (int n, short_t perm[], short_t in[], short_t out[])

{
  int i;

  for(i=0;i<n;i++)
    out[i]= perm[in[i]];
}



void pprint(short_t pp[])

{
  int i;

  for(i=0;i<27;i++)
    printf(" %2d", i);
  printf("\n");
  for(i=0;i<27;i++)
    printf(" %2d", pp[i]);
  printf("\n\n");

  for(i=27;i<54;i++)
    printf(" %2d", i);
  printf("\n");
  for(i=27;i<54;i++)
    printf(" %2d", pp[i]);
  printf("\n\n");

}


#define MAXARRAY 100


void printcycles(int_t n, short_t map[])

{
  int i,j,k,m;
  byte_t seen[MAXARRAY];

  if(n>=MAXARRAY)  {
    printf("Array too big\n");
    return;
    }

  for(i=0;i<n;i++)
    seen[i]= 0;

  for(i=0;i<n;i++) {
    if(seen[i]!=0)
      continue;
    seen[i]= 1;
    if(map[i]==i) {
      seen[i]= 1;
      continue;
      }
    j= i;
    m= i;
    printf("(%d",i);
    k= 0;
    while(k++<n) {
      m= map[m];
      if(m==j)
        break;
      printf(",%d",m);
      seen[m]= 1;      
      }
    printf(")");
    }
  printf("\n");
  return;
}




 int main(int an,char *av[])

 {
   int i;
  short_t *p;


  initp(iiF0,F0,F0I);
    initp(iiF1,F1,F1I);
    initp(iiF2,F2,F2I);
    initp(iiT0,T0,T0I);
    initp(iiT1,T1,T1I);
    initp(iiT2,T2,T2I);
    initp(iiR0,R0,R0I);
    initp(iiR1,R1,R1I);
    initp(iiR2,R2,R2I);

   //printf("F0:\n");
   //pprint(F0);
  //printf("F0I:\n");
  //pprint(F0I);

  for(i=0;i<54;i++)
    gin[i]= i;


  // applyperm(54,F0,gin,gout);
  // pcopy(54,gout,gin);
  // applyperm(54,F0I,gin,gout);
  // printf("ID?:\n");
  // pprint(gout);
  for(i=1;i<an;i++) {
    printf("%s",av[i]);
    }
  printf("\n");

  for(i=1;i<an;i++) {
    p= NULL;
    if(strcmp(av[i],"F0")==0)
      p= F0;
    if(strcmp(av[i],"F1")==0)
      p= F1;
    if(strcmp(av[i],"F2")==0)
      p= F2;
    if(strcmp(av[i],"-F0")==0)
      p= F0I;
    if(strcmp(av[i],"-F1")==0)
      p= F1I;
    if(strcmp(av[i],"-F2")==0)
      p= F2I;
    if(strcmp(av[i],"T0")==0)
      p= T0;
    if(strcmp(av[i],"T1")==0)
      p= T1;
    if(strcmp(av[i],"T2")==0)
      p= T2;
    if(strcmp(av[i],"-T0")==0)
      p= T0I;
    if(strcmp(av[i],"-T1")==0)
      p= T1I;
    if(strcmp(av[i],"-T2")==0)
      p= T2I;
    if(strcmp(av[i],"R0")==0)
      p= R0;
    if(strcmp(av[i],"R1")==0)
      p= R1;
    if(strcmp(av[i],"R2")==0)
      p= R2;
    if(strcmp(av[i],"-R0")==0)
      p= R0I;
    if(strcmp(av[i],"-R1")==0)
      p= R1I;
    if(strcmp(av[i],"-R2")==0)
      p= R2I;
    if(p!=NULL) {
      applyperm(54,p,gin,gout);
      pcopy(54,gout,gin);
      }
    }
  printf("Result:\n");
  pprint(gout);
  printcycles(54,gout);

  return(1);
 }



 // ----------------------------------------------------------------------



