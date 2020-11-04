#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>

using namespace std;
#include <complex>

typedef complex<double> dcmplx;

double PI= 3.141592653589793;



// Shor.cpp


#ifndef byte
typedef unsigned char byte;
#endif


extern int      strcmp(const char*, const char*);


// --------------------------------------------------------------------- 


void printComplex(dcmplx z)
{
    double a= z.real();
    double b= z.imag();

    printf("%8.4lf + %8.4lfi", a, b);
}


void printComplexMatrix(int n, dcmplx rgMatrix[])
{
    int j;

    for(j=0;j<(n-1);j++) {
        printComplex(rgMatrix[j]); printf("    ");
        if((j&0x3)==0x3)
            printf("\n");
    }
    printComplex(rgMatrix[n-1]); 
    printf("\n");
}


void printDataMatrix(int n, double rgMatrix[])
{
    int j;

    for(j=0;j<n;j++)
        printf("%8.4f ", rgMatrix[j]);
    printf("\n");
}



void initRootsOfOne(int n, dcmplx* rgRoot)
{
    int     j;
    dcmplx  t(1.0, 0);
    double  xN= (double) n;
    dcmplx r(cos(2*PI/xN), sin(2*PI/xN));

    for(j=0;j<n; j++) {
        rgRoot[j]= t;
        t*= r;
    }
}


bool DiscreteFourierTransform(int n, dcmplx* rgR, dcmplx* rgxIn, dcmplx* rgxOut)
{
    int     i, j;
    dcmplx  cZero (0.0, 0.0);
    dcmplx  cX;
    dcmplx  cInv (1.0/((double)n), 0.0);

    for(i=0; i<n; i++) {
        cX= cZero;
        for(j=0; j<n; j++) {
            cX+= rgxIn[j]*rgR[(i*j)%n];
        }
        rgxOut[i]= cX*cInv;
    }
    return true;
}



int powmod(int a, int e, int m)
{
    int j;
    int t;
    int hB= 0;
    int b= 1;
    int as= a;

    t= e;
    for(j=0; j<32; j++) {
        if((t&1)!=0)
            hB= j+1;
        t>>= 1;
    }

    t= e;
    for(j=0; j<hB; j++) {
        if((t&1)!=0)
            b= (b*as)%m;
        t>>= 1;
        as= (as*as)%m;
    }

    return b;
}


// -------------------------------------------------------------------------



int main(int an, char** av) 

{
    int             n= 21;
    int             a= 2;
    int             i, j;
    dcmplx*         cR= new dcmplx [n];

    dcmplx          cZero (0.0, 0.0);
    dcmplx          cOne  (1.0, 0.0);
    double          xX;
    double          x1, x2, x3, xT;


    dcmplx* rgSeq= new dcmplx [n];
    dcmplx* rgTrans= new dcmplx [n];

    printf("\n %d^j (mod %d)\n", a, n);
    for(j=0; j<n;j++) {
        xX= (double) powmod(a,j,n);
        if((j%6)==0)
            xX= 1.0;
        else
            xX= 0.0;
        printf("%d^%d= %4.0lf\n", a, j, xX);
        rgSeq[j]= dcmplx (xX, 0.0);
    }
    
    printf("\n%d-th roots of unity.\n" ,n);
    initRootsOfOne(n, cR);
    printComplexMatrix(n, cR);
    
    printf("\nSequence: \n");
    printComplexMatrix(n, rgSeq);
    printf("\n");

    DiscreteFourierTransform(n, cR, rgSeq, rgTrans);
    
    printf("\nTransform: \n");
    printComplexMatrix(n, rgTrans);
    printf("\n");

    // Normalize
    xT= 0.0;
    for(j=0; j<n; j++) {
        x1= rgTrans[j].real(); 
        x2= rgTrans[j].imag(); 
        x3= sqrt(x1*x1+x2*x2);
        xT+= x3;
    }

    // Abs
    printf("\nNormalized absolute value of spectrum\n");
    for(j=0; j<n; j++) {
        x1= rgTrans[j].real(); 
        x2= rgTrans[j].imag(); 
        x3= sqrt(x1*x1+x2*x2);
        printf("(%2d, %6.2lf)\n", j, x3/xT);   
    }

    return(0);
}


// ---------------------------------------------------------------------------


