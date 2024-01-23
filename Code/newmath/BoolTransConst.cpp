#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>


// BoolTransConst.cpp


#ifndef byte
typedef unsigned char byte;
#endif

extern int strcmp(const char*, const char*);

byte rgbM1[5]= {1,0,0,1,1};
byte rgbInvF[16];


// --------------------------------------------------------------------- 


unsigned reduce(int n, long long unsigned u, unsigned uM)
{
    long long unsigned luM= uM;
    long long luMask = 1;

#ifdef DEBUG1
    unsigned* pu= (unsigned*) &u;
    printf("reduce(%d, %08x %08x %08x) --- ", n, *(pu+1), *pu, uM);
#endif
    luM<<= n-1;
    luMask<<= 2*n-1;
    for(int i= (2*n); i>n; i--) {
        if(u&luMask)
            u^= luM;
        luM>>= 1;
        luMask>>= 1;
    }

#ifdef DEBUG1
    printf("returns %08x\n", (unsigned)u);
#endif
    return((unsigned) u);
}


unsigned FieldMultiply(int n, unsigned u, unsigned v, unsigned uM)
{
    long long unsigned w= 0;
    long long unsigned t= (long long unsigned) v;
    unsigned uMask= 1;

    for(int i=0; i<=n; i++) {
        if((uMask&u)!=0)
            w^= t;
        t<<= 1;
        uMask<<= 1;
    }
    return(reduce(n, w, uM));
}


int GenInverse(int n, byte* m, byte* rgInv, bool fDebugPrint=false)
{
    int             i;
    unsigned        N;         // field size
    unsigned        uM= 0;
    unsigned        u,v,w;


    printf("\tInvGF2 for 2^%d, m(x)= ", n);
    for(i=n; i>=0; i--) {
        if(m[i]!=0) {
            if(i==0)
                printf("1");
            else
                printf("x^%d + ", i);
        }
    }
    printf("\n");

    for(i=n; i>=0; i--) {
        uM<<= 1;
        if(m[i]!=0)
            uM|= 1;
    }

    N= (1<<n);
    for(i=0; i<((int)N); i++)
        rgInv[i]= 0;
    rgInv[1]= 1;

    for(u=2; u<N; u++) {
        if(rgInv[u]!=0)
            continue;
        for(v=u+1; v<N; v++) {
            w= FieldMultiply(n,u,v,uM);
            if(w==1) {
                rgInv[u]= v;
                rgInv[v]= u;
                break;
            }
        }
    }

    return 1;
}


// ---------------------------------------------------------------------------------------


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


// --------------------------------------------------------------------------------


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


// Calculate Polynomial representation
//  See Sloane and MacWilliams


int subset(int nVars, int a, int b)

{
    int i;

    for(i=0;i<nVars;i++) {
        if((a&1)==1)
            if((b&1)==0)
                return(0);
        a>>= 1;
        b>>= 1;
        }
    return(1);
}


