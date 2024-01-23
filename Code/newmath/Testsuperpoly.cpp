#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <string.h>
//#include <io.h>
#include <unistd.h>


// Testsuperpoly.cpp
//      Generate P(x1,x2,x3,x4,x5)= x1x2(x3+x4+1) + x2x4x5+x3x5+x2+x3+x5+1
//      John Manferdelli


// -----------------------------------------------------------------


short unsigned P[32];


inline short unsigned evalP(short unsigned x1, short unsigned x2, short unsigned x3, 
                            short unsigned x4, short unsigned x5)
{
    short unsigned  r= 0;

    // printf("evalP(%d, %d, %d, %d, %d)= ", x5,x4,x3,x2,x1);
    r^= (x1&x2)&(x3^x4^1);
    r^= x2&x4&x5;
    r^= x3&x5;
    r^= x2^x3^x5^1;
    // printf("%d\n", r);

    return r;
}


int main(int an, char** av) 

{
    short unsigned u;
    short unsigned x1, x2, x3, x4, x5;
    char* szOut= av[1];

    for(u=0;u<32;u++) {
        x1= u&0x1;
        x2= (u>>1)&0x1;
        x3= (u>>2)&0x1;
        x4= (u>>3)&0x1;
        x5= (u>>4)&0x1;
        P[(int)u]= evalP(x1, x2, x3, x4, x5);
    }

    // Read function
    int iOut= open(szOut, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(iOut<0) {
        printf("Cant open: %s\n", szOut);
        return 1;
        }
    write(iOut, P, sizeof(short unsigned)*32);
    close(iOut);

    printf("\n\nDone\n");
    return 0;
}


// ----------------------------------------------------------------------


