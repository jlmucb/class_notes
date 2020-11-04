#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>


// classspec.cpp
//      Calculate spectrum and polynomials



int iSizeofBuf= 0;
int buf[512];

extern int strcmp(const char*, const char*);

static int      g_numTerms= 0;  
short int       g_siTerms[513];  // up to nine variables and a constant
#ifndef byte
typedef unsigned char byte;
#endif


byte f1[16]={
    1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 0, 1, 1, 0
};


byte f4[16]= {
    1, 0, 1, 1, 1, 0, 0, 0,
    1, 0, 0, 1, 1, 0, 0, 1
};


byte f2[16]= {
    0, 1, 0, 1, 1, 0, 1, 0,
    0, 0, 1, 1, 0, 1, 0, 1
};


byte f5[16]= {
    0, 0, 1, 0, 1, 0, 1, 1,
    0, 1, 0, 1, 1, 1, 0, 0
};


byte f3[16]= {
    1, 0, 1, 0, 1, 0, 1, 0,
    1, 0, 0, 0, 0, 1, 1, 1
};


byte f6[16]= {
    0, 0, 0, 1, 1, 1, 1, 0,
    1, 1, 0, 1, 0, 0, 1, 0
};




/* --------------------------------------------------------------------- */



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


void printMatrix1(int n, short int rgMatrix[])
{
    int j, k;
    int iNumBits;
    short int siBits[8];

    for(j=0; j<8;j++) {
        if(((n>>j)&1)!=0) {
            iNumBits= j;
            break;
        }
    }

    for(j=0;j<n;j++) {
        ToBits(j, 8, siBits);
        for(k=(iNumBits-1); k>=0; k--) {
            printf("%d", siBits[k]);
        }
        printf(" ");
    }
    printf("\n");
    for(j=0;j<n;j++)
        printf("   %d ", rgMatrix[j]);
    printf("\n");
}

void printMatrix(int n, short int rgMatrix[])
{
    int j;

    for(j=0;j<n;j++)
        printf("   %d ", rgMatrix[j]);
    printf("\n");
}


void printIntegerDataMatrix(int n, int rgMatrix[])
{
    int j;

    for(j=0;j<n;j++) {
        printf("%4d ", rgMatrix[j]);
        if((j%8)==7)
            printf("\n");
    }
    if((j%8)!=7)
        printf("\n");
}


void printDataMatrix(int n, double rgMatrix[])
{
    int j;

    for(j=0;j<n;j++)
        printf("%5.2f ", rgMatrix[j]);
    printf("\n");
}


void printTerm(int nVars, short int term)
{
    int     j;
    bool    fNone= true;

    for(j=0;j<nVars; j++) {
        if(term&1) {
            printf("%d", nVars-j);
            fNone= false;
            }
        term>>= 1;
        }
    if(fNone==true)
        printf("c");
}


void printPoly(int nVars, int nt, short int rgT[])
{
    int j;

    if(nt==0) {
        printf("0\n");
        return;
        }
    for(j=0;j<nt;j++) {
        printTerm(nVars, rgT[j]);
        if(j!=(nt-1))
            printf("+");
        }
    printf("\n");
}


// --------------------------------------------------------------------------------


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
    int         of, sum;
    int         iSize= 1<<nVars;
    short int   term;

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


short int processTerm(char* pBeg, char* pEnd, int maxVar)
{
    char        *p;
    short int   siTerm= 0;
    int         iCurTerm;

    for(p=pBeg; p<=pEnd; p++) {
        if(*p<'0' || *p>'9')
            continue;
        iCurTerm= (int)(*p-'0');
        if(iCurTerm>maxVar)
            continue;
        if(iCurTerm==0)
            return 0;
        iCurTerm= maxVar+1-iCurTerm;
        siTerm|= 1<<(iCurTerm-1);
    }

    return siTerm;
}


bool scanPoly(char* szPoly, int nVar, int* pinTerms, short int**  ppiPolyMat)

// constant term is **iPolyMat[0]

