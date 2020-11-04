#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


// lincyclestruct.cpp


#ifndef byte
typedef unsigned char byte;
#endif


extern int      strcmp(const char*, const char*);


// --------------------------------------------------------------------- 


inline int index(int nr, int nc, int r, int c)
{
    return nc*c+r;
}


bool BoolAddMat(int nr1, int nc1, int nr2, int nc2, short unsigned* rgxM1, 
                short unsigned* rgxM2, short unsigned* rgxMo)
{
    int         i, j, k;


    if(nr1!=nr2)
        return false;
    if(nc1!=nc2)
        return false;

    for(i=0; i<nr1; i++) {
        for(j=0; j<nc1; j++) {
            k= index(nr1, nc2, i, j);
            rgxMo[k]= rgxM1[k]^rgxM2[k];
        }
    }

    return true;
}


bool BoolMultMat(int nr1, int nc1, int nr2, int nc2, short unsigned* rgxM1, 
                 short unsigned* rgxM2, short unsigned* rgxMo)
{
    int              i, j, k;
    short unsigned   x, y, z;


    if(nc1!=nr2)
        return false;

    for(i=0; i<nr1; i++) {
        for(j=0; j<nc1; j++) {
            z= 0;
            for(k=0; k<nr2; k++) {
                x= rgxM1[index(nr1, nc1, i, k)];
                y= rgxM2[index(nr2, nc2, k, j)];
                z^= x&y;
            }
            rgxMo[index(nr1, nc2, i, j)]= z;
        }
    }

    return true;
}



void BoolPrintMat(int nr, int nc, short unsigned* rgxM)
{
    int     i, j;

    for(i=0; i<nr; i++) {
        for(j=0; j<nc; j++) {
            printf("%01x ", rgxM[index(nr,nc,i,j)]);
        }
        printf("\n");
    }

}


void BoolCopyMat(int nr, int nc, short unsigned* rgxMin, short unsigned* rgxMout)
{
    memcpy(rgxMout, rgxMin, sizeof(short unsigned)*nr*nc);
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


void printMatrix(int nr, int nc, short unsigned rgMatrix[])
{
    int i, j;

    for(i=0;i<nr;i++) {
        printf("\t");
        for(j=0;j<nc;j++)
            printf("%d ", rgMatrix[index(nr,nc,i,j)]);
        printf("\n");
    }
    printf("\n");
}


bool FromBits(int iNumBits, short unsigned* siBits, short unsigned* psuR)
{
    int i;

    *psuR= 0;
    for(i=0; i<iNumBits; i++) {
        (*psuR)|= siBits[i]<<i;
    }

    return true;
}


bool ToBits(short unsigned n, int iNumBits, short unsigned* siBits)
{
    int i;

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


void PrintPerm(int iSizeofPerm, short unsigned* rgPerm)
{
    int             j, n;
    short unsigned  iNext;
    short unsigned  rgSeen[256];

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


// #define DEBUG


int main(int an, char** av) 

{
    int                 iRow, iCol;
    int                 i, j, k, m;
    int                 nVar;
    int                 iSize;
    short unsigned*     rgcM= NULL;
    short unsigned*     rgR= NULL;

    short unsigned*     rgM= NULL;
    short unsigned*     rgC= NULL;
    short unsigned*     rgX= NULL;
    short unsigned*     rgY= NULL;
    short unsigned      siC= 0;

    short unsigned      suLast= 0;
    short unsigned      suResult;
    short unsigned*     rgP= NULL;

    if(an<4) {
        printf("Lincyclestruct #bits [-c const] col_1 ... col_n\n");
        return 1;
    }

    sscanf(av[1], "%d", &nVar);
    iSize= 1<<nVar;
    printf("\nCycle Structure of Boolean transformation on %d bits\n", nVar);

    i= 2;
    if(strcmp(av[2],"-c")==0) {
        sscanf(av[++i], "%x", &siC);
        i++;
    }

    j= an-i;
    if(j<nVar) {
        printf("Lincyclestruct #bits [-c const] col_1 ... col_n an: %d, j: %d, nVar: %d\n", 
                an, j, nVar);
        return 1;
    }

    rgM= new short unsigned [nVar*nVar];
    rgC= new short unsigned [nVar];
    rgX= new short unsigned [nVar];
    rgY= new short unsigned [nVar];
    rgcM= new short unsigned [nVar];
    rgR= new short unsigned [nVar];
    rgP= new short unsigned [iSize];  // stores perm rgP[i]= j means i --> j (zero base)

    if(rgM==NULL || rgC==NULL || rgX==NULL|| rgY==NULL || rgcM==NULL || rgR==NULL || rgP==NULL) {
        printf("Allocation failure\n");
        return 1;
    }

    // zero result
    memset(rgP, 0xff, iSize*(sizeof(short unsigned)));

    for(m=0; m<nVar; m++) {
        sscanf(av[i+m], "%x", &rgcM[m]);
    }

#ifdef DEBUG
    printf("av[3]: %s\n", av[3]);
    printf("c: %02x\n",siC);
    for(m=0;m<nVar;m++)
        printf("col %d: %02x, ", m+1, rgcM[m]);
    printf("\n");
#endif
    ToBits(siC, nVar, rgC);
    for(m=0; m<nVar;m++) {
        ToBits(rgcM[m], nVar, rgR);
        for(i=0;i<nVar; i++)
            rgM[index(nVar, nVar, i, m)]= rgR[i];
    }

    printf("\n");
    printf("Constant:\n");
    printMatrix(nVar, 1, rgC);
    printf("\nMatrix:\n");
    printMatrix(nVar, nVar, rgM);
    printf("\n");

    for(suLast=0; suLast<(unsigned) iSize;suLast++) {
        ToBits(suLast, nVar, rgX);
        BoolMultMat(nVar, nVar, nVar, 1, rgM, rgX, rgR);
        BoolAddMat(nVar, 1, nVar, 1, rgR, rgC, rgY);
        FromBits(nVar, rgY, &suResult);
        rgP[suLast]= suResult;
#ifdef DEBUG
        printf("X:\n");
        printMatrix(nVar, 1, rgX);
        printf("R:\n");
        printMatrix(nVar, 1, rgR);
        printf("Y:\n");
        printMatrix(nVar, 1, rgY);
        printf("Last: %02x, Result: %02x\n---\n\n", suLast, suResult);
#endif
    }

#ifdef DEBUG
    printf("Permutation:\n\t");
    for(m=0; m<iSize; m++) {
        printf("%02x ", rgP[m]);
        if((m%16)==15)
            printf("\n\t");
    }
    printf("\n\n");
#endif
    // check perm?
    PrintPerm(iSize, rgP);
    return 0;

    if(rgM!=NULL) {
        delete rgM;
        rgM= NULL;
    }
    if(rgC!=NULL) {
        delete rgC;
        rgC= NULL;
    }
    if(rgcM!=NULL) {
        delete rgcM;
        rgcM= NULL;
    }
    if(rgX!=NULL) {
        delete rgX;
        rgX= NULL;
    }
    if(rgY!=NULL) {
        delete rgY;
        rgY= NULL;
    }
    if(rgR!=NULL) {
        delete rgR;
        rgR= NULL;
    }
    if(rgP!=NULL) {
        delete rgP;
        rgP= NULL;
    }
    return(0);
}


// ---------------------------------------------------------------------------


