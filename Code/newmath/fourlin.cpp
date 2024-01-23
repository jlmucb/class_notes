#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


// fourlin.cpp


#ifndef byte
typedef unsigned char byte;
#endif


extern int	strcmp(const char*, const char*);
static int      g_numTerms= 0;  
short int       g_siTerms[513];  // up to nine variables and a constant


// warning:  Variable names adjusted for Left to Right convention


int nBoolf= 64;
byte rgBF[16*64]={
    1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 
    0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 
    1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 
    1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 
    0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 
    1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 
    1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 
    0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 
    0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 
    1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 
    0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 
    0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 
    0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 
    0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 
    0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 
    1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 
    1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 
    0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 
    1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 
    0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 
    0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 
    0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 
    0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 
    0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 
    1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 
    0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 
    0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 
    1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 
    1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 
    1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 
    1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 
    1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 
    0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 
    0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 
    1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 
    0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 
    1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 
    1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 
    0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 
    0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 
    0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 
    1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 
    1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 
    1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 
    1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 
    0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 
    0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 
    0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 
    0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 
    1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 
    0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 
    1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 
    1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 
    1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 
    0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 
    1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 
    1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 
    1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 
    1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 
    0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 
    0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 
    0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 
    1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 
    1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 
    };

int nAf= 30;
byte rgAF[16*30];


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


inline bool hasVar(int n, int nVar, int iBit)
{
    return (n&(1<<(nVar-iBit)))!=0;
}


int main(int an, char** av) 

{
    int         iKey;
    int         i, j, k, min, imin;
    int         nVar= 4;            // number of variables in poly
    int         iSize;
    byte        rgF[256];
    byte        x1, x2, x3, x4, y;
    byte        c1, c2, c3, c4;
    byte	c;
    bool        fDebugPrint= false;


    for(i=1; i<an; i++) {
        if(strcmp("-DebugPrint", av[i])==0)
            fDebugPrint= true;
    }

    printf("Four Linear Functions\n");

    iSize= 1<<nVar;

    printf("\n-------------------------------------------------\n\n");
    for(c=0; c<=1;c++) {
	for(iKey=1; iKey<iSize; iKey++) {
	    printBits(iKey, 4);
	    printf(" + %d: ", c);
	    c1= hasVar(iKey, 4, 1);
	    c2= hasVar(iKey, 4, 2);
	    c3= hasVar(iKey, 4, 3);
	    c4= hasVar(iKey, 4, 4);
	    for(i=0; i<iSize;i++) {
		x1= hasVar(i, 4, 1);
		x2= hasVar(i, 4, 2);
		x3= hasVar(i, 4, 3);
		x4= hasVar(i, 4, 4);
		rgF[i]= (c1&x1)^(c2&x2)^(c3&x3)^(c4&x4)^c;
		rgAF[(((int)c*16)+iKey)*16+i]= rgF[i];
	    }
	    printByteMatrix(iSize, rgF);
	}
    }
    printf("\n-------------------------------------------------\n");

    byte*   rgF1;
    byte*   rgF2;
    int	    n;
    for(i=0; i<nBoolf; i++) {
	rgF1= &rgBF[16*i];
	min= 16;
	imin= 16;
	printf("%02d: ", i);
	for(j=0;j<nAf; j++) {
	    rgF2= &rgAF[16*j];
	    n= 0;
	    for(k=0;k<16;k++) {
		if((rgF1[k]^rgF2[k])!=0)
		    n++;
	    }
	    printf("%2d ",n);
	    if(n<min) {
		min= n;
		imin= j;
	    }
	}
	printf("  min at %2d: %2d\n", imin, min);
    }

    return(0);
}


// ---------------------------------------------------------------------------


