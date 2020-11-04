#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>


// SubPoly.cpp

unsigned rgSub[256]= {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, 
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, 
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, 
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, 
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16, 
    };


extern int strcmp(const char*, const char*);

static int      g_numTerms= 0;  
short int       g_siTerms[513];  // up to nine variables and a constant
#ifndef byte
typedef unsigned char byte;
#endif


// warning:  Variable names adjusted for Left to Right convention


/* --------------------------------------------------------------------- */


void printMatrix(int n, short int rgMatrix[])
{
    int j;

    for(j=0;j<n;j++) {
        printf("%d ", rgMatrix[j]);
	if((j%16)==15)
	    printf("\n");
    }
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
        printf("%8.4f ", rgMatrix[j]);
    printf("\n");
}


void printTerm(int nVars, short int term)
{
    int     j;
    bool    fNone= true;

    for(j=0;j<nVars; j++) {
        if(term&1) {
            // printf("%d", nVars-j);
            printf("%d", j+1);
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
    int         nVar= 8;               // number of variables in poly
    short int   siFunction[1024];
    int         iSize;
    int         npolyTerms= 0;
    short int   sipolyTerms[1024];


    iSize= 1<<nVar;
    for(i=0; i<iSize; i++) {
	siFunction[i]= (rgSub[i]>>5)&1;
    }

    printf("Boolean Function:\n");
    printMatrix(iSize, siFunction);
    printf("\n");

    // Compute Polynomial Representation
    FunctionToPoly(nVar, &npolyTerms, siFunction, sipolyTerms);

    printf("As Poly:\n");
    printPoly(nVar, npolyTerms, sipolyTerms);
    printf("\n");

    return(0);
}


/* --------------------------------------------------------------------- */
