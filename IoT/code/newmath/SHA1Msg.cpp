#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>

// SA1Msg.cpp


#ifndef byte
typedef unsigned char byte;
#endif


extern int      strcmp(const char*, const char*);


// --------------------------------------------------------------------- 


void printPositions(unsigned z)
{
    int i;

    for(i=0; i<32; i++) {
        if((z&1)!=0)
            printf("%02d ", i);
        z>>= 1;
    }
    printf("\n");
}


void addtotally(int* rgWu, unsigned z)
{
    int i;

    for(i=0; i<32; i++) {
        if((z&1)!=0)
            rgWu[i]++;
        z>>= 1;
    }
}




int main(int an, char** av) 

{
    int             i, j;
    unsigned        u[80];
    int             wu[16];

    for(i=0; i<16; i++)
        u[i]= 1<<i;

    for(i=0; i<16; i++)
        wu[i]= 0;

    for(i=16; i<80; i++) {
        u[i]= u[i-3]|u[i-8]|u[i-14]|u[i-16];
    }

    for(i=0; i<80; i++) {
        printf("Round %02d: ", i);
        printPositions(u[i]);
    }

    for(i=0; i<80; i++) {
        addtotally(wu, u[i]);
    }

    printf("\n");
    for(i=0; i<16; i++)
        printf("Word %2d is used in %2d rounds\n",i,wu[i]);


    return(0);
}


// ---------------------------------------------------------------------------


