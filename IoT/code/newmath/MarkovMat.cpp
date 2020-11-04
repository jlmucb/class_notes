#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>


// MarkovMat.cpp


#ifndef byte
typedef unsigned char byte;
#endif

double g_rgxMMFreq[64]= {
    0.2895, 0.1408, 0.0606, 0.0286, 0.0203, 0.0479, 0.1396, 0.2727,
    0.2305, 0.1161, 0.0634, 0.0260, 0.0297, 0.0705, 0.1833, 0.2806,
    0.1935, 0.0863, 0.0468, 0.0310, 0.0547, 0.1568, 0.2216, 0.2093,
    0.1244, 0.0754, 0.0632, 0.0490, 0.1038, 0.1889, 0.1999, 0.1954,
    0.1400, 0.1729, 0.1153, 0.0876, 0.0831, 0.1213, 0.1332, 0.1467,
    0.1556, 0.2063, 0.1569, 0.0858, 0.0626, 0.0638, 0.0930, 0.1759,
    0.1674, 0.2924, 0.1768, 0.0670, 0.0416, 0.0367, 0.0803, 0.1379,
    0.2698, 0.2796, 0.0799, 0.0421, 0.0232, 0.0295, 0.0950, 0.1809
    };


// --------------------------------------------------------------------- 


inline int index(int nr, int nc, int r, int c)
{
    return nr*r+c;
}


bool MultMat(int nr1, int nc1, int nr2, int nc2, double* rgxM1, double* rgxM2, double* rgxMo)
{
    int     i, j, k;
    double  x, y, z;


    if(nc1!=nr1)
        return false;

    for(i=0; i<nr1; i++) {
        for(j=0; j<nc1; j++) {
            z= 0.0;
            for(k=0; k<nc2; k++) {
                x= rgxM1[index(nr1, nc1, i, k)];
                y= rgxM2[index(nr2, nc2, k, j)];
                z+= x*y;
            }
            rgxMo[index(nr1, nc2, i, j)]= z;
        }
    }

    return true;
}


void PrintMat(int nr, int nc, double* rgxM)
{
    int     i, j;
    double  x;


    for(i=0; i<nr; i++) {
        for(j=0; j<nc; j++) {
            printf("%8.4f ", rgxM[index(8,8,i,j)]);
        }
        printf("\n");
    }

}


void CopyMat(int nr, int nc, double* rgxMin, double* rgxMout)
{
    memcpy(rgxMout, rgxMin, sizeof(double)*nr*nc);
}



int main(int an, char** av) 

{
    int     i, j, n;
    double rgxM1[64];
    double rgxM2[64];

    n= 8;
    if(an>1)
        sscanf(av[1], "%d", &n);

    CopyMat(8,8, g_rgxMMFreq, rgxM1);
    printf("Initial matrix:\n");
    PrintMat(8, 8, rgxM1);
    for(i=0; i<n; i++) {
        MultMat(8,8,8,8, rgxM1, rgxM1, rgxM2);
        CopyMat(8,8, rgxM2, rgxM1);
        printf("\n");
        PrintMat(8, 8, rgxM1);
        printf("\n");
    }

    return(0);
}


// --------------------------------------------------------------------- 


