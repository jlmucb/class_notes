#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>


// BoolSpectrum.cpp
//      Calulate spectrum of boolean function



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
#ifndef byte
typedef unsigned char byte;
#endif


/* --------------------------------------------------------------------- */


void printMatrix(int n, short int rgMatrix[])
{
    int j;

    for(j=0;j<n;j++)
        printf("%d ", rgMatrix[j]);
    printf("\n");
}


void printDataMatrix(int n, double rgMatrix[])
{
    int j;

    for(j=0;j<n;j++)
        printf("%8.4f ", rgMatrix[j]);
    printf("\n");
}


void printTerm(int nVars, short int term)
{
    int     j;
    bool    fNone= true;

    for(j=1;j<=nVars; j++) {
        if(term&1) {
            printf("%d", j);
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


void CalcTestFunction()
// f(x)= 0 + 1 +  4 + 5 + 23
{
    int         i, j, nt=4;
    short int   c= 1;
    int         rgTermCodes[4]= {0x01, 0x8, 0x10, 0x6};
    short int   a;
    short int   rgiBits[5];
    short int   rgtBits[5];

    for(i=0; i<32; i++) {
        ToBits(i, 5, rgiBits);
        a= 0;
        for(j=0; j<nt; j++) {
            ToBits(rgTermCodes[j], 5, rgtBits);
            a^= evalTerm(5, rgiBits, rgtBits);
        }
        siTestFunction[i]= a^1;
    }
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
    int         nVar;               // number of variables in poly
    short int   siFunction[1024];
    double      xSpectrum[1024];    // would be nicer if rational
    double      xISpectrum[1024];   // would be nicer if rational
    short int*  psiFunc= NULL;
    int         iSize;
    int         nTerms;
    short int*  rgsiTerms;
    int         npolyTerms;
    short int   sipolyTerms[1024];
    byte*       rgByte;

    if(an<2 || av[1]==NULL) {
        printf("BoolSpectrum [-P #vars 0+12+1234] [-FB|-FS file #vars offset]\n");
        return(1);
        }
    for(i=0; i<an; i++) {
        if(strcmp(av[i],"-P")==0) {
            fPolyGiven= true;
            sscanf(av[i+1], "%d", &nVar);
            szPoly= av[i+2];
            scanPoly(szPoly, nVar, &nTerms, &rgsiTerms);
            PolyToFunction(nVar, nTerms, rgsiTerms, siFunction);
            printf("Polynomial: %s\n\n", szPoly);
            psiFunc= siFunction;
            }
        if(strcmp(av[i],"-FS")==0 || strcmp(av[i],"-FB")==0) {
            int iOffset= 0;
            int iFile= 0;
            if(an<=i+2) {
                printf("-FB/-FS option requires file #vars offset\n");
                return(1);
                }
            sscanf(av[i+2], "%d", &nVar);
            if(nVar>9) {
                printf("9 or less variables %d\n", nVar);
                return(1);
                }
            sscanf(av[i+3], "%d", &iOffset);
            iSize= 1<<nVar;
            if((iFile=open(av[i+1],0))<0) {
                printf("-FB/S cant open %s\n", av[i+1]);
                return 1;
                }
            if(strcmp(av[i],"-FS")==0) {
                lseek(iFile, iOffset*sizeof(short int), 0);
                read(iFile, siFunction, iSize*sizeof(short int));
                }
            if(strcmp(av[i],"-FB")==0) {
                lseek(iFile, iOffset, 0);
                read(iFile, siFunction, iSize);
                rgByte= (byte*) siFunction;
                for(j=(iSize-1);j>=0;j--) {
                    siFunction[j]= rgByte[j];
                    }
                } 
            psiFunc= siFunction;
            printf("vars: %d, iSize: %d, iOffset: %d\n", nVar, iSize, iOffset);
            close(iFile);
            }
        if(strcmp(av[i],"-T")==0) {
            fUseTest= true;
            nVar= 5;
            CalcTestFunction();
            psiFunc= siTestFunction;
            }
    }

    iSize= 1<<nVar;
    // Compute spectrum
    for(i=0; i<iSize; i++) {
        xSpectrum[i]= FValue(nVar, i, psiFunc);
    }

    // Compute inverse spectrum
    for(i=0; i<iSize; i++) {
        xISpectrum[i]= FRValue(nVar, i, xSpectrum);
    }

    // Compute Polynomial Representation
    FunctionToPoly(nVar, &npolyTerms, psiFunc, sipolyTerms);

    printf("Boolean Function:\n");
    printMatrix(iSize, psiFunc);
    printf("\n\n");

    printf("As Poly:\n");
    printPoly(nVar, npolyTerms, sipolyTerms);
    printf("\n\n");

    printf("Spectrum:\n");
    printDataMatrix(iSize, xSpectrum);
    printf("\n\n");

    printf("Inverse spectrum\n");
    printDataMatrix(iSize, xISpectrum);
    printf("\n\n");

    return(0);
}


/* --------------------------------------------------------------------- */
