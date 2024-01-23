#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <malloc.h>


//
//      H64.cpp
//      (c) copyright, 2008, John L. Manferdelli


#ifndef byte
typedef unsigned char byte;
#endif


extern int      strcmp(const char*, const char*);


// --------------------------------------------------------------------- 


int Dot(int i, int j)
{
    int k;
    int iWt= 0;

    for(k=0; k<32;k++) {
        iWt+= i&j&1;
        i>>= 1;
        j>>= 1;
    }
    return iWt;
}


// -------------------------------------------------------------------------


// #define DEBUG


//
//      Generate all the codewords
//      


struct cw {int rgB[32];};


int main(int an, char** av) 

{
    int   i, j;
    int   n= 0;
    int   rgT[32]= {1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,1};
    struct cw rgCw[64];

    for(i=0; i<32; i++) {
        for(j=0;j<32;j++) {
            if(Dot(i, j)%2==1) {
                printf("1");
                rgCw[i].rgB[j]= 1;
            }
            else {
                printf("0");
                rgCw[i].rgB[j]= 0;
            }
        }
        printf("  %02d\n", i);
    }

    for(i=0; i<32; i++) {
        for(j=0;j<32;j++) {
            if(Dot(i, j)%2==1) {
                printf("0");
                rgCw[i+32].rgB[j]= 0;
            }
            else {
                printf("1");
                rgCw[i+32].rgB[j]= 1;
            }
        }
        printf("  %02d\n", i);
    }

    for(i=0; i<64; i++) {
        n= 0;
        for(j=0; j<32; j++) {
            n+= (1-2*rgT[j])*(1-2*rgCw[i].rgB[j]);
        }
        if((i%8)==0)
            printf("\n");
        printf("%02d: %03d, ", i, n);
    }
    printf("\n");

    return(0);
}


// ---------------------------------------------------------------------------


