#include "stdio.h"

//
//  Generate 12 dimensional golay code G(24) generation
//
//  (c) 2009, John L. Manferdelli


#ifndef byte
typedef unsigned char byte;
#endif
typedef unsigned codeword;


// ------------------------------------------------------------------------------

//
//  24 is infinity position
//

codeword    g_rgbGolay[24];
byte	    N0[24]= { 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1 };


codeword rev24cw(codeword cw)
{
    int         i;
    codeword    t= 0;

    for(i=0;i<24;i++) {
        t= (t<<1)|(cw&1);
        cw>>= 1;
    }

    return t;
}


bool compress(byte* pb, codeword* pcw)
{
    int         j;
    codeword    cw;

    cw= 0;
    for(j=0; j<24;j++) {
        cw= (cw<<1)|(*pb);
        pb++;
    }
    *pcw= cw;
    return true;
}


void circulateExcept24(byte* pFrom, byte* pTo, int iC)
{
    int	    i, j;

    pTo[23]= pFrom[23];
    for(i=0; i<23; i++) {
	j= (23+i+iC)%23;
	pTo[j]= pFrom[i];
    }
}


int weight(codeword cw)
{
    int     i;
    int     w= 0;

    for(i=0; i<24; i++) {
        if(cw&1)
            w++;
        cw>>= 1;
    }

    return w;
}


void printCw(codeword cw)
{
    int         i;
    codeword    t= rev24cw(cw);
    
    for(i=0; i<24; i++) {
        printf("%1d ", t&1);
        t>>= 1;
    }
}


// --------------------------------------------------------------------------


int main(int an, char** av) 
{
    int		i,j,k;
    codeword	cw;
    byte	rgTmp[24];

    // genGolayGenMatrix();
    compress(N0, &cw);
    printf("n0: "); printCw(cw); printf(" --- weight %d\n", weight(cw));

    for(i=0; i<23; i++) {
	circulateExcept24(N0, rgTmp, -i);
	compress(rgTmp, &g_rgbGolay[i]);
    }
    g_rgbGolay[23]= 0xffffff;


    printf("Unreduced basis:\n");
    for(i=0; i<24; i++) {
	cw= g_rgbGolay[i];
	printf("%2d: ", i); printCw(cw); printf(" --- weight %d\n", weight(cw));
    }
    printf("\n\nDone\n");
    return 0;
}


// ---------------------------------------------------------------------------


