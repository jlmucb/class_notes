#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


// fourexp.cpp


#ifndef byte
typedef unsigned char byte;
#endif


//  irreducible poly 1: 10011
byte rgP1[5]= {
    1,0,0,1,1
};

//  irreducible poly 2: 11001
byte rgP2[5]= {
    1,1,0,0,1
};


int iSizeofBuf= 0;
int buf[512];

short int siTestFunction[32]= { 
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
    };

extern int strcmp(const char*, const char*);

static int      g_numTerms= 0;  
short int       g_siTerms[513];  // up to nine variables and a constant


// warning:  Variable names adjusted for Left to Right convention


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


void printrep(int n, unsigned u)
{
    unsigned uMask= 1<<n;

    for(int i=n; i>=0; i--) {
        if(u&uMask)
            printf("1");
        else
            printf("0");
        uMask>>= 1;
    }
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

    if(fDebugPrint)
        for(u=0; u<N; u++) {
            printf("1/"); printrep(n,u);printf(" = ");printrep(n,rgInv[u]);printf("\n");
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


void printBilinearMatrix(int n, byte rgMatrix[])
{
    int i, j;
    int iMask;
    int iSize= 1<<n;

    printf(" -------------------------------------\n");
    printf("input|                key");

    // print column
    for(i=n;i>0;i--) {
        printf("\n     | ");
        iMask= 1<<(i-1);
        for(j=(iSize-1);j>0; j--) {
            if((iMask&j)!=0)
                printf("%1d ", n-i+1);
            else
                printf("- ");
        }
        if(i>1)
            printf("- ");
        else
            printf("c ");
    }
    printf("\n");
    printf(" -------------------------------------\n");

    // print rows
    for(i=(iSize-1);i>=0;i--) {
        printf(" ");
        printBitPos(i, n);
        printf("| ");
        for(j=(iSize-1);j>=0;j--)
            printf("%1d ", rgMatrix[iSize*j+i]&1);
        printf("\n");
    }
    printf("\n");
    printf(" -------------------------------------\n");
}


void printMatrix(int n, short int rgMatrix[])
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


void printIntegerSpectrum(int n, int rgMatrix[], bool fTallyOnly=false, int nCols=16)
{
    int j;
    int iSkip= nCols-1;
    int iLastSmallest= -1;
    int iVal, iCount;
    int iMax= 2*n;
    int iV;

    if(!fTallyOnly) {
        for(j=0;j<n;j++) {
            printf("%04d ", rgMatrix[j]);
            if((j%nCols)==iSkip)
                printf("\n");
        }
        if((j%nCols)!=iSkip)
            printf("\n");
    }

    while(iLastSmallest<iMax) {

        iVal= iMax;
        for(j=0;j<n;j++) {
            iV= abs(rgMatrix[j]);
            if(iV>iLastSmallest) {
                if(iV<iVal)
                    iVal= iV;
            }
        }

        iCount= 0;
        for(j=0; j<n; j++) {
            if(abs(rgMatrix[j])==iVal)
                iCount++;
        }

        if(iCount>0) {
            if(iLastSmallest>=0)
                printf(", ");
            printf(" %d(%d)", iCount, iVal);
        }
        iLastSmallest= iVal;

    }
    printf("\n");
        
}


void newprintTerm(int nVars, short int term)
{
    int         j;
    int         mVars= nVars/2;
    short int   iMask= 1<<(nVars-1);

    if(term==0) {
        printf("c");
        return;
    }
    for(j=1;j<=nVars;j++) {
        if((term&iMask)!=0) {
            if(j<=mVars)
                printf("k%d", j);
            else
                printf("x%d", j-mVars);
            }
        iMask>>= 1;
        }
}


void newprintPoly(int nVars, int nt, short int rgT[], int nCols=16)
{
    int j;
    int iBreak= nCols;

    if(nt==0) {
        printf("0");
        return;
        }
    for(j=(nt-1); j>=0;j--) {
        newprintTerm(nVars, rgT[j]);
        if(j!=0) {
            printf(" + ");
            if((iBreak--)<=0) {
                printf("\n\t");
                iBreak= nCols;
            }
        }
    }
}


void printTerm(int nVars, short int term)
{
    int         j;
    short int   iMask= 1<<(nVars-1);

    if(term==0) {
        printf("c");
        return;
    }
    for(j=1; j<=nVars; j++) {
        if((term&iMask)!=0) {
            printf("%d", j);
            }
        iMask>>= 1;
        }
}


void printPoly(int nVars, int nt, short int rgT[], int nCols=16)
{
    int j;
    int iBreak= nCols;

    if(nt==0) {
        printf("0\n");
        return;
        }
    for(j=(nt-1);j>=0;j--) {
        printTerm(nVars, rgT[j]);
        if(j!=0) {
            printf(" + ");
            if((iBreak--)<=0) {
                printf("\n\t");
                iBreak= nCols;
            }
        }
    }
    printf("\n");
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


short int evalTerm(int n, short int* rgiVals, short int* rgiTerm)
{
    short int   a= 1;
    int         i;
    bool        fNoMult= true;

    for(i=0; i<n; i++) {
        if(rgiTerm[i]!=0) {
            a*= rgiVals[i];
            fNoMult= false;
        }
    }
    return fNoMult?0:a;
}


bool PolyToFunction(int nVar, int nTerms, short int* rgsiTerms, short int* rgsiFunc)
{
    int         i, j;
    short int   c= rgsiTerms[0];
    int         iTerm;
    short int   a;
    short int   rgiBits[32];
    short int   rgtBits[32];
    int         iSize= 1<<nVar;

#ifdef DEBUG3
    printf("PolytoFunction %d variables, %d terms: ", nVar, nTerms);
    printMatrix(nTerms, rgsiTerms);
#endif
    for(i=0; i<iSize; i++) {
        ToBits(i, nVar, rgiBits);
        a= 0;
        for(j=1; j<nTerms; j++) {
            iTerm= rgsiTerms[j];
            ToBits(iTerm, nVar, rgtBits);
            a^= evalTerm(nVar, rgiBits, rgtBits);
        }
        rgsiFunc[i]= a^c;
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


bool FunctionToPoly(int nVars, int* pnTerms, short int* rgF, short int* rgT)

{
    int         i, j;
    int         sum;
    int         iSize= 1<<nVars;
    short int   term;

    *pnTerms= 0;
    for(i=0; i<iSize; i++) {
        sum= 0;
        for(j=0; j<iSize; j++) {
            if(subset(nVars, j, i)==1)
                sum+= rgF[j];
            }
        term= sum%2;
        if(term!=0) {
                rgT[*pnTerms]= (short int) i;
                (*pnTerms)++;
            }
        }
    return true;
}


short int evalDot(int n, short int* rgiVal1, short int* rgiVal2)
{
    short int   a= 0;
    int         i;

    for(i=0; i<n; i++) 
        a^= rgiVal1[i]&rgiVal2[i];
    return a;
}


double FRValue(int nVar, int nArg, double* rgxFunc)
{
    int         i;
    short int   siArgBits[32];
    short int   siIBits[32];
    double      x= 0.0;
    int         iSize= 1<<nVar;
    short int   a;

    // f(w)= sum (-1)**(wa + f(a))
    ToBits(nArg, nVar, siArgBits);
    for(i=0; i<iSize; i++) {
        ToBits(i, nVar, siIBits);
        a= evalDot(nVar, siIBits, siArgBits);
        x+= ((double) (1-2*a))*rgxFunc[i];
        }

    return x;
}


double FValue(int nVar, int nArg, short int* rgiFunc)
{
    int         i;
    short int   siArgBits[32];
    short int   siIBits[32];
    double      x= 0.0;
    int         iSize= 1<<nVar;
    short int   a;

    // f(w)= sum (-1)**(wa + f(a))
    ToBits(nArg, nVar, siArgBits);
    if(rgiFunc==NULL) {
        for(i=0; i<iSize; i++) {
            ToBits(i, nVar, siIBits);
            a= evalDot(nVar, siIBits, siArgBits);
            x+= (double) (1-2*a);
            }
    }
    else {
        for(i=0; i<iSize; i++) {
            ToBits(i, nVar, siIBits);
            a= evalDot(nVar, siIBits, siArgBits)^rgiFunc[i];
            x+= (double) (1-2*a);
            }
    }

#ifdef DEBUG1
    printf("FValue(%02d, %02d, %8.4f)\n", nVar, nArg, x);
#endif
    return x/((double) iSize);
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


void geneightfunction(byte* rgPoly1, byte* rgPoly2, byte c1, byte c2, byte* rgFunction,
                     bool fDebugPrint=false)
{
    byte    rgF1[16];
    byte    rgF2[16];
    byte    k,x,kr;
    byte    y,z;

    GenInverse(4, rgP1, rgF1, fDebugPrint);
    GenInverse(4, rgP2, rgF2, fDebugPrint);

    // rgFunction(k,x)= Inverse2( ((k<<<2)+(Inverse(k+x)+c1))) +c2
    for(k=0; k<16;k++) {
        kr= ((k<<2)&0xc)|((k>>2)&0x3);
        for(x=0; x<16;x++) {
            y= rgF1[x^k]^c1;
            z= rgF2[x^kr]^c2;
            rgFunction[(k<<4)|x]= z;
        }
    }
}


inline bool hasVar(int n, int nVar, int iBit)
{
    return (n&(1<<iBit))!=0;
}


int main(int an, char** av) 

{
    int         iKey;
    int         iBit;
    int         i, j, m;
    int         nVar= 4;            // number of variables in poly

    double      xSpectrum[1024];    // would be nicer if rational
    int         rgiSpectrum[1024];
    int         iSize;
    int         npolyTerms;
    int         nsomepolyTerms;
    short int   sipolyTerms[1024];
    short int   sisomepolyTerms[1024];
    byte        rgF[256];
    short int   siBitFunc[16];

    unsigned    uLc;
    short int   rgBitLc[1024];
    int         nBothVar= 2*nVar;
    int         iBothSize;
    byte        rgTermMatrix[1024];

    int         iNumTerms= 0;
    int         rgiNumKeyTerms[10];
    int         rgiNumInTerms[10];
    short int   siMask;
    short int   siLeadMask;
    bool        fFirstTerm= true;
    byte        uMask;

    bool        fDebugPrint= false;
    bool        fAllDebugPrint= false;
    bool        fDominatingBits= false;
    bool        fSpectrumTallyOnly= false;

    long long int nAvSpec, nSqSpec;
    int         iAvSpec, iSqSpec;


    for(i=1; i<an; i++) {
        if(strcmp("-DebugPrint", av[i])==0)
            fDebugPrint= true;
        if(strcmp("-AllDebugPrint", av[i])==0)
            fDebugPrint= true;
        if(strcmp("-DominatingBits", av[i])==0)
            fDominatingBits= true;
        if(strcmp("-SpectrumTallyOnly", av[i])==0)
            fSpectrumTallyOnly= true;
    }

    printf("Four function experiment high order bit is bit k1 or x1\n");
    geneightfunction(rgP1, rgP2, 0x9, 0xb, rgF, fDebugPrint);
    printf("\n\n");

    iSize= 1<<nVar;
    iBothSize= 1<<nBothVar;

    if(fDebugPrint) {
        short int rgsiTestF[256];

        printf("Poly Test, low order bit on\n");
        for(i=0; i<16;i++) {
            if((i&1)!=0)
                rgsiTestF[i]= 1;
            else
                rgsiTestF[i]= 0;
        }
        FunctionToPoly(4, &npolyTerms, rgsiTestF, sipolyTerms);
        printf("  Polynomial: ");
        printPoly(4, npolyTerms, sipolyTerms);
        printf("\n");

        printf("Poly Test, highest and second to lowest bit on + 1 \n");
        for(i=0; i<256;i++) {
            if((i&2)!=0)
                rgsiTestF[i]= 1;
            else
                rgsiTestF[i]= 0;
            if((i&0x80)!=0)
                rgsiTestF[i]^= 1;
            rgsiTestF[i]^= 1;
        }
        FunctionToPoly(8, &npolyTerms, rgsiTestF, sipolyTerms);
        printf("  Polynomial: ");
        printPoly(8, npolyTerms, sipolyTerms);
        printf("  Key bit segment Polynomial: ");
        newprintPoly(8, npolyTerms, sipolyTerms);
        printf("\n\n");
    }


    for(iKey=0; iKey<iSize; iKey++) {

        printf("\n-------------------------------------------------\n\n");
        printf("(k1 k2 k3 k4)= ");
        printBits(iKey, 4);
        printf("\nPermutation: ");
        PrintPerm(iSize, &rgF[16*iKey]);
        printf("\n");

        for(iBit=1; iBit<=nVar; iBit++) {

            printf("Output bit %d\n", iBit);
            uMask= 1<<(nVar-iBit);
            for(i=0; i<iSize;i++) {
                if((rgF[iSize*iKey+i]&uMask)!=0)
                    siBitFunc[i]= 1;
                else
                    siBitFunc[i]= 0;
            }

            // Combined Terms
            printf("  Boolean Function: ");
            printMatrix(iSize, siBitFunc);

            // Polynomial Representation
            FunctionToPoly(nVar, &npolyTerms, siBitFunc, sipolyTerms);
            printf("  Polynomial: ");
            printPoly(nVar, npolyTerms, sipolyTerms);

            // Compute spectrum
            for(i=0; i<iSize; i++) {
                xSpectrum[i]= FValue(nVar, i, siBitFunc);
            }
            // Want integer Spectrum
            nAvSpec= 0;
            nSqSpec= 0;
            for(i=0; i<iSize; i++) {
                rgiSpectrum[i]= (int)(xSpectrum[i]*iSize);
                nAvSpec+= (long long int) rgiSpectrum[i];
                nSqSpec+= (long long int) rgiSpectrum[i]*rgiSpectrum[i];
            }
            printf("  Spectrum: ");
            printIntegerSpectrum(iSize, rgiSpectrum, fSpectrumTallyOnly);
            iAvSpec= (int) nAvSpec;
            iSqSpec= (int) nSqSpec;
            printf("    Spectrum sum: %d, spectrum square sum: %d\n", iAvSpec, iSqSpec);
        }
    }
    printf("\n-------------------------------------------------\n");
    printf("\n\n\n");


    for(uLc=1; uLc<(unsigned)iSize; uLc++) {

        printf("\n________________________________________________________________\n\n");
        printf("Linear function combination (top mostbit first): ");
        printBits(uLc, nVar);
        printf("\n");

        // get linear combination
        for(i=0; i<(iSize*iSize); i++) {

            rgBitLc[i]= 0;
            for(j=0; j<nVar; j++) {
                uMask= 1<<j;
                if((uLc&uMask)!=0) {
                    if((rgF[i]&uMask)!=0)
                        rgBitLc[i]^= 1;
                }
            }
        }

        // Print boolean functions
        printf("\n\nBoolean Function: \n");
        printMatrix(iSize*iSize, rgBitLc);

        // Full polynomial
        FunctionToPoly(nBothVar, &npolyTerms, rgBitLc, sipolyTerms);
        printf("\n\nPolynomial:\n\t");
        newprintPoly(nBothVar, npolyTerms, sipolyTerms, 6);
        printf("\n");

        // Compute spectrum
        for(i=0; i<iBothSize; i++) {
            xSpectrum[i]= FValue(nBothVar, i, rgBitLc);
        }

        nAvSpec= 0;
        nSqSpec= 0;
        // Want integer Spectrum
        for(i=0; i<iBothSize; i++) {
            rgiSpectrum[i]= (int)(xSpectrum[i]*iBothSize);
            nAvSpec+= (long long int) rgiSpectrum[i];
            nSqSpec+= (long long int) rgiSpectrum[i]*rgiSpectrum[i];
        }
        printf("\n\nSpectrum:\n");
        printIntegerSpectrum(iBothSize, rgiSpectrum, fSpectrumTallyOnly);
        iAvSpec= (int) nAvSpec;
        iSqSpec= (int) nSqSpec;
        printf("  Spectrum sum: %d, spectrum square sum: %d\n", iAvSpec, iSqSpec);

        // fill in matrix term map
        for(i=0; i<iBothSize; i++)
            rgTermMatrix[i]= 0;
        for(i=0; i<npolyTerms; i++) {
            rgTermMatrix[sipolyTerms[i]]= 1;
        }
        if(fDebugPrint) {
            printf("\nPolynomial terms:\n");
            for(i=0; i<npolyTerms; i++) {
                printf("%08x ", sipolyTerms[i]);
                if((i%8)==7)
                    printf("\n");
            }
        }
        printf("\n");

        printf("\nMatrix representation key terms-columns, input terms-rows, npolyTerms: %d.\n\n",
               npolyTerms);
        printBilinearMatrix(nVar, rgTermMatrix);

        // tally terms
        iNumTerms= 0;
        for(i=0;i<nVar;i++) {
            rgiNumKeyTerms[i]= 0;
            rgiNumInTerms[i]= 0;
        }
        for(i=0;i<iSize;i++) {
            for(j=0;j<iSize;j++) {

                if(rgTermMatrix[iSize*i+j]==0)
                    continue;
                iNumTerms++;

                for(m=0; m<nVar; m++) {
                    if(hasVar(i, nVar, m))
                        rgiNumKeyTerms[nVar-1-m]++;
                    if(hasVar(j, nVar, m))
                        rgiNumInTerms[nVar-1-m]++;
                }

            }
        }
        printf("\n");
        printf("Number of terms: %d\n", iNumTerms);
        for(i=0; i<nVar; i++)
            printf("\tKey bit %d use: %d\n", i+1, rgiNumKeyTerms[i]);
        for(i=0; i<nVar; i++)
            printf("\tInput bit %d use: %d\n", i+1, rgiNumInTerms[i]);
        printf("\n");

        if(fDominatingBits) {
            for(iKey=0; iKey<nVar; iKey++) {
                nsomepolyTerms= 0;
                siMask= 1<<(nBothVar-iKey-1);
                printf("k%d Key Bit Dominated: \n  ", iKey+1);
                for(i=0; i<npolyTerms; i++) {
                    if((sipolyTerms[i]&siMask)!=0) {
                        sisomepolyTerms[nsomepolyTerms++]= sipolyTerms[i]&~siMask;
                    }
                }
                newprintPoly(nBothVar, nsomepolyTerms, sisomepolyTerms);
                printf("\n");
            }
            printf("\n");

            for(iKey=0; iKey<nVar; iKey++) {
                nsomepolyTerms= 0;
                siMask= 1<<(nVar-iKey-1);
                printf("x%d Input Bit Dominated: \n  ", iKey+1);
                for(i=0; i<npolyTerms; i++) {
                    if((sipolyTerms[i]&siMask)!=0) {
                        sisomepolyTerms[nsomepolyTerms++]= sipolyTerms[i]&~siMask;
                    }
                }
                newprintPoly(nBothVar, nsomepolyTerms, sisomepolyTerms);
                printf("\n");
            }
            printf("\n");
        }

        printf("Key Coefficient Representation:\n\t ");
        fFirstTerm= true;
        siLeadMask= 0xf0;
        for(iKey=(iSize-1); iKey>=0; iKey--) {
            nsomepolyTerms= 0;
            siMask= ((short int) iKey)<<4;
            for(i=0; i<npolyTerms; i++) {
                if((sipolyTerms[i]&siLeadMask)==siMask) {
                    sisomepolyTerms[nsomepolyTerms++]= sipolyTerms[i]&~siLeadMask;
                }
            }
            if(nsomepolyTerms>0) {
                if(!fFirstTerm)
                    printf(" + ");
                newprintPoly(nBothVar, 1, &siMask);
                printf(" (");
                newprintPoly(nBothVar, nsomepolyTerms, sisomepolyTerms);
                printf(")\n\t");
                fFirstTerm= false;
            }
        }
        printf("\n");

        printf("Input Coefficient Representation:\n\t ");
        fFirstTerm= true;
        siLeadMask= 0xf;
        for(iKey=(iSize-1); iKey>=0; iKey--) {
            nsomepolyTerms= 0;
            siMask= ((short int) iKey);
            for(i=0; i<npolyTerms; i++) {
                if((sipolyTerms[i]&siLeadMask)==siMask) {
                    sisomepolyTerms[nsomepolyTerms++]= sipolyTerms[i]&~siLeadMask;
                }
            }
            if(nsomepolyTerms>0) {
                if(!fFirstTerm)
                    printf(" + ");
                newprintPoly(nBothVar, 1, &siMask);
                printf(" (");
                newprintPoly(nBothVar, nsomepolyTerms, sisomepolyTerms);
                printf(")\n\t");
                fFirstTerm= false;
            }
        }
        printf("\n");
    }
    printf("\n________________________________________________________________\n");

    return(0);
}


/* --------------------------------------------------------------------- */
