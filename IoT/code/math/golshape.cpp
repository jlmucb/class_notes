#include "stdio.h"


// ------------------------------------------------------------------------------

char g_rgiGolay[24*12]= {
//  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   // row  1
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1,   // row  2
  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1,   // row  3
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0,   // row  4
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1,   // row  5
  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0,   // row  6
  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0,   // row  7
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0,   // row  8
  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1,   // row  9
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1,   // row 10
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1,   // row 11
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0     // row 12
};


char g_rgiRowSpace[4096*24];


int g_iShape[25]= {
  0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0
};


// ------------------------------------------------------------------------------


void CalcProd(char* pInd, char* pOut)
{
  int i, j;

  for(i=0; i<24;i++) {
    pOut[i]= 0;
    for(j=0;j<12;j++) {
      if(pInd[j]==1)
        pOut[i]^= g_rgiGolay[24*j+i];
    }
  }
}


void Vec(int i, char* ps)
{
  int j;

  for(j=0;j<12;j++)
    ps[j]= (i>>j)&1;
}


int main(int an, char** av) 
{
  int    i,j,k;
  char  sProd[12];
  char*  ps;

  for(i=0;i<4096;i++) {
    Vec(i,sProd);
    ps= &g_rgiRowSpace[i*24];
    CalcProd(sProd, ps);
  }

  // Calculate Shapes
  for(i=0;i<4096;i++) {
    ps= &g_rgiRowSpace[i*24];
    k= 0;
    for(j=0;j<24;j++)
      k+= ps[j];
    g_iShape[k]++;
  }

  printf("Shapes:\n");
  for(i=0; i<25; i++) {
    if(g_iShape[i]>0)
      printf("\t(%02d[0], %02d[1]) - %d\n", 24-i,i,g_iShape[i]);
  }
}


// ------------------------------------------------------------------------------

