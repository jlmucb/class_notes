#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>


// readrand.cpp


#ifndef byte
typedef unsigned char byte;
#endif


/* --------------------------------------------------------------------- */



inline bool LeadZeros(unsigned x, int i)
{
        if(i<1 || i>32)
                return  false;
        if((x>>(32-i))!=0)
                return false;
        return true;
}


#define MAXDATASIZE 10000
int                 g_iNumSamples= 0;
long long unsigned  g_rguData[MAXDATASIZE];

int     g_iValidSamples= 0;
byte    rand1[4*MAXDATASIZE];
byte    rand2[4*MAXDATASIZE];

// ------------------------------------------------------------------------------------


inline unsigned circ(unsigned x, int i)
{
    return (x<<i) | (x>>(32-i));
}


inline int wt(unsigned x)
{
    int      w= 0;
    unsigned y= x;

    // make this faster
    for(int i=0; i<32; i++) {
        if(y&1)
            w++;
        y>>= 1;
    }
    return w;
}


void printData(int n, unsigned rgMatrix[])
{
    int j;

    for(j=0;j<n;j++)
        printf("%08x     %08x\n", rgMatrix[2*j], rgMatrix[2*j+1]);
    printf("\n");
}


void minwt(unsigned x, unsigned y, int* piDist, int* piwt)
{

    int iDist= 32;
    int iWt= 32;
    int w;
    unsigned z;

    for(int i=0; i<31; i++) {
        z= circ(x,i);
        w= wt(z^y);
        if(w<iWt) {
            iWt= w;
            iDist= i;
        }
    }

    *piDist= iDist;
    *piwt= iWt;
    return;
}


void printbinary(unsigned x)
{
    for(int i=31;i>=0;i--)
        if(((x>>i)&1)==1)
            printf("1");
        else
            printf("0");
}


int main(int an, char** av) 

{
    int         i, j, k, n, m, mw;
    int         iOffset= 0;
    int         iFile= 0;
    char*       szInFile= NULL;
    unsigned    udw1= 0;
    unsigned    udw2= 0;
    unsigned    updw1= 0;
    unsigned    updw2= 0;
    unsigned*   pU;
    unsigned    z1, z2;
    int         N= 50;
    char*       szHeader= "Analysis Run";
    bool        fFixedShift= false;

    if(an<2 || av[1]==NULL) {
        printf("RandData filename\n");
        return(1);
    }
    szInFile= av[1];

    if((iFile=_open(szInFile, 0))<0) {
        printf("cant open %s\n\n", szInFile);
        return 1;
    }
        _setmode(iFile, _O_BINARY);

        k= MAXDATASIZE*sizeof(long long unsigned);
    i= _read(iFile, g_rguData, k);
    g_iNumSamples= i/sizeof(long long unsigned);
    printf("File %s, requested %d bytes got %d bytes, %d samples\n", szInFile, k, i, g_iNumSamples);
        _close(iFile);

        if(an>2)
                sscanf(av[2], "%d", &N);

        if(an>3)
                szHeader= av[3];

        if(an>4) {
                fFixedShift= true;
                sscanf(av[4], "%d", &m);
        }

        printf("Analysis of %d values from %s samples\n\n", N, szHeader);

    for(j= 0; j<N; j++) {
        pU= (unsigned*) &g_rguData[j];
        updw1= udw1;
        updw2= udw2;
        udw1= *(pU++);
        udw2= *pU;

        printf("\nranddw1[%d]\n", j);
        printbinary(updw1); printf("\n");
        printbinary(udw1); printf("\n");
        minwt(updw1, udw1, &k, &n); 
        if(!fFixedShift)
            m= 5*((k+4)/5); 
        z1= circ(updw1,m)^udw1; mw= wt(z1);
        printbinary(z1); printf("\t\t d: %02d, w: %02d  m: %02d mw: %02d\n",k,n,m,mw);

        printf("\nranddw2[%d]\n", j);
        printbinary(updw2); printf("\n");
        printbinary(udw2); printf("\n");
        minwt(updw2, udw2, &k, &n); 
        if(!fFixedShift)
            m= 5*((k+4)/5); 
        z2= circ(updw2,m)^udw2; mw= wt(z2);
        printbinary(z2); printf("\t\t d: %02d, w: %02d  m: %02d mw: %02d\n",k,n,m,mw);

        if(j>2 && LeadZeros(z1, 12) && LeadZeros(z2, 12)) {
            // good samples, record them
            rand1[4*g_iValidSamples  ]= (udw1>>20)&0x1f;
            rand1[4*g_iValidSamples+1]= (udw1>>15)&0x1f;
            rand1[4*g_iValidSamples+2]= (udw1>>10)&0x1f;
            rand1[4*g_iValidSamples+3]= (udw1>>5)&0x1f;
            rand2[4*g_iValidSamples  ]= (udw2>>20)&0x1f;
            rand2[4*g_iValidSamples+1]= (udw2>>15)&0x1f;
            rand2[4*g_iValidSamples+2]= (udw2>>10)&0x1f;
            rand2[4*g_iValidSamples+3]= (udw2>>5)&0x1f;
            g_iValidSamples++;
        }
    }

    printf("\n%d samples collected\n", g_iValidSamples);
    // write them out
    iFile= _creat("Samples.out", _S_IWRITE);
    if(iFile<0) {
        printf("cant creat %s\n", "Samples.out");
        return 1;
    }
    _setmode(iFile, _O_BINARY);
    _write(iFile, &g_iValidSamples, 4);
    _write(iFile, &rand1, 4*g_iValidSamples);
    _write(iFile, &rand2, 4*g_iValidSamples);
    _close(iFile);

    return(0);
}


/* --------------------------------------------------------------------- */