{
    char*       p= szPoly;
    char*       qBeg= NULL;
    short int*  rgsiPoly= g_siTerms;
    short int   siTerm= 0;

#ifdef DEBUG2
    printf("scanPoly %s\n", szPoly);
#endif
    *ppiPolyMat= g_siTerms;
    *pinTerms= 1;
    rgsiPoly[0]= 0;

    while(*p!='\0') {
        if((*p>='0') && (*p<='9')) {
            if(qBeg==NULL)
                qBeg= p;
        }
        else { 
            if(qBeg!=NULL) {
                siTerm= processTerm(qBeg, p-1, nVar);
                if(siTerm==0)
                    rgsiPoly[0]^= 1;
                else
                    g_siTerms[(*pinTerms)++]= siTerm;
                qBeg= NULL;
            }
        }
        p++;
    }

    if(qBeg!=NULL) {
        siTerm= processTerm(qBeg, p-1, nVar);
        if(siTerm==0)
            rgsiPoly[0]^= 1;
        else
            g_siTerms[(*pinTerms)++]= siTerm;
        qBeg= NULL;
        }

    return true;
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


int main(int an, char** av) 

{
    int         i, j, k, n;
    bool        fReadFile= false;
    bool        fPolyGiven= false;
    bool        fUseTest= false;
    char*       szPoly= NULL;
    int         nVar=4;             // number of variables in poly
    short int   siFunction[1024];
    double      xSpectrum[1024];    // would be nicer if rational
    double      xISpectrum[1024];   // would be nicer if rational
    int         rgiSpectrum[1024];
    short int*  psiFunc= NULL;
    int         iSize=1<<nVar;
    int         nTerms;
    short int*  rgsiTerms;
    int         npolyTerms;
    short int   sipolyTerms[1024];
    byte*       rgByte;
    short int   rgsiU[4];


    psiFunc= siFunction;
    printf("Analysis of Small DES Functions from Trappe and Washington\n\n");
    for(n=1; n<8; n++) {

        ToBits(n, 3, rgsiU);

        for(i=0; i<16; i++) {
            siFunction[i]= ((short int) f1[i]&rgsiU[0]) ^ 
                           ((short int) f2[i]&rgsiU[1]) ^ 
                           ((short int) f3[i]&rgsiU[2]);
        }

        printf("\n\nFunction: ");
        if(rgsiU[0])
            printf("f1+");
        if(rgsiU[1])
            printf("f2+");
        if(rgsiU[2])
            printf("f3");
        printf("\n");
        // printf("Boolean Function:\n");
        printMatrix1(iSize, psiFunc);
        printf("\n");

        // Compute spectrum
        for(i=0; i<iSize; i++) {
            xSpectrum[i]= FValue(nVar, i, psiFunc);
        }

        // Compute Polynomial Representation
        npolyTerms= 0;
        FunctionToPoly(nVar, &npolyTerms, psiFunc, sipolyTerms);

        printf("As Poly:\n");
        printPoly(nVar, npolyTerms, sipolyTerms);
        printf("\n");

        printf("Spectrum:\n");
        printDataMatrix(iSize, xSpectrum);
    }


    for(n=1; n<8; n++) {

        ToBits(n, 3, rgsiU);

        for(i=0; i<16; i++) {
            siFunction[i]= ((short int) f4[i]&rgsiU[0]) ^ 
                           ((short int) f5[i]&rgsiU[1]) ^ 
                           ((short int) f6[i]&rgsiU[2]);
        }

        printf("\n\nFunction: ");
        if(rgsiU[0])
            printf("f4+");
        if(rgsiU[1])
            printf("f5+");
        if(rgsiU[2])
            printf("f6");
        printf("\n");
        // printf("Boolean Function:\n");
        printMatrix1(iSize, psiFunc);
        printf("\n");

        // Compute spectrum
        for(i=0; i<iSize; i++) {
            xSpectrum[i]= FValue(nVar, i, psiFunc);
        }

        // Compute Polynomial Representation
        npolyTerms= 0;
        FunctionToPoly(nVar, &npolyTerms, psiFunc, sipolyTerms);

        printf("As Poly:\n");
        printPoly(nVar, npolyTerms, sipolyTerms);
        printf("\n");

        printf("Spectrum:\n");
        printDataMatrix(iSize, xSpectrum);
    }

    return(0);
}


/* --------------------------------------------------------------------- */
