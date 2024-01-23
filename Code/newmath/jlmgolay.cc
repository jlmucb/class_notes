#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 

#ifndef byte
typedef unsigned char byte;
#endif


unsigned cw[4096];
int counts[25];
int bit_counts[256];


// ---------------------------------------------------------------------------------------


int to_positions(unsigned uA, byte* out, int size) {
  int i, n;

  n= 0;
  for(i=0; i<size; i++) {
    if(uA&1)
      out[n++]= i;
    uA>>= 1;
  }
  return(n);
}


int countBits(byte b) {
  int i, n;

  n= 0;
  for(i=0;i<8; i++) {
    if(b&1)
      n++;
    b>>= 1;
  }
  return(n);
}


void init_bit_count(int* in) {
  int i;
  byte b;

  for(i=0; i<256; i++)
    in[i]= countBits((byte) i);

#ifdef DEBUG1
  printf("debug bitcount\n");
  for(i=0; i<256; i++) {
    printf("%d ", in[i]);
    if((i%16)==15)
      printf("\n");
  }
#endif
}

int perf_count(unsigned w) {
  byte* pb= (byte*) &w;
  int n= bit_counts[(int)(*pb++)];
  n+= bit_counts[(int)(*pb++)];
  n+= bit_counts[(int)(*pb++)];
  n+= bit_counts[(int)(*pb)];
  return(n);
}


unsigned to_word(byte* in, int size) {
  unsigned u= 0;
  int i;

  for(i=(size-1); i>=0; i--) {
    u <<= 1;
    u |= in[i]&1;
  }
  return(u);
}


void to_bytes(byte* out, int size, unsigned in) {
  int i;

  for(i=0; i<size; i++) {
      out[i]= in&1;
      in>>= 1;
  }
  return;
}


void mult(byte* rguGen, int iRows, int iCols, byte* rguIn, byte* rguResult) {
  int i, j, n;
  byte uA;

  for(i=0; i<iCols; i++) {
      uA= 0;
      for(j=0; j<iRows; j++) {
    n= iCols*j + i;
    uA^= rguGen[n]&rguIn[j];
      }
      rguResult[i]= uA;
  }
}


// Generator  I(12)||  Circ(11)
// QR's 0, 1, 3, 4, 5, 9

byte G[288]= {
//      0  1  2  3  4  5  6  7  8  9 10 11  0  1  2  3  4  5  6  7  8  9 10 11  
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, //  0
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, //  1
  0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, //  2
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, //  3
  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, //  4
  0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, //  5
  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, //  6
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, //  7
  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, //  8
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, //  9
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, // 10
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0  // 11
  };

void print_bits(unsigned a) {

  unsigned t = 1;
  for (int j = 0; j < 24; j++) {
    if (t&a)
      printf("1");
    else
      printf("0");
    t <<= 1;
  }
}

// Golay Code
int main(int an, char** av) { 
  unsigned u_cw;
  byte b_cw[24];
  byte vec[12];
  byte* pb;
  int iWeight= 8;

  init_bit_count(bit_counts);

  for(unsigned u=0; u<4096; u++) {
      u_cw= 0;
      to_bytes(vec, 12, u);
      mult(G, 12, 24, vec, b_cw);
      cw[u]= to_word(b_cw, 24);
      print_bits(cw[u]);
	    printf(" ---- %2d", perf_count(cw[u]));
      printf("\n");
  }

  int i, j, k, n;
  for(i=0;i<=24;i++)
      counts[i]= 0;
  for(i=0;i<4096;i++) {
    n= perf_count(cw[i]);
    if(n<=24)
      counts[n]++;
  }

  printf("Golay Survey\n");
  for(i=0; i<=24; i++) {
      if(counts[i] !=0)
        printf("    %4d codewords of weight %2d\n", counts[i], i);
  }
  printf("\n");

  for(i=1; i<=iWeight; i++) {
      if(counts[i]>0) {
    printf("Codewords of weight %d:\n",i);
    for(j=0; j<4096; j++) {
        u_cw= cw[j];
        if(perf_count(u_cw)!=i)
          continue;
        k= to_positions(u_cw, b_cw, 24);
        if(k<=0)
          continue;
        printf("\t");
        for(int m=0; m<k; m++) {
          printf("%2d ", (int) b_cw[m]);
        }
        printf("\n");
    }
      }
  }

  printf("\nDone\n");

  return(0);
}


// ---------------------------------------------------------------------------------------




