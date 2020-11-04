#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>


// statsamples.cpp


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
int     g_iValidSamples= 0;
byte    rand1[4*MAXDATASIZE];
byte    rand2[4*MAXDATASIZE];


// --------------------------------------------------------------------------------


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



/* --------------------------------------------------------------------- */


double pi= 3.141592653589793;

class complex {
public:
    double Re, Im;
};



complex s_root32[32];


double complexModulus(complex& a)
{
        return sqrt(a.Re*a.Re + a.Im*a.Im);
}


void complexMult(complex& a, complex& b, complex& result)
{
        double x, y;

        x= a.Re*b.Re - a.Im*b.Im;
        y= a.Re*b.Im + a.Im*b.Re;
        result.Re= x;
        result.Im= y;
}


void complexAdd(complex& a, complex& b, complex& result)
{
        double x, y;

        x= a.Re+b.Re;
        y= a.Im+b.Im;
        result.Re= x;
        result.Im= y;
}


void fill32RootsofOne()
{
    int i;
    double a, s, c;

    for(i=0;i<32;i++) {
        a= ((double) i)*(2*pi)/32.0;
        c= cos(a);
        s= sin(a);
        s_root32[i].Re= c;
        s_root32[i].Im= s;
    }
}


inline void croot(int i, int j, int n, complex& result)
{
    double a= 2*pi/((double) n);
    a*= ((double) i)* ((double) j);
    result.Re= cos(a);
    result.Im= sin(a);
}


void MFGDFT(int N, byte* rgIn, double rgOut[])
{
    int i, j;
    complex root;
    complex acc;
    complex t;
    complex* transform= new complex [N];

    for(i=0; i<N; i++) {            // output
        acc.Re= 0.0;
        acc.Im= 0.0;
        for(j=0; j<N; j++) {        // input
            croot(i, j, N, root);
            complexMult(root, s_root32[(int)rgIn[j]], t);
            complexAdd(acc, t, acc);
        }
        rgOut[i]= complexModulus(acc);
    }
    delete transform;
}


// maybe later

void permutationTest (int N, byte* rgData)
{
}


void couponTest (int N, byte* rgData)
{
}


void serialTest (int N, byte* rgData)
{
}


void gapTest (int N, byte* rgData)
{
}


void pokerTest (int N, byte* rgData)
{
}


void runTest (int N, byte* rgData)
{
    int*    count= new int [N];
    int     i;
    int     last= 0;
    int     n= 0;
    double  d= 0.0;
    double  s;

    for(i=0;i<N;i++) {
        count[i]= 0;
    }

    for(i=0; i<N; i++) {
        if(rgData[i]>last) {
            last= rgData[i];
            n++;
        }
        else {
            n= 1;
            last= count[i+1];
            i+= 2;
        }
    }

    for(i=0; i<n; i++) {
        d+= (double) count[i];
    }
    d+= 1.0;

    s= (d-2.0*pi*((double)N)*(1.0-pi))/(2.0*pi*(1.0-pi)*sqrt(2.0));
    printf("Run test: %8.1f runs, s= %8.2f\n", d,s);

    delete count;
}


void serialcorrelationTest (int N, byte* rgData, int shift, int period)
// ((n \sum_j u_j v_j)-((\sum_j u_j)(\sum v_j))/
//          (sqrt((n (\sum_j u_j^2 – (\sum u_j)^2))(n(\sum_j v_j^2 –(\sum_j v_j)^2))
{
    double u, v, su, sv, s2u, s2v, suv;

    su= 0.0;
    sv= 0.0;
    s2u= 0.0;
    s2v= 0.0;
    suv= 0.0;

    double c, n, d;

    for(int i=0; i<period; i++) {
        u= (double) rgData[i];
        v= (double) rgData[shift+i];
        su+= u;
        sv+= v;
        s2u+= u*u;
        s2v+= v*v;
        suv+= u*v;
    }

    n= ((double) period)*suv - su*sv;
    d= sqrt(((((double) period)*s2u) - su*su)*((((double) period)*s2v) - sv*sv));
    c= n/d;
    if(d==0.0) {
        printf("zero denom\n");
        return;
    }
    printf("self-correlation coefficient for shift %d, period %d is %8.4f\n", shift, period, c);
}


void spectralTest (int N, byte* rgData, int m)
{
    int i;
    double* spec= new double[m];
    double  peak= sqrt(3.0 * ((double)m));
    int     n0= (int) (.95*((double) N)/2.0);
    int     n1= 0;
    double  d;

    MFGDFT(m, rgData, spec);
    printf("Spectrum %d\n", m);
    for(i=0; i<m; i++) {
        if(spec[i]>=peak)
            n1++;
        printf("%3d: %8.4f, ", i, spec[i]);
        if((i%4)==3)
            printf("\n");
    }

    d= ((double)(n0-n1))/sqrt(((double) m)*.95*.05/2.0);
    printf("\nd= %8.4f\n", d);
}



