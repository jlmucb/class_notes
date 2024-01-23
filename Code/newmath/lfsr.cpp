#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>


// FreqStudy.cpp


#ifndef byte
typedef unsigned char byte;
#endif


extern int      strcmp(const char*, const char*);


double xPr[26]= {
    0.0805, /* a */
    0.0162, /* b */
    0.032,  /* c */
    0.0365, /* d */
    0.1231, /* e */
    0.0228, /* f */
    0.0161, /* g */
    0.0514, /* h */
    0.0718, /* i */
    0.001,  /* j */
    0.0052, /* k */
    0.0403, /* l */
    0.0225, /* m */
    0.0719, /* n */
    0.0794, /* o */
    0.0229, /* p */
    0.002,  /* q */
    0.0603, /* r */
    0.0659, /* s */
    0.0959, /* t */
    0.031,  /* u */
    0.0093, /* v */
    0.0203, /* w */
    0.002,  /* x */
    0.0188, /* y */
    0.0009  /* z */
};


double xPDist[52];
double xQDist[52];


// --------------------------------------------------------------------- 



inline int index(int nr, int nc, int r, int c)
{
    return nr*r+c;
}


bool MultMat(int nr1, int nc1, int nr2, int nc2, double* rgxM1, double* rgxM2, double* rgxMo)
{
    int     i, j, k;
    double  x, y, z;


    if(nc1!=nr1)
        return false;

    for(i=0; i<nr1; i++) {
        for(j=0; j<nc1; j++) {
            z= 0.0;
            for(k=0; k<nc2; k++) {
                x= rgxM1[index(nr1, nc1, i, k)];
                y= rgxM2[index(nr2, nc2, k, j)];
                z+= x*y;
            }
            rgxMo[index(nr1, nc2, i, j)]= z;
        }
    }

    return true;
}


void PrintMat(int nr, int nc, double* rgxM)
{
    int     i, j;

    for(i=0; i<nr; i++) {
        for(j=0; j<nc; j++) {
            printf("%6.3f ", rgxM[index(nr,nc,i,j)]);
        }
        printf("\n");
    }

}


void CopyMat(int nr, int nc, double* rgxMin, double* rgxMout)
{
    memcpy(rgxMout, rgxMin, sizeof(double)*nr*nc);
}


inline double fabs(double v)
{
    if(v<0.0)
        return -v;
    return v;
}


inline int abs(int v)
{
    if(v<0)
        return -v;
    return v;
}


bool BasicStatistics(double* rgxP, double* rgxVal, int iLen, double* pxMean, 
               double* pxVar, double* pxStdDev)
{
    int i;
    double t,s;
    double xM= 0.0;
    double xVar= 0.0;

    if(rgxVal==NULL) {
        for(i=0; i<iLen; i++) {
            xM+= ((double) i)*rgxP[i];
        }
        for(i=0; i<iLen; i++) {
            t= ((double)i)-xM;
            s= t*t*rgxP[i];
	    xVar+= s;
        }
    }
    else {
        for(i=0; i<iLen; i++) {
            xM+= rgxVal[i]*rgxP[i];
        }
        for(i=0; i<iLen; i++) {
            t= rgxVal[i]-xM;
            xVar+= t*t*rgxP[i];
        }
    }

    *pxMean= xM;
    *pxVar= xVar;
    *pxStdDev= sqrt(xVar);

    return true;
}


bool Correlation(double* rgxP1, double* rgxVal1, double* rgxP2, double* rgxVal2, 
                 int iLen, double* pxCor)
{
    int	    i,j;
    double  xT= 0.0;
    double  xM1, xM2, xV1, xV2, xSD1, xSD2;
    double  t1, t2;

    BasicStatistics(rgxP1, rgxVal1, iLen, &xM1, &xV1, &xSD1);
    BasicStatistics(rgxP2, rgxVal2, iLen, &xM2, &xV2, &xSD2);
    printf("Correlation, M1 %6.4f, Var1: %6.4f, StdVar1: %6.4f\n", xM1, xV1, xSD1);
    printf("Correlation, M2 %6.4f, Var2: %6.4f, StdVar2: %6.4f\n", xM2, xV2, xSD2);
    
    if(rgxVal1==NULL) {    
	for(i=0;i<iLen;i++) {
		t1= ((double)i)-xM1;
	    for(j=0;j<iLen;j++) {
		t2= ((double)j)-xM2;
		xT+= t1*rgxP1[i]*t2*rgxP2[j];
	    }
	}
    }
    else {
    }

    if(xSD1!=0.0)
	xT/= xSD1;
    if(xSD2!=0.0)
	xT/= xSD2;

    printf("rgP1[0]= %6.4f, rgP2[0]= %6.4f, xT= %6.4f\n", rgxP1[0], rgxP2[0], xT);
    *pxCor= xT;
    return true;
}
               


// -------------------------------------------------------------------------


int main(int an, char** av) 

{
    int         iRow, iCol;
    int         i, j;
    double      t;
    double      x;
    double      xIc;
    double      xM1, xVar1, xStdDev1;
    double      xM2, xVar2, xStdDev2;
    double	xCor;
    double	rgxCorMat[26*26];
    double*	rgxP1;
    double*	rgxP2;

    for(i=0;i<52;i++) {
        xPDist[i]= xPr[i%26];
        xQDist[i]= xPr[i%26];
    }

    t= 0.0;
    xIc= 0.0;
    for(i=0; i<26; i++) {
        printf("\t%c: %6.4f\n", i+'a', xPr[i]);
        t+= xPr[i];
        xIc+= xPr[i]*xPr[i];
    }
    printf("sum P[i]: %6.3f, IC: %6.4f\n\n", t, xIc);

    BasicStatistics(xPr, NULL, 26, &xM1, &xVar1, &xStdDev1);
    printf("Mean: %6.4f, Variance: %6.4f, Standard Deviation: %6.4f\n",xM1, xVar1, xStdDev1);

    for(iRow=0; iRow<1; iRow++) {
	rgxP1= &xPDist[iRow];
	for(iCol=0; iCol<1; iCol++) {
	    rgxP2= &xQDist[iCol];
	    // Correlation(rgxP1, NULL, rgxP2, NULL, 26, &xCor);
	    Correlation(xPr, NULL, xPr, NULL, 26, &xCor);
	    printf("Correlation at %d, %d shifts: %6.4f\n", iRow, iCol, xCor);
	    // rgxCorMat[index(26, 26, iRow, iCol)]= xCor;
	}
    }

    
    // printf("Correlation matrix\n");
    // PrintMat(26, 26, rgxCorMat);

    return(0);
}


// ---------------------------------------------------------------------------


