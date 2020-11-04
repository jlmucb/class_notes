#include <stdio.h> 
#include <string.h>
// #include <io.h>
// #include <fcntl.h>
// #include <sys/types.h>
// #include <sys/stat.h>
#include <stdlib.h> 

#ifndef byte
typedef unsigned char byte;
#endif


unsigned g_rguCW[4096];
int g_rgiCounts[25];
int g_iBitCount[256];


// ---------------------------------------------------------------------------------------


int toPositions(unsigned uA, byte* rgbOut, int size)
{
    int i, n;

    n= 0;
    for(i=0; i<size; i++) {
        if(uA&1)
            rgbOut[n++]= i;
        uA>>= 1;
    }
    return(n);
}


int countBits(byte b)
{
    int i, n;

    n= 0;
    for(i=0;i<8; i++) {
        if(b&1)
            n++;
        b>>= 1;
    }
    return(n);
}


void initBitCount(int* rgiArray)
{
        int i;
        byte b;

        for(i=0; i<256; i++)
            rgiArray[i]= countBits((byte) i);

#ifdef DEBUG1
        printf("debug bitcount\n");
        for(i=0; i<256; i++) {
            printf("%d ", rgiArray[i]);
            if((i%16)==15)
                    printf("\n");
        }
#endif
}

int perfCount(unsigned w)
{
        byte* pb= (byte*) &w;
        int n= g_iBitCount[(int)(*pb++)];
        n+= g_iBitCount[(int)(*pb++)];
        n+= g_iBitCount[(int)(*pb++)];
        n+= g_iBitCount[(int)(*pb)];
        return(n);
}


unsigned toWord(byte* rgB, int size)
{
        unsigned u= 0;
        int i;

        for(i=(size-1); i>=0; i--) {
                u <<= 1;
                u |= rgB[i]&1;
        }
        return(u);
}


void toBytes(byte* rgB, int size, unsigned uIn)
{
        int i;

        for(i=0; i<size; i++) {
            rgB[i]= uIn&1;
            uIn>>= 1;
        }
        return;
}


void MultOnLeft(byte* rguGen, int iRows, int iCols, byte* rguIn, byte* rguResult)
{
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

byte g_rgbG[288]= {
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

void printBits(unsigned a) {

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
int main(int an, char** av) 
{ 
        unsigned uIndex, uCW;
        byte rguLCW[24];
        byte rguVec[12];
        byte* pb;
        int iWeight= 8;

        initBitCount(g_iBitCount);

        for(uIndex=0; uIndex<4096; uIndex++) {
            uCW= 0;
            toBytes(rguVec, 12, uIndex);
            MultOnLeft(g_rgbG, 12, 24, rguVec, rguLCW);
            g_rguCW[uIndex]= toWord(rguLCW, 24);
            printBits(g_rguCW[uIndex]);
            printf("\n");
        }

        int i, j, k, n;
        for(i=0;i<=24;i++)
            g_rgiCounts[i]= 0;
        for(i=0;i<4096;i++) {
            n= perfCount(g_rguCW[i]);
            if(n<=24)
                g_rgiCounts[n]++;
        }

        printf("Golay Survey\n");
        for(i=0; i<=24; i++) {
            if(g_rgiCounts[i] !=0)
                printf("    %4d codewords of weight %2d\n", g_rgiCounts[i], i);
        }
        printf("\n");

        for(i=1; i<=iWeight; i++) {
            if(g_rgiCounts[i]>0) {
                printf("Codewords of weight %d:\n",i);
                for(j=0; j<4096; j++) {
                    uCW= g_rguCW[j];
                    if(perfCount(uCW)!=i)
                        continue;
                    k= toPositions(uCW, rguLCW, 24);
                    if(k<=0)
                        continue;
                    printf("\t");
                    for(int m=0; m<k; m++) {
                        printf("%2d ", (int) rguLCW[m]);
                    }
                    printf("\n");
                }
            }
        }

        printf("\nDone\n");

        return(0);
}


// ---------------------------------------------------------------------------------------




