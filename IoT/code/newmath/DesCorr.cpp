#include <stdio.h>
// #include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>


// DesCorr.cpp


#ifndef byte
typedef unsigned char byte;
#endif

extern int strcmp(const char*, const char*);

// #define DEBUG


class xArgs {
public:
    int rgiArg[6];
};


int     rgiBox[32];
int     rgiPos[32];
xArgs   rgVarArgs[8];
xArgs   rgKeyArgs[8];


int sBoxOutIndex[32]= {
    // SBox 1
    9,17,23,31,
    // SBox 2
    13,28,2,18,
    // SBox 3
    24,16,30,6,
    // SBox 4
    26,20,10,1,
    // SBox 5
    8,14,25,3,
    // SBox 6
    4,29,11,19,
    // SBox 7
    32,12,22,7,
    // SBox 8
    5,27,15,21
};


int sBoxArgs[48]= {
    // SBox 1
    64,33,34,35,36,37,
    // SBox 2
    36,37,38,39,40,41,
    // SBox 3
    40,41,42,43,44,45,
    // SBox 4
    44,45,46,47,48,49,
    // SBox 5
    48,49,50,51,52,53,
    // SBox 6
    52,53,54,55,56,57,
    // SBox 7
    56,57,58,59,60,61,
    // SBox 8
    60,61,62,63,64,33
};


