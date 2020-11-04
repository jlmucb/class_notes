#include <stdio.h>
#include <math.h>
#include <io.h>
#include "hmm.h"

//      File: MarkovData.cpp
//      Generate Markov data for english
//
//      (c) Copyright, 1991, 2007 John L. Manferdelli.  All Rights Reserved.
//


/* ------------------------------------------------------------------------ */


#define BUF  2048
#define UGBUF  256

static int  ugbuf[UGBUF];
int         ugnc={0};

static char buf[BUF];
static int nc={0};
static char *cpos;

#define EC '\\'
#define CC '#'

int nlines={0};


/* ------------------------------------------------------------------------ */


int jgetc(int in)

{
    int i;

    if(ugnc>0)
        return((int) ugbuf[--ugnc]);

    if(nc<=0) {
        if((nc=read(in,buf,BUF))<=0)
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


/* ------------------------------------------------------------------------ */

char trans[256]= {
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,1,2,3,4,5,6,7,
              8,9,10,11,12,13,14,15,
              16,17,18,19,20,21,22,23,
              24,25,26,0,0,0,0,0,
              0,1,2,3,4,5,6,7,
              8,9,10,11,12,13,14,15,
              16,17,18,19,20,21,22,23,
              24,25,26,0,0,0,0,0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,   0};

int     g_iNumchars= 0;
int     g_iNumFirst= 0;
int     g_iNumTransitions= 0;
int     g_rgiCharCount[26];
int     g_rgiFirstCount[26];
int     g_rgiTransitionCount[26*26];
int     g_rgiFirstTransitionCount[26];


// ------------------------------------------------------------------------ 


void zeroMat(int nSize, int* rgMat)

{
    int i;

    for(i=0;i<nSize; i++)
            rgMat[i]= 0;
}

int main(int an, char** av)

{
    int     in;
    int     i,j,k,n;
    int     thisC;
    int     thisCI, lastCI;
    double  x,y,u,z;
    bool    fLastSpace= true;
    bool    fPrecChar= false;

    if(an<2) {
        printf("Syntax: MarkovData Input-file [friendly-text-name]\n");
        return(0);
        }
    if((in=open(av[1],0))<0) {
        printf("Can't open %s\n",av[1]);
        return(1);
        }

    zeroMat(26, g_rgiCharCount);
    zeroMat(26, g_rgiFirstCount);
    zeroMat(26*26, g_rgiTransitionCount);

    // Count Characters and transitions
    while((thisC=jgetc(in))>=0) {

        if(thisC<256)
            thisCI= trans[thisC];
        else
            thisCI= 0;

        if(thisCI>0) {
            g_iNumchars++;
            g_rgiCharCount[thisCI-1]++;
            if(fLastSpace) {
                g_rgiFirstCount[thisCI-1]++;
                g_iNumFirst++;
            }
            if(fPrecChar) {
                g_iNumTransitions++;
                g_rgiTransitionCount[dIndex(lastCI-1, thisCI-1, 26, 26)]++;
            }
        }

        if(thisCI==0)
            fLastSpace= true;
        else
            fLastSpace= false;

        if(thisCI>0) {
            fPrecChar= true;
            lastCI= thisCI;
        }
        else
            fPrecChar= false;
    }
    
    zeroMat(26, g_rgiFirstTransitionCount);
    for(i=0; i<26; i++) {
        for(j=0; j<26; j++) {
            g_rgiFirstTransitionCount[i]+= g_rgiTransitionCount[dIndex(i, j, 26, 26)];
        }
    }

    printf("//  Text: %s\n", an>2?av[2]:av[1]);
    printf("//  %d characters, %d first characters, %d transitions\n",
                g_iNumchars, g_iNumFirst, g_iNumTransitions);
    printf("//\n");
    printf("//\n");

    // Characters
    printf("//  Counts\n");
    for(i=0; i<26; i++) {
        if((i%5)==0)
            printf("\n// ");
        printf("%c: %05d ", 'a'+i, g_rgiCharCount[i]);

    }
    printf("\n");
    printf("//\n");
    printf("//  Frequencies\n");
    x= (double) g_iNumchars;
    for(i=0; i<26; i++) {
        if((i%5)==0)
            printf("\n// ");
        y= (double) g_rgiCharCount[i];
        printf("%c: %8.4f ", 'a'+i, y/x);
    }
    printf("\n");
    printf("//\n");
    printf("//\n");

    // First Characters
    printf("//  First Character Counts\n");
    for(i=0; i<26; i++) {
        if((i%5)==0)
            printf("\n// ");
        printf("%c: %05d ", 'a'+i, g_rgiFirstCount[i]);
    }
    printf("\n");
    printf("//\n");
    printf("//  First Character Frequencies\n");
    x= (double) g_iNumFirst;
    for(i=0; i<26; i++) {
        if((i%5)==0)
            printf("\n// ");
        y= (double) g_rgiFirstCount[i];
        printf("%c: %8.4f ", 'a'+i, y/x);
    }
    printf("\n");
    printf("//\n");
    printf("//\n");

    // Transitions
    printf("//  Transition Counts\n");
    for(i=0; i<26; i++) {
        k= 0;
        for(j=0; j<26; j++) {
            if(((k++)%5)==0)
                printf("\n// ");
            printf("%c->%c: %05d ", 'a'+i, 'a'+j, 
                   g_rgiTransitionCount[dIndex(i, j, 26, 26)]);
        }
    printf("\n");
    }
    printf("\n");
    printf("//\n");
    printf("//  Transition Frequencies\n");
    for(i=0; i<26; i++) {
        k= 0;
        x= g_rgiFirstTransitionCount[i];
        for(j=0; j<26; j++) {
            y= g_rgiTransitionCount[dIndex(i, j, 26, 26)];
            if(((k++)%5)==0)
                printf("\n// ");
            printf("%c->%c: %8.4f ", 'a'+i, 'a'+j, y/x);
        }
        printf("\n");
    }
    printf("\n");
    printf("//\n");
    printf("//\n");

    return(0);
}


// ------------------------------------------------------------------------ 

