#include <stdio.h>
#include <io.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>


// readAscrand.cpp


#ifndef byte
typedef unsigned char byte;
#endif


/* --------------------------------------------------------------------- */


long long unsigned  g_uFreq1=0;
long long unsigned  g_uFreq2=0;

#define MAXDATASIZE 10000
int                 g_iValidSamples1= 0;
long long unsigned  rand0[MAXDATASIZE];
long long unsigned  rand1[MAXDATASIZE];

int                 g_iValidSamples2= 0;
long long unsigned  rand2[MAXDATASIZE];
long long unsigned  rand3[MAXDATASIZE];


int                 g_iValidSamples= 0;
byte                trand1[MAXDATASIZE];
byte                trand2[MAXDATASIZE];



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


void MarkovFreq(int iSamples, byte* rgSamples, int m=32)
{
    int         i, j, n;
    int         ia, ib;
    double      x, y, z;
    int         rgbiCounts[256][256];
    int         rgTotalCount[256];

    for(i=0; i<m; i++) {
        for(j=0; j<m; j++) {
            rgbiCounts[i][j]= 0;
        }
    }
    ia= (int) rgSamples[0]; 
    for(i=1; i<iSamples; i++) {
        ib= ia;
        ia= (int) rgSamples[i]; 
        rgbiCounts[ib][ia]+= 1;
    }
    for(i=0; i<m; i++) {
        n= 0;
        for(j=0; j<m; j++) {
            n+= rgbiCounts[j][i];
        }
        rgTotalCount[i]= n;
    }
    for(i=0; i<m; i++) {
        x= (double) rgTotalCount[i];
        printf("Pr(j|%d):\n", i);
        for(j=0; j<m; j++) {
            y= (double) rgbiCounts[j][i];
            if(y==0.0 || x==0.0)
                z= 0.0;
            else
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


void Freq(int N, byte* rgData, int m=32)
{
    int         i;
    unsigned    u;
    double      r, t, b;
    double      check;
    double      xic;
    double      H;
    double      avg;
    int         rgCounts[256];
    double      pMax= 0.0;
    double      hMax= 0.0;

    for(i=0; i<m; i++)
        rgCounts[i]= 0;
    for(i= 0; i<N; i++) {
        u= (unsigned) rgData[i];
        if(u>(unsigned)(m-1))
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
    for(i=0; i<m; i++) {
        t= (double) rgCounts[i];
        r= t/b;
        if(r>pMax)
            pMax= r;
        check+= r;
        xic+= (t*(t-1.0))/(b*(b-1.0));
        printf("%02x     %4d    %8.4f\n", i, rgCounts[i], r);
        if(r!=0.0)
            H+= -r*lg(r);
        avg+= t*((double) i);
    }
    avg/= b;
    if(pMax!=0.0)
        hMax= -lg(pMax);
    printf("IC: %8.4f,  H: %8.4f, average: %8.4f, check %8.4f, pMax: %8.4f, hMax: %8.4f\n\n", 
                        xic, H, avg, check, pMax, hMax);
    return;
}


int analyze(int iNumBuckets, int iNumShift, byte uMask)

{
    int  i;

    for(i=0;i<g_iValidSamples1; i++)
        trand1[i]= ((byte)((rand1[i]-rand0[i])>>iNumShift))&uMask;
    printf("\n\nKeypress interval at frequency %d\n", g_uFreq1);
    Freq(g_iValidSamples1, trand1, iNumBuckets);
    printf("Keypress Markov at frequency %d\n", g_uFreq1);
    MarkovFreq(g_iValidSamples1, trand1, iNumBuckets);
    printf("run test at frequency %d\n", g_uFreq1);
    runTest (g_iValidSamples1, trand1);
    printf("serial correlation test at frequency %d\n", g_uFreq1);
    serialcorrelationTest (g_iValidSamples1, trand1, 10, 100);
    printf("serial correlation test at frequency %d\n", g_uFreq1);
    serialcorrelationTest (g_iValidSamples1, trand1, iNumBuckets, 100);
    printf("permutation test at frequency %d\n", g_uFreq1);
    permutationTest (g_iValidSamples1, trand1);

    for(i=0;i<g_iValidSamples2; i++)
        trand2[i]= ((byte)((rand3[i]-rand2[i])>>iNumShift))&uMask;
    printf("\n\nKeypress interval at frequency %d\n", g_uFreq2);
    Freq(g_iValidSamples2, trand2, iNumBuckets);
    printf("Keypress Markov at frequency %d\n", g_uFreq2);
    MarkovFreq(g_iValidSamples2, trand2, iNumBuckets);
    printf("run test at frequency %d\n", g_uFreq2);
    runTest (g_iValidSamples2, trand2);
    printf("serial correlation test at frequency %d\n", g_uFreq2);
    serialcorrelationTest (g_iValidSamples2, trand2, 10, 100);
    printf("serial correlation test at frequency %d\n", g_uFreq2);
    serialcorrelationTest (g_iValidSamples2, trand2, iNumBuckets, 100);
    printf("permutation test at frequency %d\n", g_uFreq2);
    permutationTest (g_iValidSamples2, trand2);

#if 0
    fill32RootsofOne();
    m= 256;
    printf("\n\nRand1 spectral Test, length %d\n", m);
    spectralTest (4*g_iValidSamples, rand1, m);
    m= 1024;
    printf("\n\nRand1 spectral Test, length %d\n", m);
    spectralTest (4*g_iValidSamples, rand1, m);
#endif

    for(i=0;i<g_iValidSamples1; i++)
        trand1[i]= ((byte)(rand0[i]>>iNumShift))&uMask;
    printf("\n\nSample interval at frequency %d\n", g_uFreq1);
    Freq(g_iValidSamples1, trand1, iNumBuckets);
    printf("Sample Markov at frequency %d\n", g_uFreq1);
    MarkovFreq(g_iValidSamples1, trand1, iNumBuckets);
    printf("run test at frequency %d\n", g_uFreq1);
    runTest (g_iValidSamples1, trand1);
    printf("serial correlation test at frequency %d\n", g_uFreq1);
    serialcorrelationTest (g_iValidSamples1, trand1, 10, 100);
    printf("serial correlation test at frequency %d\n", g_uFreq1);
    serialcorrelationTest (g_iValidSamples1, trand1, iNumBuckets, 100);
    printf("permutation test at frequency %d\n", g_uFreq1);
    permutationTest (g_iValidSamples1, trand1);

    for(i=0;i<g_iValidSamples2; i++)
        trand2[i]= ((byte)(rand2[i]>>iNumShift))&uMask;
    printf("\n\nSample interval at frequency %d\n", g_uFreq2);
    Freq(g_iValidSamples2, trand2, iNumBuckets);
    printf("Sample Markov at frequency %d\n", g_uFreq2);
    MarkovFreq(g_iValidSamples2, trand2, iNumBuckets);
    printf("run test at frequency %d\n", g_uFreq2);
    runTest (g_iValidSamples2, trand2);
    printf("serial correlation test at frequency %d\n", g_uFreq2);
    serialcorrelationTest (g_iValidSamples2, trand2, 10, 100);
    printf("serial correlation test at frequency %d\n", g_uFreq2);
    serialcorrelationTest (g_iValidSamples2, trand2, iNumBuckets, 100);
    printf("permutation test at frequency %d\n", g_uFreq2);
    permutationTest (g_iValidSamples2, trand2);

    printf("\n\ndone\n");
    return(0);
}


// ------------------------------------------------------------------------------------


#define BUF     2048
#define UGBUF    256

static char     ugbuf[UGBUF];
static int      ugnc={0};
static char     buf[BUF];
static int      nc={0};
static char*    cpos;
static int      nlines={0};



int jgetc(int in)

{
    int i;

    if(ugnc>0)
        return((int) ugbuf[--ugnc]);

    if(nc<=0) {
        if((nc=_read(in,buf,BUF))<=0)
            return(-1);
        cpos= buf;
    }
    i= *(cpos++);
    nc--;
    if(i==((int) '\n'))
        nlines++;
    return(i);
}


int jungetc(int in,int ugc)

{
    if(ugnc>=UGBUF)
        return(-1);
    ugbuf[ugnc++]= ugc;
    return(ugc);
}


#define SZLINE      200
char                g_szLine[SZLINE];

int getline(int in)
{
    int iL= 0;
    int ic;

    ic= jgetc(in);
    if(ic<0)
        return -1;
    g_szLine[iL]= ic;
    if(ic==(int)'\n') {
        g_szLine[++iL]= 0;
        return iL;
    }
    iL++;
    for(;;) {
        ic= jgetc(in);
        if(ic<0) {
            g_szLine[iL]= 0;
            return iL;
        }
        g_szLine[iL++]= ic;
        if(ic==(int)'\n') {
            g_szLine[iL]= 0;
            return iL;
        }
    }
}


inline bool whitespace(char c)
{
    if(c==' ' || c=='\t')
        return true;
    return false;
}


// ------------------------------------------------------------------------------------



int main(int an, char** av) 

{
    int        i, n;
    int        iFile= 0;
    char*      szInFile= NULL;
    char*      szOutFile= NULL;
    char*      p;
    int        iNumBits= 5;
    int        iNumShift= 0;
    int        iUpperLimit= 32;
    byte       uMask= 0x1f;


    if(an<2 || av[1]==NULL) {
        printf("TextRandData filename\n");
        return(1);
    }
    szInFile= av[1];
    g_uFreq1= 1000;

    if((iFile=_open(szInFile, 0))<0) {
        printf("cant open %s\n\n", szInFile);
        return 1;
    }

    for(i=0; i<an; i++) {
        if(strcmp("-Bits", av[i])==0 && (i+1)<=an)
            sscanf(av[i+1], "%d", &iNumBits);
        if(strcmp("-Shift", av[i])==0 && (i+1)<=an)
            sscanf(av[i+1], "%d", &iNumShift);
    }
    iUpperLimit= (1<<iNumBits);
    uMask= (byte) (iUpperLimit-1);

    _setmode(iFile, _O_BINARY);
    printf("TextRandData analysis file %s, %d bits shifted by %d\n", szInFile, iNumBits, iNumShift);

    for(;;) {
        n= getline(iFile);
        if(n<0)
            break;
        if(n==0)
            continue;

        // Performance Frequency:
        if(strncmp((const char*) "Performance Frequency:", g_szLine, 22)==0) {
            p= &g_szLine[22];
            while(*p!=0 && whitespace(*p))
                p++;
            if(*p!=0) {
                sscanf(p, "%d", &g_uFreq2);
            }
        }

        // TC
        if(strncmp((const char*) "TC", g_szLine, 2)==0) {
            p= &g_szLine[2];
            while(*p!=0 && whitespace(*p))
                p++;
            if(*p!=0) {
                sscanf(p, "%d", &rand0[g_iValidSamples1]);
            }
            while(*p!=0 && !whitespace(*p))
                p++;
                        while(*p!=0 && whitespace(*p))
                p++;
            if(*p!=0) {
                sscanf(p, "%d", &rand1[g_iValidSamples1]);
            }
            g_iValidSamples1++;
        }

        // PC
        if(strncmp((const char*) "PC", g_szLine, 2)==0) {
            p= &g_szLine[2];
            while(*p!=0 && whitespace(*p))
                p++;
            if(*p!=0) {
                sscanf(p, "%d", &rand2[g_iValidSamples2]);
            }
            while(*p!=0 && !whitespace(*p))
                p++;
                        while(*p!=0 && whitespace(*p))
                p++;
            if(*p!=0) {
                sscanf(p, "%d", &rand3[g_iValidSamples2]);
            }
            g_iValidSamples2++;
        }
    }

    printf("\n%d TC samples collected, frequency: %Ld\n", g_iValidSamples1, g_uFreq1);
#ifdef PRINTSAMPLES
    for(i=0; i< g_iValidSamples1; i++) {
        printf("%010Ld", rand0[i]);
        printf("  %010Ld\n",rand1[i]);
    }
#endif
    printf("%d PC samples collected, frequency: %Ld\n", g_iValidSamples2, g_uFreq2);
#ifdef PRINTSAMPLES
    for(i=0; i< g_iValidSamples2; i++) {
        printf("%010Ld", rand2[i]);
        printf("  %010Ld\n", rand3[i]);
    }
#endif

    analyze(iUpperLimit, iNumShift, uMask);

#if 0
    // write them out
    if(an>2)
        szOutFile= av[2];
    else
        szOutFile= "Samples.out";
    iFile= _creat(szOutFile, _S_IWRITE);
    if(iFile<0) {
        printf("cant creat %s\n", szOutFile);
        return 1;
    }
    _setmode(iFile, _O_BINARY);
    _close(iFile);
#endif

    return(0);
}


/* --------------------------------------------------------------------- */


