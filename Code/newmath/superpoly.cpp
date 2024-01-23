#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <string.h>
//#include <io.h>
#include <unistd.h>


// superpoly.cpp
//      Find Superpolynomial
//      John Manferdelli


// -----------------------------------------------------------------


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


unsigned const a= (1<<31);
unsigned const b= 0x5d982395;

int g_iCt[16] = {
0,  // 0
1,  // 1
1,  // 2
2,  // 3
1,  // 4
2,  // 5
2,  // 6
3,  // 7
1,  // 8
2,  // 9
2,  // 10
3,  // 11
2,  // 12
3,  // 13
3,  // 14
4  // 15
};


#define DEBUG
inline int popcount(unsigned u)
{
    int     n= 0;
    int     i;

    for(i=0; i<8; i++) {
        n+= g_iCt[u&0xf];
        u>>= 4;
    }
#ifdef DEBUG1
        printf("popcount returns %d\n", n);
#endif
    return n;
}


unsigned nextMinterm(int iNumVars, int iNumVarsinTerm, unsigned uLastTerm)
//  Generate the cheap way
{
    unsigned    u;

    for(u=uLastTerm+1; u<(1<<iNumVars); u++) {
#ifdef DEBUG1
        printf("nextMinterm(%d, %d, %08x\n", iNumVars, iNumVarsinTerm, u);
#endif
        if(popcount(u)==iNumVarsinTerm)
            return u;
    }
    return 0;
}


inline int indexinP(unsigned ut, int iLeft, int* rgiInR, 
                    unsigned vt, int iTerm, int* rgiMinR)
{
    int     i;
    int     k= 0;

    for(i=0; i<iLeft; i++) {
        if((ut&1)!=0)
            k+= 1<<rgiInR[i];
        ut>>= 1;
    }
    for(i=0; i<iTerm; i++) {
        if((vt&1)!=0)
            k+= 1<<rgiMinR[i];
        vt>>= 1;
    }

    return k;
}


bool CalcR(int iNumVars, short unsigned* rgP, int iTerm, int* rgiMinR, 
           int iLeft, int* rgiInR, short unsigned* rgR)
//  sum rgP over ut terms
{
    int             i, j, k;
    short unsigned  u;
    
#ifdef DEBUG1
    printf("CalcR(%d, %d, %d)\n", iNumVars, iTerm, iLeft);
    printf("rgiMinR: ");
    for(i=0; i<iTerm; i++) {
        printf("%d ", rgiMinR[i]);
    }
    printf("\n");
    printf("rgiInR: ");
    for(i=0; i<iLeft; i++) {
        printf("%d ", rgiInR[i]);
    }
    printf("\n");
    printf("1<<%d= %d\n", iLeft, 1<<iLeft);
#endif

    for(i=0; i<(1<<iLeft); i++) {
        u= 0;
        for(j=0; j<(1<<iTerm); j++) {
            k= indexinP((unsigned)i, iLeft, rgiInR, (unsigned)j, iTerm, rgiMinR);
            u^= rgP[k];
#ifdef DEBUG1
            printf("\t\tIndex(In: %04x, Min: %04x), rgP[%d]= %d\n", i, j, k, rgP[k]);
#endif
        }
        rgR[i]= u;
#ifdef DEBUG1
        printf("\trgR[%04x]=  %08x\n", i, u);
#endif
    }

    return true;
}


bool TestLinear(int iNumVars, unsigned ua, unsigned ub, short unsigned* rgR)
{
    short unsigned r1, r2;

    r1= rgR[ua^ub]^rgR[0];
    r2= rgR[ua]^rgR[ub];
#ifdef DEBUG1
    printf("\tTestlinear(%04x, %04x), ", ua, ub);
    printf(" rgR[%d] ^ rgR[%d]= %d, ", ua^ub, 0, r1);
    printf("rgR[%d] ^ rgR[%d]= %d\n", ua, ub, r2);
#endif
    return r1==r2;
}


bool CheckTerm(int iNumVars, int iVar, short unsigned* rgR)
// flip iVar bit and see if term changes
{
    short unsigned  u, v;
    int             iu, iv;

    iu= 0;
    iv= iu+(1<<iVar);

    return rgR[iu]!=rgR[iv];
}


unsigned g_I= 0;
unsigned g_rgTest[32] = {
    1,4,8,9,2,1,5,0,5,6,7,8,9,3,2,4,5,6,8,1,2,4,8,9,0,2,4,5,12,45,6
};

unsigned nextTest(int nVars, unsigned uLast)
{
    unsigned    u= (7*uLast)+g_rgTest[g_I++%32];
    unsigned    uMask= (1<<nVars)-1;

#ifdef DEBUG1
    printf("\tnextTest(%d %d)= %d\n", nVars, uLast, u&uMask);
#endif
    return u&uMask;
}


// -----------------------------------------------------------------


