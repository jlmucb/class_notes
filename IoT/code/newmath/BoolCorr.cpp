#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


// BoolCorr.cpp


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


void printByteMatrix(int n, byte rgMatrix[])
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


bool ToBits(int n, int iNumBits, short int* siBits)
{
    int i;

    if(iNumBits>31)
        return false;

    for(i=0; i<iNumBits; i++) {
        siBits[i]= n&1;
        n>>= 1;
    }

    return true;
}


inline int abs(int v)
{
    if(v<0)
        return -v;
    return v;
}


byte evalDot(int n, short int* rgiVal1, short int* rgiVal2)
{
    byte  a= 0;

    for(int i=0; i<n; i++) 
        a^= rgiVal1[i]&rgiVal2[i];
    return a;
}


double WalshTransformValue(int nVar, int w, byte* rgF)
{
    int         iSize= 1<<nVar;
    int         i;
    short int   siArgBits[32];
    short int   siIBits[32];
    int         t= 0;
    int         ia;

    // f(w)= sum (-1)**(wa + f(a))
    ToBits(w, nVar, siArgBits);
    for(i=0; i<iSize; i++) {
        ToBits(i, nVar, siIBits);
        ia= (int) evalDot(nVar, siIBits, siArgBits)^rgF[i];
        t+= (1-2*ia);
    }

    return ((double) t)/((double) iSize);
}


void PrintPerm(int iSizeofPerm, byte* rgPerm)
{
    int     j, n;
    byte   iNext;
    byte   rgSeen[256];

    for(j=0;j<iSizeofPerm; j++)
        rgSeen[j]= 0;

    n= 0;  // <iSizeofPerm
    while(n<iSizeofPerm) {
        // new cycle
        if(rgSeen[n]!=0) {
            n++;
            continue;
        }
        printf("(%d", n);

        rgSeen[n]= 1;
        iNext= rgPerm[n];
        for(;;) {
            if(rgSeen[iNext]!=0) {
                printf(") ");
                break;
            }
            printf(" %d", iNext);
            rgSeen[iNext]= 1;
            iNext= rgPerm[iNext];
        }
    }
    printf("\n");
}


inline bool hasVar(int n, int nVar, int iBit)
{
    return (n&(1<<(iBit-1)))!=0;
}


// -------------------------------------------------------------------------


short unsigned rgTestTransform[16]= {
    2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1
};


int main(int an, char** av) 

{
    int             iRow, iCol;
    int             i, j;
    int             nVar;
    int             iSize;
    short unsigned* rgT= NULL;
    byte*           rgF= NULL;
    double*         rgC= NULL;
    double*         rgC2= NULL;
    byte            t;
    double          x;

    nVar= 4;  // test value
    printf("Calculate Correlation matrix of %d bit Boolean transformation\n", nVar);
    iSize= 1<<nVar;

    rgT= new short unsigned [iSize];
    rgF= new byte[iSize];
    rgC= new double [iSize*iSize];
    rgC2= new double [iSize*iSize];
    if(rgT==NULL || rgF==NULL || rgC==NULL) {
        printf("Allocation failure\n");
        return 1;
    }

    memcpy(rgT, rgTestTransform, sizeof(short unsigned)*iSize);
    memset(rgF, 0, sizeof(byte)*iSize);
    memset(rgC, 0, sizeof(double)*iSize*iSize);
    memset(rgC2, 0, sizeof(double)*iSize*iSize);

    printf("\nTransform:\n");
    printMatrix(iSize, rgT);

    for(iRow=0; iRow<iSize; iRow++) {
        // function linear sum of indicator components
        for(i=0; i<iSize; i++) {
            t= 0;
            for(j=0;j<nVar;j++) {
                if(hasVar(iRow, nVar, j+1)) {
                    if(((rgT[i]>>j)&1)!=0)
                        t^= 1;
                }
            }
            rgF[i]= t;
        }
        // Compute spectrum
        for(iCol=0; iCol<iSize; iCol++) {
            x= WalshTransformValue(nVar, iCol, rgF);
            rgC[index(iSize, iSize, iRow, iCol)]= x;
        }
    }
    printf("\nCorrelation Matrix (low order first):\n");
    PrintMat(iSize, iSize, rgC);

    MultMat(iSize, iSize, iSize, iSize, rgC, rgC, rgC2);
    printf("\nCorrelation Matrix squared:\n");
    PrintMat(iSize, iSize, rgC2);

    return(0);
}


// ---------------------------------------------------------------------------