int perfCount(byte b)
{
    int     i= 0;

    for(int j=0; j<8; j++) {
        if((b&1)!=0)
            i++;
        b>>= 1;
    }
    return i;
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


// -------------------------------------------------------------------------


byte rgFunctions[16*16];


void AddToRow(int sn, byte* rgbDest, byte* rgbToBeAdded)
{
    int i;

    for(i=0; i<sn; i++) 
        rgbDest[i]^= rgbToBeAdded[i];
}


inline int getNewPosition(int sn, int n, int* rgiOrder)
{
    int i;

    for(i=0;i<sn; i++) {
        if(rgiOrder[i]==n)
            return i;
    }

    return -1;
}


void DumpRowSol(int sn, int* rgiOrder, byte* rgM, byte* rgC)
{
    int i,j,n;

    for(i=0; i<sn; i++) {
        n= rgiOrder[i];
        printf("%3d %3d:  ",i,n);
        for(j=0; j<sn; j++)
            printf("%1d", rgM[sn*n+j]);
        printf(" %1d\n", rgC[n]);
    }
}


bool GF2RowGE(int sn, byte* rgMat, byte* rgC, byte* rgB)
//
//  Solve linear equations of form [a0, a1, ..., a15] M= [c0, c1, ... , c15]
//
{
    int*    rgiRearrange= (int*) malloc(sn*sizeof(int));
    byte*   rgM= (byte*) malloc(sn*sn);
    byte*   rgA= (byte*) malloc(sn);
    int     i,j,k;
    bool    fSwapFound= false;

    if(rgiRearrange==NULL || rgM==NULL || rgA==NULL) {
        printf("GF2Solver: Can't get memory\n");
        return false;
    }

    // virtual columns, answer
    for(i=0;i<sn;i++) {
        rgiRearrange[i]= i;
        rgA[i]= rgC[i];
    }

    printf("\nBefore transpose\n");
    DumpRowSol(sn, rgiRearrange, rgMat, rgA);
    printf("\n");

    // copy and transpose 
    for(i=0; i<sn; i++) {
        rgA[i]= rgC[i];
        for(j=0; j<sn; j++) {
            rgM[sn*j+i]= rgMat[sn*i+j];
        }
    }

    for(i=0; i<sn; i++) {
        // Make sure ith row has 1 in ith column

        if(rgM[sn*rgiRearrange[i]+i]==0) {
            // swap with another row
            fSwapFound= false;
            for(j= (i+1); j<sn; j++) {
                if(rgM[sn*rgiRearrange[j]+i]!=0) {
                    k= rgiRearrange[j];
                    rgiRearrange[j]= rgiRearrange[i];
                    rgiRearrange[i]= k;
                    fSwapFound= true;
                    break;
                }
            }
            if(fSwapFound==false) {
                printf("GF2Solver: Equations not solvable at %d\n",i);
                DumpRowSol(sn, rgiRearrange, rgM, rgA);
                goto Failed;
            }
        }

        // add row i to the remaining ones
        for(k=(i+1); k<sn; k++)  {
            if(rgM[sn*rgiRearrange[k]+i]!=0) {
                AddToRow(sn, &rgM[sn*rgiRearrange[k]], &rgM[sn*rgiRearrange[i]]);
                rgA[rgiRearrange[k]]^= rgA[rgiRearrange[i]];
            }
        }
    }

    printf("\nSolution Matrix\n");
    DumpRowSol(sn, rgiRearrange, rgM, rgA);
    printf("\n");

    // now do backwards pass
    for(i=(sn-1);i>0;i--) {
        for(j=(i-1); j>=0; j--) {
            if(rgM[sn*rgiRearrange[j]+i]!=0) {
                AddToRow(sn, &rgM[sn*rgiRearrange[j]], &rgM[sn*rgiRearrange[i]]);
                rgA[rgiRearrange[j]]^= rgA[rgiRearrange[i]];
            }
        }
    }

    printf("\nSolution Matrix after backwards pass\n");
    DumpRowSol(sn, rgiRearrange, rgM, rgA);
    printf("\n");

    for(i=0; i<sn; i++)
        rgB[i]= rgA[rgiRearrange[i]];

    if(rgiRearrange!=NULL) {
        free(rgiRearrange);
        rgiRearrange= NULL;
    }
    if(rgM!=NULL) {
        free(rgM);
        rgM= NULL;
    }
    if(rgA!=NULL) {
        free(rgA);
        rgA= NULL;
    }
    return true;

Failed:
    if(rgiRearrange!=NULL) {
        free(rgiRearrange);
        rgiRearrange= NULL;
    }
    if(rgM!=NULL) {
        free(rgM);
        rgM= NULL;
    }
    if(rgA!=NULL) {
        free(rgA);
        rgA= NULL;
    }
    return false;
}


void printVarName(byte b)
{
    if(b==0) {
        printf("y");
        return;
    }

    for(int j=0; j<8;j++) {
        if((b&1)!=0)
            printf("y%d", j+1);
        b>>= 1;
    }
}


int main(int an, char** av) 

{
    int         i, j, k;
    int         nVar= 4;            // number of variables in poly
    int         iSize;
    byte        c;
    byte*       rgF;
    byte*       rgpbF[4];
    byte        rgbC[16];
    byte        rgbAns[16];
    bool        fDebugPrint= false;
    byte        rgVarNames[16];


    for(i=1; i<an; i++) {
        if(strcmp("-DebugPrint", av[i])==0)
            fDebugPrint= true;
    }

    iSize= 1<<nVar;
    GenInverse(4, rgbM1, rgbInvF, false);
    printf("Function:\n");
    printByteMatrix(16, rgbInvF);
    PrintPerm(16, rgbInvF);

    for(i=0;i<16;i++) {
        rgFunctions[i]= 1;
    }

    rgF= &rgFunctions[16];
    for(i=1; i<16; i++) {
        for(j=0; j<4; j++) {
            rgF[16*j+i]= (rgbInvF[i]>>(3-j))&1;
        }
    }

    for(j=0;j<4;j++) {
        rgpbF[j]= &rgFunctions[16*(j+1)];
    }

    // The vectors for 0, 1, 2, 3, 4 are now filled
    int n= 5;
    for(i=0;i<16;i++) {
        if(perfCount((byte)i)<=1)
            continue;
        rgF= &rgFunctions[16*n];
        for(k=0; k<16; k++) {
            // columns to multiply
            c= 1;
            for(j=0;j<nVar;j++) {
                if((i&(1<<j))!=0)
                    c&= (rgpbF[j])[k];
            }
            rgF[k]= c;
        }
        n++;
    }

    // Now all 16 vectors are defined
    // Set up constants for x4 solution
    for(i=0; i<16; i++)
        rgbC[i]= i&1;

    // Gaussian Elimination
    if(!GF2RowGE(16, rgFunctions, rgbC, rgbAns)) {
        printf("Cant solve linear equations\n");
        return 1;
    }

    printf("Solution for x4\n");
    for(i=0;i<16;i++) {
        if(rgbAns[i]!=0)
            printByteMatrix(16, &rgFunctions[16*i]);
    }
    printf("\nx4:\n");
    printByteMatrix(16, rgbC);

    printf("\nConfirmation\n");
    byte rgT[16];
    for(j=0;j<16;j++)
        rgT[j]= 0;
    for(i=0;i<16;i++) {
        if(rgbAns[i]!=0) {
            for(j=0; j<16; j++) {
                rgT[j]^= rgFunctions[16*i+j];
            }
        }
    }
    printByteMatrix(16, rgT);

    rgVarNames[0]= 0;
    rgVarNames[1]= 0x1;
    rgVarNames[2]= 0x2;
    rgVarNames[3]= 0x4;
    rgVarNames[4]= 0x8;
    k= 5;
    for(i=0;i<16;i++) {
        if(perfCount(i)>1)
            rgVarNames[k++]= (byte) i;
    }

    printf("\nPrint as Vars: ");
    bool fNotFirst= false;
    for(i=0;i<16;i++) {
        if(rgbAns[i]!=0) {
            if(fNotFirst)
                printf(" + ");
            printVarName((byte)i);
            fNotFirst= true;
        }
    }
    printf("\n");
    
    return 0;
}


// ---------------------------------------------------------------------------