void MarkovFreq(int iSamples, byte* rgSamples)
{
    int         i, j, n;
    int         ia, ib;
    double      x, y, z;
    int         rgbiCounts[32][32];
    int         rgTotalCount[32];

    for(i=0; i<32; i++) {
        for(j=0; j<32; j++) {
            rgbiCounts[i][j]= 0;
        }
    }
    ia= (int) rgSamples[0]; 
    for(i=1; i<iSamples; i++) {
        ib= ia;
        ia= (int) rgSamples[i]; 
        rgbiCounts[ib][ia]+= 1;
    }
    for(i=0; i<32; i++) {
        n= 0;
        for(j=0; j<32; j++) {
            n+= rgbiCounts[j][i];
        }
        rgTotalCount[i]= n;
    }
    for(i=0; i<32; i++) {
        x= (double) rgTotalCount[i];
        printf("Pr(j|%d):\n", i);
        for(j=0; j<32; j++) {
            y= (double) rgbiCounts[j][i];
            z= y/x;
            printf("%2d: %6.4f  ", j, z);
            if((j%4)==3)
                printf("\n");
        }
        printf("\n");
    }
}


void dumpascii(char* szOut, int iNum, byte* rgData)
{
        FILE* Out= fopen(szOut, "w");

        for(int i=0; i<iNum;i++) {
        }

        fclose(Out);
}


inline double lg(double x)
{
    return log(x)/log(2.0);
}


void Freq(int N, byte* rgData)
{
    int         i;
    unsigned    u;
    double      r, t, b;
    double      check;
    double      xic;
    double      H;
    double      avg;
    int         rgCounts[32];

    for(i=0; i<32; i++)
        rgCounts[i]= 0;
    for(i= 0; i<N; i++) {
        u= (unsigned) rgData[i];
        if(u>31)
            continue;
        rgCounts[u]++;
    }
    H= 0.0;
    xic= 0.0;
        avg= 0.0;
    b= (double) N;
    check= 0.0;
    printf("Total samples: %d\n", N);
    printf("Value   Count    Frequency\n");
    for(i=0; i<32; i++) {
        t= (double) rgCounts[i];
        r= t/b;
            check+= r;
        xic+= (t*(t-1.0))/(b*(b-1.0));
        printf("%02x     %4d    %8.4f\n", i, rgCounts[i], r);
        H+= -r*lg(r);
                avg+= t*((double) i);
    }
    avg/= b;
    printf("IC: %8.4f,  H: %8.4f, average: %8.4f, check %8.4f\n\n", xic, H, avg, check);
    return;
}


int main(int an, char** av) 

{
    int         k, m;
    int         iFile= 0;
    char*       szInFile= NULL;
    char*       szSampleType= "unknown";

    if(an<2 || av[1]==NULL) {
        printf("RandData filename\n");
        return(1);
    }
    szInFile= av[1];
    if(an>2)
        szSampleType= av[2];

    if((iFile=_open(szInFile, 0))<0) {
        printf("cant open %s\n\n", szInFile);
        return 1;
    }
    _setmode(iFile, _O_BINARY);

    if(_read(iFile, &g_iValidSamples,4)<4) {
        printf("Cant read number of samples\n");
    }
    k= 4*g_iValidSamples;
    if(_read(iFile, rand1, k)<k) {
        printf("Cant read rand1 samples\n");
    }
    k= 4*g_iValidSamples;
    if(_read(iFile, rand2, k)<k) {
        printf("Cant read rand2 samples\n");
    }
    printf("File %s %d samples of type %s\n\n", szInFile, g_iValidSamples, szSampleType);
    _close(iFile);

    printf("\n\nRand1 Frequency\n");
    Freq(4*g_iValidSamples, rand1);
    printf("\nRand2 Frequency\n");
    Freq(4*g_iValidSamples, rand2);

    printf("\n\nMarkov rand1\n");
    MarkovFreq(4*g_iValidSamples, rand1);
    printf("\n\nMarkov rand2\n");
    MarkovFreq (4*g_iValidSamples, rand2);

    printf("\n\nRand1 run Test\n");
    runTest (4*g_iValidSamples, rand1);
    printf("\nRand2 run Test\n");
    runTest (4*g_iValidSamples, rand2);

    printf("\n\nRand1 serial correlation Test\n");
    serialcorrelationTest (4*g_iValidSamples, rand1, 128, 128);
    printf("\nRand2 serial correlation Test\n");
    serialcorrelationTest (4*g_iValidSamples, rand2, 128, 128);

    printf("\n\nRand1 serial correlation Test\n");
    serialcorrelationTest (4*g_iValidSamples, rand1, 512, 128);
    printf("\nRand2 serial correlation Test\n");
    serialcorrelationTest (4*g_iValidSamples, rand2, 512, 128);

    printf("\n\nRand1 permutation Test\n");
    permutationTest (4*g_iValidSamples, rand1);
    printf("\nRand2 permutation Test\n");
    permutationTest (4*g_iValidSamples, rand2);

    fill32RootsofOne();

    m= 256;
    printf("\n\nRand1 spectral Test, length %d\n", m);
    spectralTest (4*g_iValidSamples, rand1, m);
    printf("\nRand2 spectral Test, length %d\n", m);
    spectralTest (4*g_iValidSamples, rand2, m);

    m= 1024;
    printf("\n\nRand1 spectral Test, length %d\n", m);
    spectralTest (4*g_iValidSamples, rand1, m);
    printf("\nRand2 spectral Test, length %d\n", m);
    spectralTest (4*g_iValidSamples, rand2, m);

    return(0);
}


/* --------------------------------------------------------------------- */