void initTables()
{
    int i,j,k;

    for(i=0;i<32;i++) {
        rgiBox[sBoxOutIndex[i]-1]= (i/4)+1;
        rgiPos[sBoxOutIndex[i]-1]= (i%4)+1;
    }
    for(i=0;i<8;i++) {
        rgVarArgs[i].rgiArg[j]= sBoxArgs[6*i+j];
        rgKeyArgs[i].rgiArg[j]= 6*i+j+1;
    }
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


void printByteMatrix(int n, short int rgMatrix[])
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


inline double dabs(double x)
{
    if(x>=0.0)
        return x;
    return -x;
}


inline int abs(int v)
{
    if(v<0)
        return -v;
    return v;
}


bool ToBits(long long unsigned n, int iNumBits, short int* siBits)
{
    int i;

    for(i=(iNumBits-1); i>=0; i--) {
        siBits[i]= n&1;
        n>>= 1;
    }

    return true;
}


bool shortToBits(unsigned n, int iNumBits, short int* siBits)
{
    int i;

    for(i=(iNumBits-1); i>=0; i--) {
        siBits[i]= n&1;
        n>>= 1;
    }

    return true;
}


// -------------------------------------------------------------------------


int P[32]= {
    16,  7, 20, 21, 29, 12, 28, 17,
     1, 15, 23, 26,  5, 18, 31, 10,
     2,  8, 24, 14, 32, 27,  3,  9,
    19, 13, 30,  6, 22, 11,  4, 25
};


byte s[512]= {

    /* s1 */
    14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
    0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
    4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
    15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13,

    /* s2 */
    15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
    3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
    0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
    13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9,

    /* s3 */
    10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
    13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
    13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
    1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12,

    /* s4 */
    7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
    13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
    10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
    3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14,

    /* s5 */
    2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
    14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
    4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
    11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3,

    /* s6 */
    12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
    10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
    9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
    4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13,

    /* s7 */
    4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
    13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
    1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
    6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12,

    /* s8 */
    13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
    1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
    7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
    2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11

};

byte eb[48]= {
    32, 1, 2, 3, 4, 5,
     4, 5, 6, 7, 8, 9,
     8, 9,10,11,12,13,
    12,13,14,15,16,17,
    16,17,18,19,20,21,
    20,21,22,23,24,25,
    24,25,26,27,28,29,
    28,29,30,31,32,1
};



bool getWalshBlockForSBox(int iBox, int rgInBits[10], int rgKeyBits[6])
{
    int i, j, n, m;

    n= (iBox-1);
    m= 4*n+1;
    for(i=0; i<4; i++) {
        for(j=0; j<32; j++) {
            if(P[j]==(m+i)) {
                rgInBits[i]= j+1;
                break;
            }
        }
    }

    for(i=0; i<6; i++) {
        m= (4*n+31+i)%32 + 33;
        rgInBits[i+4]= m;
    }

    for(i=0; i<6; i++) {
        rgKeyBits[i]= 6*n+i+1;
    }
    
    
    return true;
}


void printLinVar(int j)
{
    printf("x%d ",j);
}


void printFuncVarAndArgs(int j)
{
    int iBox= ((j-1)/4) + 1;
    int iPos= ((j-1)%4)+1;
    int v, k, sv, sk;

    printf("S%d[%d](", iBox, iPos);
    sk= 6*(iBox-1) +  1;
    sv= 4*(iBox-1) + 31;
    for(int m=0; m<6; m++) {
        v= (sv+m)%32 + 33;
        k= sk+m;
        printf("x%2d + rk%d", v,k);
        if(m!=5)
            printf(", ");
    }
    printf(")\n");

}


void ComputeOutputPosition(int i, short int rgiLinVar[64], short int rgiFuncVar[32])
{
#ifdef DEBUG1
    printf("\nComputeOutputPosition(%d)\n", i);
#endif

    if(i<1||i>64)
        return;
    rgiLinVar[i-1]^= 1;
    if(i>32) {
        return;
    }

    int k= P[i-1];
    rgiFuncVar[k-1]^= 1;
}


inline byte SboxVal(int iBox, unsigned x) 
{
    int m= 64*iBox;
    int k= (x&0x20)|((x&1)<<4)|((x>>1)&0xf);

    return s[m+k];
}


inline byte shortparity(unsigned short x)
{
    byte b= 0;

    for(int i=0; i<16; i++) {
        if((x&1)!=0)
            b^= 1;
        x>>= 1;
    }

    return b;
}


inline byte parity(byte x)
{
    byte b= 0;

    for(int i=0; i<8; i++) {
        if((x&1)!=0)
            b^= 1;
        x>>= 1;
    }

    return b;
}


double combSboxCorr(int iBox, byte uMask, byte blinVarMask) 
{
    unsigned    i;
    int         t= 0;
    byte        y, z;

#ifdef DEBUG3
    printf("\ncombSboxCorr(%d, %04x, %04x)\n", iBox, uMask, blinVarMask);
#endif
    for(i=0; i<64; i++) {
#ifdef DEBUG
        y= SboxVal(iBox, i);
        printf("\nS(%04x)= %04x  %04x\n", i, y, uMask);
#endif
        y= uMask&SboxVal(iBox, i);
        z= parity(y)^parity(blinVarMask&((byte)i));
        t+= 1-2*((int) z);
    }

    return ((double)t)/64.0;
}


double combInvolvedSboxCorr(int iBox, unsigned i, unsigned j)
{
    byte            uMask= 0;
    byte            uLinMask= 0;
    short int       rgsiRow[10];
    short int       rgsiCol[10];
    int             m, n;

#ifdef DEBUG3
    printf("\ncombInvolvedSboxCorr(%d, %04x, %04x)\n", iBox, i, j);
#endif

    if(i==0 && j==0)
        return 1.0;

    // construct masks
    shortToBits(i, 10, rgsiRow);
    shortToBits(j, 10, rgsiCol);

#ifdef DEBUG3
    printf("\nRowBits:\n");
    printByteMatrix(10, rgsiRow);
    printf("\nColBits:\n");
    printByteMatrix(10, rgsiCol);
#endif

    // last 6 variables of Cols are the linear function and add to linear part
    for(m=0; m<10;m++)
        rgsiCol[m]^= rgsiRow[m];
    // first 4 bits have sbox function components
    for(m=0; m<4;m++) {
        uMask|= (rgsiRow[m] <<(3-m));
    }

#ifdef DEBUG3
    printf("\nRowBits (2):\n");
    printByteMatrix(10, rgsiRow);
    printf("\nColBits (2):\n");
    printByteMatrix(10, rgsiCol);
    printf("\nMask: %x\n\n", uMask);
#endif

    // if mask is 0, there is no non-linear component so correlation is
    // 1 if Row vector is 0 and 0 otherwise
    if(uMask==0) {
        for(m=0;m<10;m++) {
            if(rgsiCol[m]!=0)
                return 0.0;
        }
        return 1.0;
    }

    // if any of the first 4 variables is present, correlation is 0
    for(m=0;m<4;m++) {
        if(rgsiCol[m]!=0)
            return 0.0;
    }

    // remainder is just Walsh coefficient of SBox
    for(m=0;m<6;m++) {
        uLinMask|= (byte) (rgsiCol[4+m] << (5-m));
    }

    return combSboxCorr(iBox, uMask, uLinMask);
}


double ComputeCorrelationEntry(short int rgiLinVar[64], short int rgiFuncVar[32])
{
    short int   rgiSVars[64];
    int         i, j, k, m;
    int         iLast= -2;
    double      Cuv= 1.0;
    double      t;
    byte        bMask;
    byte        blinVarMask;
    bool        fAllZero= true;
    bool        rgfSboxUsed[8];

 
    // if no linear variables and no function variables, 1
    for(i=0;i<64;i++) {
        if(rgiLinVar[i]!=0) {
            fAllZero= false;
            break;
        }
    }
    for(i=0;i<64;i++) {
        if(rgiFuncVar[i]!=0) {
            fAllZero= false;
            break;
        }
    }
    if(fAllZero)
        return 1.0; 


    // if any linear variable occurs unmatched by argument variables in function, 0
    for(i=0;i<64;i++)
        rgiSVars[i]= 0;
    for(i=0; i<32; i++) {
        if(rgiFuncVar[i]==0) {
            continue;
        }
        k= rgiFuncVar[i];  // the s-box
        k= (k-1)/4;
        for(j=0;j<6;j++) {
            rgiSVars[rgVarArgs[k].rgiArg[j]-1]|= 1;
        }
    }

    // if there is a linear variable not in Sbox Vars, the correlation is 0
    for(i=0;i<64;i++) {
        if(rgiLinVar[i]!=0 && rgiSVars[i]==0)
            return 0.0;
    }

    for(i=0;i<8;i++)
        rgfSboxUsed[8]= false;

    // otherwise compute Walsh transform of related variables and multiply
    // for now, if consecutive S Boxes occur print error
    // later replace even sboxes with identity for position and compute C1 then
    // multiply by C2 (replacing odd Sboxes with identity)

    // Have to compute
    //  First, group all SBoxes with same input variables 
    //  Then, multiply them
    for(m=0; m<8; m++) {        // loop over Sboxes

        bMask= 0;
        blinVarMask= 0;

        if(rgiFuncVar[sBoxOutIndex[4*m]-1]!=0)
            bMask|= 1<<3;
        if(rgiFuncVar[sBoxOutIndex[4*m+1]-1]!=0)
            bMask|= 1<<2;
        if(rgiFuncVar[sBoxOutIndex[4*m+2]-1]!=0)
            bMask|= 1<<1;
        if(rgiFuncVar[sBoxOutIndex[4*m+3]-1]!=0)
            bMask|= 1;
        
        if(bMask==0)
            continue;

        rgfSboxUsed[m]= true;
        for(j=0; j<6; j++) {
            if(rgiLinVar[rgVarArgs[m].rgiArg[j]-1]!=0)
                blinVarMask|= (1<<(j-1));
        }

        t= combSboxCorr(m, bMask, blinVarMask);
        Cuv*= t;
    }

    for(m=0;m<8;m++) {
        if(rgfSboxUsed[m]==true && rgfSboxUsed[(m+1)%8]==true)
            printf("Warning: Successive S boxes %d\n", m);
    }

    return Cuv;
}


void printExp(short int rgiLinVar[64], short int rgiFuncVar[32])
{
    bool    fSeenPlus= false;
    int     j;

    for(j=0; j<64; j++) {
        if(rgiLinVar[j]!=0) {
            if(fSeenPlus)
                printf(" + ");
            fSeenPlus= true;
            printLinVar(j+1);
        }
    }
    for(j=0; j<32; j++) {
        if(rgiFuncVar[j]!=0) {
            if(fSeenPlus)
                printf(" + ");
            fSeenPlus= true;
            printFuncVarAndArgs(j+1);
        }
    }

    return;
}


int main(int an, char** av) 

{
    int                 i, j, k;
    unsigned            uH, uL, vH, vL;
    long long unsigned  u,v,l;
    short int           rgiuBits[64];
    short int           rgivBits[64];
    short int           rgiLinVar[64];
    short int           rgiFuncVar[32];
    double              Cuv;
    unsigned            a, b;

    int                 iBox= 4;
    byte                bMask, blinVarMask;
    int                 iPosLargest;
    double              xLarge;
    double              t;
    int                 rgInBits[10];
    int                 rgKeyBits[6];


    for(j=0; j<32; j++) {
        rgiLinVar[j]= 0;
        rgiFuncVar[j]= 0;
    }
    for(j=32; j<64; j++) {
        rgiLinVar[j]= 0;
    }
    initTables();

    if(an<2) {
        printf("DesCorr [-Test] | [-OV i] | [-CM u v [len]]\n");
        return 1;
    }

    if(strcmp(av[1], "-WalshBlock")==0) {
        if(an>2) {
            sscanf(av[2], "%d", &iBox);
        }
        getWalshBlockForSBox(iBox, rgInBits, rgKeyBits);
        printf("\nWalsh Block for SBox %d\n\t", iBox);
        for(i=0; i<10; i++)
            printf("x%02d ", rgInBits[i]);
        printf(";  ");
        for(i=0; i<6; i++)
            printf("k%02d ", rgKeyBits[i]);
        printf("\n");
    }
    else if(strcmp(av[1], "-WalshBlockCorr")==0) {
        if(an<5) {
            printf("DesCorr -WalshBlockCorr SBox u v\n");
            return 1;
        }
        sscanf(av[2], "%d", &iBox);
        sscanf(av[3], "%x", &a);
        sscanf(av[4], "%x", &b);
        getWalshBlockForSBox(iBox, rgInBits, rgKeyBits);
        printf("\nWalsh Block Correlation for SBox %d\n\t", iBox);
        for(i=0; i<10; i++)
            printf("x%02d ", rgInBits[i]);
        printf(";  ");
        for(i=0; i<6; i++)
            printf("k%02d ", rgKeyBits[i]);
        printf("\n");
        t= combInvolvedSboxCorr(iBox, a, b);
        printf("C(%03x, %03x)= %8.4f\n", a,b,t);
    }
    else if(strcmp(av[1], "-Test")==0) {
        if(an>2) {
            sscanf(av[2], "%d", &iBox);
        }
        printf("Correlation test using Sbox %d\n", iBox);
        for(i=1; i<16; i++) {
            bMask= (byte) i;
            printf("\nWalsh transform of %02x dot S%d\n", bMask, iBox);
            iPosLargest= 0;
            xLarge= 0.0;
            for(j=0; j<64; j++) {
                blinVarMask= (byte) j;
                t= combSboxCorr(iBox-1, bMask, blinVarMask);
                printf("%8.4f ", t);
                if((j&0x7)==0x7)
                    printf("\n");
                t= dabs(t);
                if(t>0.0 && t>xLarge) {
                    iPosLargest= j;
                    xLarge= t;
                }
            }
            printf("\tPosition of largest (w): %d, F(w): %8.4f, p: %8.4f\n", 
                    iPosLargest, xLarge, (1.0+xLarge)/2.0);
        }
    }
    else if(strcmp(av[1], "-OV")==0) {
        if(an<3) {
            printf("DesCorr -OV i\n");
            return 1;
        }
        sscanf(av[2], "%d", &i);

        ComputeOutputPosition(i, rgiLinVar, rgiFuncVar);

#ifdef DEBUG1
        printf("\nLinear:\n");
        printByteMatrix(64, rgiLinVar);
        printf("\nFunctions:\n");
        printByteMatrix(32, rgiFuncVar);
#endif

        printExp(rgiLinVar, rgiFuncVar);
    }
    else if(strcmp(av[1], "-CM")==0) {
        if(an<5) {
            printf("DesCorr -CM uH uL vH vL [len]\n");
            return 1;
        }
        l= 1L;
        u= 0L;
        v= 0L;
        sscanf(av[2], "%x", &uH);
        sscanf(av[3], "%x", &uL);
        sscanf(av[4], "%x", &vH);
        sscanf(av[5], "%x", &vL);
        if(an>6) {
            sscanf(av[6], "%ld", &l);
        }
        u= ((long long unsigned) uH)<<32 | ((long long unsigned) uL);
        v= ((long long unsigned) vH)<<32 | ((long long unsigned) vL);
     
        printf("uH: %08x, uL: %08x, ", uH, uL);
        printf("vH: %08x, vL: %08x\n", vH, vL);
        printf("u: %lx, ", u);
        printf("v: %lx, ", v);
        printf("l: %ld\n\n", l);
    
        if(!ToBits(u, 64, rgiuBits))
            printf("Cant convert u\n");
        if(!ToBits(v, 64, rgivBits))
            printf("Cant convert v\n");

        for(i=0; i<64; i++) {
            if(rgiuBits[i]!=0) {
                ComputeOutputPosition(i+1, rgiLinVar, rgiFuncVar);
            }
            if(rgivBits[i]!=0) {
                rgiLinVar[i]^= 1;
            }
        }

#ifdef DEBUG
        printf("\nuBits:\n");
        printByteMatrix(64, rgiuBits);
        printf("\nvBits:\n");
        printByteMatrix(64, rgivBits);
        printf("\nrgiLinVar:\n");
        printByteMatrix(64, rgiLinVar);
        printf("\nrgiFuncVar:\n");
        printByteMatrix(32, rgiFuncVar);
#endif
        printExp(rgiLinVar, rgiFuncVar);

        Cuv= ComputeCorrelationEntry(rgiLinVar, rgiFuncVar);
        printf("\nC[%ld, ", u);
        printf("%ld]= ", v);
        printf("%f\n", Cuv);
    }
    else {
        printf("Must use -OV -CM  -Test or -WalshBlock option\n");
        return 1;
    }

    return 0;
}


// ---------------------------------------------------------------------------