//  P= tR+Q
//      R= superpoly, affine
//      t= term
//      Q= terms that miss at least t by at least one variable
//
//  For possible terms t, check if R(x+x')+R(0) = R(x)+R(x')
//      If test succeeds, obtain superpoly by checking numerical effect of flipping bits
//
//  1. Generate candidate minterms
//  2. Calculate sum over cube (maxterm variables)
//  3. Test R(a+b) + R(0)= R(a) + R(b)
//  4. to get poly, flip each x_i to test effect

int main(int an, char** av) 

{
    int                     nVar= 5;        // number of variables in poly
    short unsigned*         rgP= NULL;      // polynomial
    short unsigned*         rgR= NULL;      // polynomial
    int*                    rgiInR= NULL;
    int*                    rgiMinR= NULL;

    unsigned                ut= 0;          // maxterm mask, low order bit is variable x_1
    unsigned                uNextt= 0;      // maxterm mask, low order bit is variable x_1
    int                     i, j, k;
    unsigned                ua, ub;
    unsigned                u, v;

    bool                    fIsSuper;

    char*                   szInFile= NULL;
    int                     iIn;

    int                     iNumVarsinTerm= 2;
    int                     iNumVarsLeft;
    int                     iTestLinearThreshold= 16;

    rgP= new short unsigned int [1<<nVar];
    rgR= new short unsigned int [1<<nVar];
    rgiInR= new int [nVar];
    rgiMinR= new int [nVar];

    if(rgP==NULL || rgR==NULL || rgiInR==NULL || rgiMinR==NULL) {
        printf("can't allocate function arrays\n");
        return 1;
    }

    if(an>1)
        szInFile= av[1];
    else
        szInFile= (char*) "file";
    // Read function
    iIn= open(szInFile, O_RDONLY);
    if(iIn<0) {
        printf("Cant open: %s\n", szInFile);
        delete rgP;
        delete rgR;
        delete rgiInR;
        delete rgiMinR;
        return 1;
        }
    read(iIn, rgP, sizeof(short unsigned)*(1<<nVar));
    close(iIn);

#ifdef DEBUG
    printf("\nP function\n");
    printMatrix(1<<nVar, rgP);
#endif

    iNumVarsLeft= nVar-iNumVarsinTerm;

    while((uNextt=nextMinterm(nVar, iNumVarsinTerm, ut))!=0) {

#ifdef DEBUG1
        printf("nextMinterm returns %08x\n", uNextt);
#endif
        // fill variable use;
        j= 0;
        k= 0;
        u= uNextt;
        v= ((1<<nVar)-1)&(~u);
#ifdef DEBUG1
        printf("u: %08x, v: %08x\n", u, v);
#endif
        for(i=0; i<nVar; i++) {
            if((u&1)!=0) {
                rgiMinR[j++]= i;
            }
            if((v&1)!=0) {
                rgiInR[k++]= i;
            }
            u>>= 1;
            v>>= 1;
        }

        if(j!=iNumVarsinTerm || k!=iNumVarsLeft) {
            printf("Bad variable scan %d %d %d %d\n", j, iNumVarsinTerm, k, iNumVarsLeft);
            delete rgP;
            delete rgR;
            delete rgiInR;
            delete rgiMinR;
            return 1;
        }

        if(!CalcR(nVar, rgP, iNumVarsinTerm, rgiMinR, iNumVarsLeft, rgiInR, rgR)) {
            printf("Can't calculate cube sum\n");
            delete rgP;
            delete rgR;
            delete rgiInR;
            delete rgiMinR;
            return 1;
        }
#ifdef DEBUG
        printf("\nR function\n");
        printMatrix(1<<iNumVarsLeft, rgR);
#endif
        ub= 0;
        fIsSuper= false;
        for(i=1; i<(1<<iNumVarsLeft); i++) {
            if(rgR[i-1]!=rgR[i]) {
                fIsSuper= true;
                break;
            }
        }
        for(i=0; i<iTestLinearThreshold; i++) {
            ua= nextTest(iNumVarsLeft, ub);
            ub= nextTest(iNumVarsLeft, ua);
            if(!TestLinear(iNumVarsLeft, ua, ub, rgR)) {
                fIsSuper= false;
                break;
            }
        }

        if(fIsSuper) {
            printf("Minterm: ");
            for(i=0; i<iNumVarsinTerm; i++)
                printf("%d", rgiMinR[i]+1);
            printf(", ");
            printf("superpoly: ");
            for(i=0; i<iNumVarsLeft; i++) {
                if(CheckTerm(iNumVarsLeft, i, rgR)) {
                    printf("%d ", rgiInR[i]+1);
                }
            }
            printf("\n");
        }
        else {
            printf("No superpoly\n");
        }

    ut= uNextt;
    }

    delete rgP;
    delete rgR;
    delete rgiInR;
    delete rgiMinR;

    printf("\nDone\n");
    return 0;
}


// ----------------------------------------------------------------------


