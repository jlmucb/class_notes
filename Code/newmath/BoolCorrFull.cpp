#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


// BoolCorrFull.cpp


#ifndef byte
typedef unsigned char byte;
#endif


extern int      strcmp(const char*, const char*);


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

    for(i=0; i<nr; i++) {
        for(j=0; j<nc; j++) {
            printf("%6.3f ", rgxM[index(nr,nc,i,j)]);
        }
        printf("\n");
    }

}


void CopyMat(int nr, int nc, double* rgxMin, double* rgxMout)
{
    memcpy(rgxMout, rgxMin, sizeof(double)*nr*nc);
}


// --------------------------------------------------------------------- 


void printBits(int i, int nBits)
{
    int     j;
    int     iMask= 1<<(nBits-1);

    for(j=0; j<nBits; j++) {
        if((iMask&i)!=0)
            printf("1");
        else
            printf("0");
        iMask>>= 1;
    }
}


void printBitPos(int i, int nBits)
{
    int     j;
    int     iMask= 1<<(nBits-1);

    if(i==0) {
        printf("   c");
        return;
    }
    for(j=1; j<=nBits; j++) {
        if((iMask&i)!=0)
            printf("%1d",j);
        else
            printf(" ");
        iMask>>= 1;
    }
}


void printMatrix(int n, short unsigned rgMatrix[])
{
    int j;

    for(j=0;j<n;j++) {
        printf("%d ", rgMatrix[j]);
        if((j%32)==31)
            printf("\n");
    }
    if((j%32)!=31)
        printf("\n");
}


void printIntegerDataMatrix(int n, int rgMatrix[])
{
    int j;

    for(j=0;j<n;j++) {
        printf("%04d ", rgMatrix[j]);
        if((j%16)==15)
            printf("\n");
    }
    if((j%16)!=15)
        printf("\n");
}


void printDataMatrix(int n, double rgMatrix[])
{
    int j;

    for(j=0;j<n;j++)
        printf("%8.4f ", rgMatrix[j]);
    printf("\n");
}


inline int abs(int v)
{
    if(v<0)
        return -v;
    return v;
}


short unsigned evalDot(int nVar, short unsigned u1, short unsigned u2)
{
    short unsigned a= 0;

    for(int i=0; i<nVar; i++) {
        a^= u1&u2&1;
	u1>>= 1;
	u2>>= 1;
    }
    return a;
}


void projectMask(int iSize, int nVar, short unsigned uMask, 
		short unsigned* rgu1, short unsigned* rgu2)
{   
    int i;

    for(i=0;i<iSize;i++) {
	rgu2[i]= evalDot(nVar, uMask, rgu1[i]);
    }
    return;
}


// -------------------------------------------------------------------------


#include "func.inc"


int main(int an, char** av) 

{
    int             iRow, iCol;
    int             i, j;
    int             nVar;
    int             iSize;

    short unsigned* rgT= NULL;  // Function vector
    short unsigned* rgF= NULL;  // u.T
    short unsigned* rgC= NULL;
    double*         rgW= NULL;

    short unsigned  t;
    short unsigned  x;
    short unsigned  u;
    short unsigned  v;

    int		    iF;
    double	    xF;

    nVar= g_nVar;
    printf("Calculate Correlation matrix of %d bit Boolean transformation\n", nVar);
    iSize= 1<<nVar;

    rgT= new short unsigned [iSize];
    rgF= new short unsigned [iSize];
    rgC= new short unsigned [iSize];
    rgW= new double [iSize*iSize];
    if(rgT==NULL || rgF==NULL || rgC==NULL) {
        printf("Allocation failure\n");
        return 1;
    }

    memcpy(rgT, rgTestTransform, sizeof(short unsigned)*iSize);
    memset(rgF, 0, sizeof(short unsigned)*iSize);
    memset(rgC, 0, sizeof(short unsigned)*iSize);
    memset(rgW, 0, sizeof(double)*iSize*iSize);

    printf("\nTransform:\n");
    printMatrix(iSize, rgT);

    for(iRow=0; iRow<iSize; iRow++) {
	printf("\n");
	printBits(iRow, nVar);
	printf("=u: ");
	projectMask(iSize, nVar, (short unsigned)iRow, rgT, rgF); 
	for(j=0;j<iSize; j++) {
	    printf("%d ", rgF[j]);
	}
	printf("\n");
	for(iCol=0; iCol<iSize; iCol++) {
	    printf("\t");
	    printBits(iCol, nVar);
	    printf("=v: ");
	    for(x=0; x<(short unsigned) iSize; x++) {
		rgC[x]= evalDot(nVar, (short unsigned) iCol, x);
		printf("%d", rgC[x]);
	    }
	    iF= 0;
	    for(j=0; j<iSize; j++) {
		iF+= 1-2*((int)(rgC[j]^rgF[j]));
	    }
	    xF=((double) iF)/((double) iSize);
	    rgW[iSize*iRow+iCol]= xF;
	printf("%8.5f\n", xF);
	}
    }
    printf("\nCorrelation Matrix (low order first):\n");
    PrintMat(iSize, iSize, rgW);

    return(0);
}


// ---------------------------------------------------------------------------


