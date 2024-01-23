#include "stdio.h"

//
//  Generate 12 dimensional golay code G(24) and list the octads
//
//  (c) 2009, John L. Manferdelli


#ifndef byte
typedef unsigned char byte;
#endif
typedef unsigned codeword;


// ------------------------------------------------------------------------------

//
//  24 is infinity position
//

//#define GENGOLAYGEN
#define CODE1
#ifdef GENGOLAYGEN

byte g_rgbGolay[24*12]g

inline int mindex(int r, int c, int rd, int cd) {
  return r*cd+c;
}


void genGolayGenMatrix() {
  int i, j, k;
  int mod11squares[11]={1,1,0,1,1,1,0,0,0,1,0};

  for(i=0; i<12; i++) {
    for(j=0; j<24; j++) {
      g_rgbGolay[mindex(i,j,12,24)]= 0;
    }
  }

  for(i=0; i<12; i++)
    g_rgbGolay[mindex(i,i,12,24)]= 1;

  for(i=0; i<11; i++)
    g_rgbGolay[mindex(i,12,12,24)]= 1;
  g_rgbGolay[mindex(11,12,12,24)]= 0;

  for(i=0; i<11; i++) {
    for(j=0; j<11; j++) {
      g_rgbGolay[mindex(i,13+j,12,24)]= mod11squares[(11+j-i)%11];
    }
  }
  for(j=0; j<11; j++)
    g_rgbGolay[mindex(11,13+j,12,24)]= 1;
}
#endif


#define BIGGOLAY
#ifndef BIGGOLAY

byte g_rgbGolay[24*12]= {
//    1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     // row  1
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1,     // row  2
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1,     // row  3
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0,     // row  4
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1,     // row  5
    0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0,     // row  6
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0,     // row  7
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0,     // row  8
    0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1,     // row  9
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1,     // row 10
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1,     // row 11
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0      // row 12
};

#else

byte g_rgbGolay[24*24]= {
//  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24
  0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1,
  0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1,
  0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1,
  0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1,
  1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1,
  0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1,
  1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1,
  0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1,
  0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1,
  1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1,
  0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1,
  0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1,
  1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1,
  1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1,
  0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1,
  1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1,
  0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1,
  1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1,
  1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1,
  1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1,
  // 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

#endif


codeword  g_rgcwGenerators[12];
codeword  g_rgCodeWord[4096];

int     g_iNumOctads= 0;
codeword  g_rgOctads[4096];


codeword rev24cw(codeword cw) {
  int     i;
  codeword  t= 0;

  for(i=0;i<24;i++) {
    t= (t<<1)|(cw&1);
    cw>>= 1;
  }

  return t;
}


bool compressGenerators() {
  int     i, j;
  codeword  cw;
  byte*     pM;

  for(i=0; i<12; i++) {
    cw= 0;
    pM= &g_rgbGolay[24*i];
    for(j=0; j<24;j++) {
      cw= (cw<<1)|(*pM);
      pM++;
    }
    g_rgcwGenerators[i]= cw;
  }
  return true;
}


void GenerateCodewords() {
  unsigned  u, v;
  codeword  cw;
  int     j;

  for(u=0; u<4096; u++) {
    cw= 0;
    v= u;
    for(j=0;j<12;j++) {
      if(v&1) {
        cw^=  g_rgcwGenerators[j];
      }
      v>>= 1;
    }
    g_rgCodeWord[u]= cw;
  }
}


int weight(codeword cw) {
  int   i;
  int   w= 0;

  for(i=0; i<24; i++) {
    if(cw&1)
      w++;
    cw>>= 1;
  }

  return w;
}


void printCw(codeword cw) {
  int     i;
  codeword  t= rev24cw(cw);
  
  for(i=0; i<24; i++) {
    printf("%1d ", t&1);
    t>>= 1;
  }
}


void printOctad(codeword cw) {
  int i, j;

  cw= rev24cw(cw);
  for(i=0;i<24;i++) {
    if(cw&1) {
      if(i==23)
        printf("inf ");
      else
        printf("%3d ", i);
    }
    cw>>= 1;
  }
}


void sortOctads() {
  int     i, j;
  codeword  cw;

  for(i=0;i<g_iNumOctads; i++) {
    for(j=i+1;j<g_iNumOctads; j++) {
      if(g_rgOctads[i]<g_rgOctads[j]) {
        cw= g_rgOctads[i];
        g_rgOctads[i]= g_rgOctads[j];
        g_rgOctads[j]= cw;
      }
    }
  }

  // for(i=0;i<g_iNumOctads; i++)
  //   g_rgOctads[i]= rev24cw(g_rgOctads[i]);

  return;
}



void fetchOctads() {
  int     i;
  codeword  cw;

  for(i=0;i<4096; i++) {
    cw= g_rgCodeWord[i]; 
    if(weight(cw)==8) {
      g_rgOctads[g_iNumOctads++]= cw;
    }
  }
}


void listOctads() {
  int     i;
  codeword  cw;

  for(i=0;i<g_iNumOctads; i++) {
    cw= g_rgOctads[i];
    printf("\t");
    printOctad(cw);
    printf("\n");
    }
}


// --------------------------------------------------------------------------


int main(int an, char** av) {
  int   i,j,k;
  int   iWt0cw= 0;
  int   iWt8cw= 0;
  int   iWt12cw= 0;
  int   iWt16cw= 0;
  int   iWt24cw= 0;

#ifdef GENGOLAYGEN
  genGolayGenMatrix();
#endif

  if(!compressGenerators()) {
    printf("Cant compress generators\n");
    return 1;
  }

  GenerateCodewords();

  printf("Generators:\n");
  for(i=0;i<12; i++) {
    printf("\t"); 
    printCw(g_rgcwGenerators[i]); 
    printf(" --- %d\n", weight(g_rgcwGenerators[i]));
  }
  printf("End Generators:\n\n");

  for(i=0; i<4096; i++) {
    switch(weight((codeword)g_rgCodeWord[i])) {
      default:
      printf("Wierd weight: %08x\n", g_rgCodeWord[i]);
      break;
      case 0:
      iWt0cw++;
      break;
      case 8:
      iWt8cw++;
      break;
      case 12:
      iWt12cw++;
      break;
      case 16:
      iWt16cw++;
      break;
      case 24:
      iWt24cw++;
      break;
      }
  }
  printf("%d 0-ads, %d 8-ads, %d 12-ads, %d 16-ads, %d 24-ads\n",
       iWt0cw, iWt8cw, iWt12cw, iWt16cw, iWt24cw);

  fetchOctads();
  sortOctads();

  printf("\nOctads:\n");
  listOctads();
  printf("End Octads\n");
  printf("\nDone\n");
  return 0;
}


// ---------------------------------------------------------------------------


