#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


// CorrMat.cpp


#ifndef byte
typedef unsigned char byte;
#endif


const double PI= 3.14159265358979323846;


// --------------------------------------------------------------------- 


inline int index(int nr, int nc, int r, int c)
{
    return nc*r+c;
}


bool ZeroMat(int nr, int nc, double* rgxM)
{
    int i, j;
    
    for(i=0; i<nr; i++) {
        for(j=0; j<nc; j++) {
            rgxM[index(nr,nc,i,j)]= 0.0;
        }
    }
    return true;
}


bool TransMat(int nr, int nc, double* rgxM1, double* rgxM2)
{
    int i, j;
    
    if(nr!=nc)
        return false;
    for(i=0; i<nr; i++) {
        for(j=0; j<nc; j++) {
            rgxM2[index(nr,nc,i,j)]= rgxM1[index(nr,nc,j,i)];
        }
    }
    return true;
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
            printf("%8.4f ", rgxM[index(nr,nc,i,j)]);
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
    float   ta, tb;
    double  a, b;
    double  ra, rb;
    double  ca, cb, sa, sb;
    double rgxT[9];
    double rgxR[9];
    double rgxRI[9];
    double rgxRITR[9];
    double rgxTmp[9];

    a= 30.0;
    b= 45.0;

    if(an>=3) {
        sscanf(av[1], "%lf", &a);
        sscanf(av[2], "%lf", &b);
    }
    ra= a*PI/180.0;
    rb= b*PI/180.0;
    ca= cos(ra);
    sa= sin(ra);
    cb= cos(rb);
    sb= sin(rb);

    ZeroMat(3,3,rgxT);
    ZeroMat(3,3,rgxR);
    ZeroMat(3,3,rgxRI);
    ZeroMat(3,3,rgxRITR);

    rgxR[index(3,3,0,0)]= ca;
    rgxR[index(3,3,0,1)]= sa;
    rgxR[index(3,3,1,0)]= -sa;
    rgxR[index(3,3,1,1)]= ca;
    rgxR[index(3,3,2,2)]= 1.0;

    rgxT[index(3,3,0,0)]= 1.0;
    rgxT[index(3,3,1,1)]= cb;
    rgxT[index(3,3,1,2)]= sb;
    rgxT[index(3,3,2,1)]= -sb;
    rgxT[index(3,3,2,2)]= cb;

    printf("R(%f):\n",a);
    PrintMat(3, 3, rgxR);
    printf("\nT(%f):\n",b);
    PrintMat(3, 3, rgxT);

    TransMat(3, 3, rgxR, rgxRI);
    MultMat(3, 3, 3, 3, rgxRI, rgxT, rgxTmp);
    MultMat(3, 3, 3, 3, rgxTmp, rgxR, rgxRITR);

    printf("\nR**(-1)TR:\n");
    PrintMat(3, 3, rgxRITR);

    return(0);
}


// --------------------------------------------------------------------- 


