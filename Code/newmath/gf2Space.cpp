#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <malloc.h>


//
//      gf2Space.cpp
//      (c) copyright, 2008, John L. Manferdelli


#ifndef byte
typedef unsigned char byte;
#endif


extern int      strcmp(const char*, const char*);


// --------------------------------------------------------------------- 


long long unsigned reverseCw(int iBitSize, long long unsigned uCw)
{
    long long unsigned tCw= 0L;

    for(int i=0; i<iBitSize; i++) {
        tCw<<= 1;
        tCw|= uCw&1;
        uCw>>= 1;
    }
    return tCw;
}


void printCwBits(long long uCw, int nBits)
{
    int     j;

    for(j=0; j<nBits; j++) {
        if((uCw&1)!=0)
            printf("1");
        else
            printf("0");
        uCw>>= 1;
    }
}


int readCw(char* szStr, long long unsigned* puCw)
{
    int n= 0;
    long long unsigned uCw= 0;

    for(;;) {
        if(*szStr=='\0') {
            *puCw= reverseCw(n, uCw);
            return n;
        }
        if(*szStr=='0') {
            uCw<<= 1;
            uCw|= 0;
            n++;
        }
        else if(*szStr=='1') {
            uCw<<= 1;
            uCw|= 1;
            n++;
        }
        else {
            return -1;
        }
        szStr++;
    }
    return -1;
}


bool fromCw(int iNumBits, long long unsigned uCw, byte* rgbBits)
{
    for(int i=0; i<iNumBits; i++) {
        rgbBits[i]= uCw&1;
        uCw>>= 1;
    }
    return true;
}


bool toCw(int iNumBits, byte* rgbBits, long long unsigned* puCw)
{
    *puCw= 0;
    for(int i=0; i<iNumBits; i++) {
        (*puCw)<<= 1;
        *puCw|= rgbBits[i];
    }
    return true;
}


long long addBasis(int iBitSize, unsigned n, long long unsigned* rgU)
{
    long long unsigned uCw= 0L;

    if((n>>iBitSize)!=0)
        return 0L;
    for(int i=0; i<iBitSize; i++) {
        if(n&1)
            uCw^= rgU[i];
        n>>= 1;
    }
    return uCw;
}


int getWeight(int iBitSize, long long unsigned uCw)
{
    int iWt= 0;

    for(int i=0; i<iBitSize; i++) {
        if((uCw&1)!=0)
            iWt++;
        uCw>>= 1;
    }
    return iWt;
}


// -------------------------------------------------------------------------


// #define DEBUG


//
//      Generate all the codewords
//      


int main(int an, char** av) 

{
    int                     i, j;
    int                     iGenSize= an-1;
    int                     iBitSize;
    int                     iPowSize;
    long long unsigned*     rgCw= NULL;
    long long unsigned*     rgGw= NULL;
    int*                    rgiWt= NULL;
    long long unsigned      rguTmp= 0;


    if(an<2) {
        printf("gf2Space 0101001 ... (%d)\n", an);
        return 1;
    }

    iBitSize= readCw(av[1], &rguTmp);
    iPowSize= 1<<iGenSize;
    if((rgGw=(long long unsigned*) malloc(sizeof(long long unsigned)*iGenSize))==NULL) {
        printf("Cant allocate generator matrix\n");
        return 1;
    }
    if((rgCw=(long long unsigned*) malloc(sizeof(long long unsigned)*iPowSize))==NULL) {
        printf("Cant allocate codeword matrix\n");
        return 1;
    }
    if((rgiWt=(int*) malloc(sizeof(int)*iPowSize))==NULL) {
        printf("Cant allocate weight matrix\n");
        return 1;
    }

    printf("\n");
    printf("Codeword size: %d bits, %d generators, subspace size: %d\n", 
            iBitSize, iGenSize, iPowSize);
    printf("\n");

    for(i=0; i<iGenSize; i++) {
        if(readCw(av[i+1], &rgGw[i])<0) {
            printf("Bad codeword: %s\n", av[i+1]);
            return 1;
        }
    }

    printf("Generators:\n");
    for(i=0; i<iGenSize; i++) {
        printf("\t");
        printCwBits(rgGw[i], iBitSize);
        printf("\n");
    }
    printf("\n");

    // Generate codewords
    for(i=0; i<iPowSize; i++) {
        rgCw[i]= addBasis(iBitSize, (unsigned)i, rgGw);
        rgiWt[i]= getWeight(iBitSize, rgCw[i]);
    }

    printf("Codewords:\n");
    // Generate codewords
    for(i=0; i<iPowSize; i++) {
        printf("\t");
        printCwBits(rgCw[i], iBitSize);
        printf(", weight: %d\n", rgiWt[i]);
    }

    return(0);
}


// ---------------------------------------------------------------------------


