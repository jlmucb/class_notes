#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


//
// SymCorMat.cpp
// Symbolically multiply correlation matrices
//
//

#ifndef byte
typedef unsigned char byte;
#endif

extern int      strcmp(const char*, const char*);
int	g_iLineSize=80;


// --------------------------------------------------------------------- 


#define TAPESIZE   512000
int     g_iNumLeftinTape= TAPESIZE;
char    g_strTape[TAPESIZE];
char*   g_strNextFree= g_strTape;

char*   g_strZero= "0";
char*   g_strOne= "1";


char* getString(int iNum)
{
    char *p= g_strNextFree;

    if(g_iNumLeftinTape<iNum) {
        printf("Out of tape\n");
        return NULL;
    }

    g_strNextFree+= iNum;
    g_iNumLeftinTape-= iNum;
    return p;
}


char* ConcatStrings(char* strOne, char* strTwo)
{
    return NULL;
}


// --------------------------------------------------------------------- 


inline int index(int nr, int nc, int r, int c)
{
    return nr*r+c;
}
 

char* addTerms(char* t1, char* t2)
{
    int     lt1, lt2;
    char*   pOut= NULL;

    if(t1==NULL || t2==NULL)
        return NULL;

    if(strcmp(g_strZero, t1)==0)
        return t2;
    if(strcmp(g_strZero, t2)==0)
        return t1;

    lt1= strlen(t1);
    lt2= strlen(t2);
    if((pOut=getString(lt1+lt2+4))==NULL)
        return NULL;

    strcat(pOut, t1);
    strcat(pOut+lt1, " + ");
    strcat(pOut+lt1+3, t2);
    *(pOut+lt1+lt2+3)= '\0';

    return pOut;
}
 

char* multTerms(char* t1, char* t2)
{
    int     lt1, lt2;
    char*   pOut= NULL;

    if(t1==NULL || t2==NULL)
        return NULL;

    if(strcmp(g_strZero, t1)==0 || strcmp(g_strZero, t2)==0)
        return g_strZero;
    if(strcmp(g_strOne, t1)==0)
        return t2;
    if(strcmp(g_strOne, t2)==0)
        return t1;

    lt1= strlen(t1);
    lt2= strlen(t2);
    if((pOut=getString(lt1+lt2+2))==NULL)
        return NULL;

    strcat(pOut, t1);
    *(pOut+lt1)= ' ';
    strcat(pOut+lt1+1, t2);
    *(pOut+lt1+lt2+1)= '\0';

    return pOut;
}


bool SymMultMat(int nr1, int nc1, int nr2, int nc2, char** rgMat1, char** rgMat2, char** rgMatOut)
{
    int     i, j, k;
    char*   p1;
    char*   p2;
    char*   q;


    if(nc1!=nr1)
        return false;

    for(i=0; i<nr1; i++) {
        for(j=0; j<nc2; j++) {
            q= g_strZero;
            for(k=0; k<nc1; k++) {
                q= addTerms(q, multTerms(rgMat1[index(nr1,nc1,i,k)], rgMat2[index(nr2,nc2,k,j)]));
            }
        rgMatOut[index(nr1,nc2,i,j)]= q;
        }
    }

    return true;
}


void rightPrint(int iColSize, char* str)
{
    if(str==NULL)
        return;

    int iLen= strlen(str);

    for(int i=0; i<(iColSize-iLen); i++)
        printf(" ");
    printf("%s", str);
}
 

int maxSize(int nr, int nc, char** rgM)
{
    int     iMax= 0;
    int     iCur;
    int     i, j;
    char*   p;

    for(i=0; i<nr; i++) {
        for(j=0; j<nc; j++) {
            p= rgM[index(nr,nc,i,j)];
            if(p==NULL) {
                printf("Bad entry %d %d\n", i, j);
                continue;
            }
            iCur= strlen(p);
            if(iCur>iMax)
                iMax= iCur;
        }
    }

    return iMax;
}



void PrintEntries(int nr, int nc, char** rgM)
{
    int	    i, j;
    char*   p;

    for(i=0; i<nr; i++) {
        for(j=0; j<nc; j++) {
            p= rgM[index(nr,nc,i,j)];
	    if(p==NULL)
		printf("[%02d, %02d] is NULL\n", i+1, j+1);
	    else
		printf("[%02d, %02d]: %s\n", i+1, j+1, p);
	    }
    }
}


void PrintMat(int nr, int nc, char** rgM)
{
    int     iColSize= maxSize(nr, nc, rgM)+3;
    int     i, j, k, l, m, n;

    if(2*iColSize>=g_iLineSize) {
	PrintEntries(nr, nc, rgM);
        return;
    }
    else {
        l= g_iLineSize/iColSize;            // number of columns on a line
        n= (nc+l-1)/l;
    }
    printf("Printing %d at a time\n", l);
    for(i=0; i<nr; i++) {
        for(k=0; k<n; k++) {
            m= l*k;
            for(j=0; j<l; j++) {
                if(m>(nc-1))
                    break;
                rightPrint(iColSize, rgM[index(nr,nc,i,m)]);
                m++;
            }
            printf("\n");
        }
        if(n>1)
            printf("\n");
    }
}


// --------------------------------------------------------------------- 


char* g_str16Id[256]= {
    "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
    "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
    "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
    "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
    "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
    "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
    "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0",
    "0", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0",
    "0", "0", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0",
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0",
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0",
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0",
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0",
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "0", "0",
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "0",
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1"
};


char* g_str16tau[256]= {
// 0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111
    "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 0000
    "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 0001
    "0", "0", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0",  // 0010
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0",  // 0011

    "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 0100
    "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 0101
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0",  // 0110
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "0", "0",  // 0111

    "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 1000
    "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 1001
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0",  // 1010
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "0",  // 1011

    "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 1100
    "0", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0",  // 1101
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1", "0", "0", "0", "0",  // 1110
    "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1"   // 1111
};


char* g_str16f[256]= {
// 0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111
    "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 0000
    "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 0001
    "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 0010
    "0", "0", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 0011

//  Row 0100
//      0000 0001 0010 0011 
         "0", "0", "0", "0", 
//          0100     0101     0110     0111
         "F2(0)", "F2(1)", "F2(2)", "F2(3)", 
//      1000 1001 1010 1011 1100 1101 1110 1111
         "0", "0", "0", "0", "0", "0", "0", "0",

//  Row 0101
//      0000 0001 0010 0011 
         "0", "0", "0", "0", 
//          0100     0101     0110     0111
         "F2(1)", "F2(0)", "F2(3)", "F2(2)", 
//      1000 1001 1010 1011 1100 1101 1110 1111
         "0", "0", "0", "0", "0", "0", "0", "0",

//  Row 0110
//      0000 0001 0010 0011 
         "0", "0", "0", "0", 
//          0100     0101     0110     0111
         "F2(2)", "F2(3)", "F2(0)", "F2(1)", 
//      1000 1001 1010 1011 1100 1101 1110 1111
         "0", "0", "0", "0", "0", "0", "0", "0",

//  Row 0111
//      0000 0001 0010 0011 
         "0", "0", "0", "0", 
//          0100     0101     0110     0111
         "F2(3)", "F2(2)", "F2(1)", "F2(0)", 
//      1000 1001 1010 1011 1100 1101 1110 1111
         "0", "0", "0", "0", "0", "0", "0", "0",

//  Row 1000
//      0000 0001 0010 0011 0100 0101 0110 0111 
         "0", "0", "0", "0", "0", "0", "0", "0", 
//          1000     1001     1010     1011
         "F1(0)", "F1(1)", "F1(2)", "F1(3)", 
//      1100 1101 1110 1111
         "0", "0", "0", "0",

//  Row 1001
//      0000 0001 0010 0011 0100 0101 0110 0111 
         "0", "0", "0", "0", "0", "0", "0", "0", 
//          1000     1001     1010     1011
         "F1(1)", "F1(0)", "F1(3)", "F1(2)", 
//      1100 1101 1110 1111
         "0", "0", "0", "0",

//  Row 1010
//      0000 0001 0010 0011 0100 0101 0110 0111 
         "0", "0", "0", "0", "0", "0", "0", "0", 
//          1000     1001     1010     1011
         "F1(2)", "F1(3)", "F1(0)", "F1(2)", 
//      1100 1101 1110 1111
         "0", "0", "0", "0",

//  Row 1011
//      0000 0001 0010 0011 0100 0101 0110 0111 
         "0", "0", "0", "0", "0", "0", "0", "0", 
//          1000     1001     1010     1011
         "F1(3)", "F1(2)", "F1(1)", "F1(0)", 
//      1100 1101 1110 1111
         "0", "0", "0", "0",

//  Row 1100
//      0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 
         "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
//         1100    1101    1110    1111
         "H(0)", "H(1)", "H(2)", "H(3)", 

//  Row 1001
//      0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 
         "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
//         1100    1101    1110    1111
         "H(1)", "H(0)", "H(3)", "H(2)", 

//  Row 1010
//      0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 
         "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
//         1100    1101    1110    1111
         "H(2)", "H(3)", "H(0)", "H(2)", 

//  Row 1011
//      0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 
         "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
//         1100    1101    1110    1111
         "H(3)", "H(2)", "H(1)", "H(0)", 
};


char* g_str16f1[256]= {
// 0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111
    "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 0000
    "0", "a[1]", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 0001
    "0", "0", "b[1]", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 0010
    "0", "0", "0", "c[1]", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",  // 0011

//  Row 0100
//      0000 0001 0010 0011 
         "0", "0", "0", "0", 
//          0100     0101     0110     0111
         "F2(0)", "a[1]F2(1)", "b[1]F2(2)", "c[1]F2(3)", 
//      1000 1001 1010 1011 1100 1101 1110 1111
         "0", "0", "0", "0", "0", "0", "0", "0",

//  Row 0101
//      0000 0001 0010 0011 
         "0", "0", "0", "0", 
//          0100     0101     0110     0111
         "F2(1)", "a[1]F2(0)", "b[1]F2(3)", "c[1]F2(2)", 
//      1000 1001 1010 1011 1100 1101 1110 1111
         "0", "0", "0", "0", "0", "0", "0", "0",

//  Row 0110
//      0000 0001 0010 0011 
         "0", "0", "0", "0", 
//          0100     0101     0110     0111
         "F2(2)", "a[1]F2(3)", "b[1]F2(0)", "c[1]F2(1)", 
//      1000 1001 1010 1011 1100 1101 1110 1111
         "0", "0", "0", "0", "0", "0", "0", "0",

//  Row 0111
//      0000 0001 0010 0011 
         "0", "0", "0", "0", 
//          0100     0101     0110     0111
         "F2(3)", "a[1]F2(2)", "b[1]F2(1)", "c[1]F2(0)", 
//      1000 1001 1010 1011 1100 1101 1110 1111
         "0", "0", "0", "0", "0", "0", "0", "0",

//  Row 1000
//      0000 0001 0010 0011 0100 0101 0110 0111 
         "0", "0", "0", "0", "0", "0", "0", "0", 
//          1000     1001     1010     1011
         "F1(0)", "a[1]F1(1)", "b[1]F1(2)", "c[1]F1(3)", 
//      1100 1101 1110 1111
         "0", "0", "0", "0",

//  Row 1001
//      0000 0001 0010 0011 0100 0101 0110 0111 
         "0", "0", "0", "0", "0", "0", "0", "0", 
//          1000     1001     1010     1011
         "F1(1)", "a[1]F1(0)", "b[1]F1(3)", "c[1]F1(2)", 
//      1100 1101 1110 1111
         "0", "0", "0", "0",

//  Row 1010
//      0000 0001 0010 0011 0100 0101 0110 0111 
         "0", "0", "0", "0", "0", "0", "0", "0", 
//          1000     1001     1010     1011
         "F1(2)", "a[1]F1(3)", "b[1]F1(0)", "c[1]F1(2)", 
//      1100 1101 1110 1111
         "0", "0", "0", "0",

//  Row 1011
//      0000 0001 0010 0011 0100 0101 0110 0111 
         "0", "0", "0", "0", "0", "0", "0", "0", 
//          1000     1001     1010     1011
         "F1(3)", "a[1]F1(2)", "b[1]F1(1)", "c[1]F1(0)", 
//      1100 1101 1110 1111
         "0", "0", "0", "0",

//  Row 1100
//      0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 
         "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
//         1100    1101    1110    1111
         "H(0)", "a[1]H(1)", "b[1]H(2)", "c[1]H(3)", 

//  Row 1001
//      0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 
         "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
//         1100    1101    1110    1111
         "H(1)", "a[1]H(0)", "b[1]H(3)", "c[1]H(2)", 

//  Row 1010
//      0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 
         "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
//         1100    1101    1110    1111
         "H(2)", "a[1]H(3)", "b[1]H(0)", "c[1]H(2)", 

//  Row 1011
//      0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 
         "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0",
//         1100    1101    1110    1111
         "H(3)", "a[1]H(2)", "b[1]H(1)", "c[1]H(0)"
};

char* g_str16f2[256];
char* g_str16f3[256];
char* g_str16f4[256];

char* g_str16tauf1[256];
char* g_str16tauf2[256];
char* g_str16tauf3[256];
char* g_str16tauf4[256];

char* g_str16tauf2tauf1[256];
char* g_str16tauf4tauf3[256];
char* g_str16tauf3tauf2tauf1[256];
char* g_str16tauf4tauf3tauf2tauf1[256];


char* g_strOut[256];


// --------------------------------------------------------------------- 


char* hasSquareBracket(char* p)
{
    if(p==NULL)
        return NULL;
    while(*p!='\0') {
        if(*p=='\[') {
            return p;
        }
        p++;
    }
    return NULL;
}


bool checkPrefix(char* pre, char* s)
{
    while(*pre!='\0') {
        if(*pre!=*s)
            return false;
        pre++;
        s++;
    }
    return true;
}


char* replace(char* original, char* sbracket, char* strOld, char* strNew)
// preFix[strOld*] --> preFix[strNew*]
{
    char*   pNew;
    int     iLen;
    int     ioldStringLen;
    int     inewStringLen;
    int     inewLen;
    int     ipreLen;

    if(original==NULL || sbracket==NULL || strOld==NULL || strNew==NULL)
        return NULL;
    iLen= strlen(original);
    ioldStringLen= strlen(strOld);
    inewStringLen= strlen(strNew);
    if(!(checkPrefix(strOld, sbracket+1)))
        return original;
    inewLen= iLen+(inewStringLen-ioldStringLen);
    pNew= getString(inewLen+1);
    if(pNew==NULL)
        return NULL;
    ipreLen= (sbracket-original+1);
    strncat(pNew, original, ipreLen);
    strcat(pNew+ipreLen, strNew);
    strcat(pNew+ipreLen+inewStringLen, sbracket+1+ioldStringLen);
    *(pNew+inewLen)= '\0';
    return pNew;
}


bool copyandFix(char** rgM1, char** rgM2, char* strOld, char* strNew, int nr, int nc)
// F[old*] --> F[new*];
// eg: copyandFix(g_str16f1, g_str16f2, "1", "2", 16, 16);
{
    int     i, j;
    char*   s1;
    char*   s2;
    char*   sbracket;

    for(i=0; i<nr; i++) {
        for(j=0; j<nc; j++) {
            s1= rgM1[index(nr, nc, i, j)];
            sbracket= hasSquareBracket(s1);
            if(sbracket!=NULL) {
                s2= replace(s1, sbracket, strOld, strNew);
                if(s2==NULL)
                    return false;
                rgM2[index(nr, nc, i, j)]= s2;
            }
            else {
                rgM2[index(nr, nc, i, j)]= s1;
            }
        }
    }

    return true;
}


int main(int an, char** av) 

{
    int             iRow, iCol;
    int             i, j;
    int             nVar;
    int             iSize;

    nVar= 4;  // test value
    iSize= 1<<nVar;

    for(i=1; i<an; i++) {
	if(strcmp(av[i], "-LineSize")==0 && i<(an-1))
	    sscanf(av[i+1],"%d", &g_iLineSize);
    }
    
    if(!copyandFix( g_str16f1, g_str16f2, "1", "2", 16, 16)) {
        printf("cant copyandFix f2\n");
        return 1;
    }
    if(!copyandFix( g_str16f1, g_str16f3, "1", "3", 16, 16)) {
        printf("cant copyandFix f3\n");
        return 1;
    }
    if(!copyandFix( g_str16f1, g_str16f4, "1", "4", 16, 16)) {
        printf("cant copyandFix f4\n");
        return 1;
    }


    printf("Symbolic multiplication of %d x %d correlation matrices\n", iSize, iSize);

    printf("f\n\n");
    PrintMat(16, 16, g_str16f);
    printf("\n\n");

    // tau f1
    if(!SymMultMat(iSize, iSize, iSize, iSize, g_str16tau, g_str16f1, g_str16tauf1)) {
        printf("Cant multiply matrices\n");
        return 1;
    }
    printf("tau\n\n");
    PrintMat(16, 16, g_str16tau);
    printf("\ntimes f1\n\n");
    PrintMat(16, 16, g_str16f1);
    printf("\nis\n\n");
    PrintMat(16, 16, g_str16tauf1);
    printf("\n\n");

    // tau f2
    if(!SymMultMat(iSize, iSize, iSize, iSize, g_str16tau, g_str16f2, g_str16tauf2)) {
        printf("Cant multiply matrices\n");
        return 1;
    }
    printf("tau\n\n");
    PrintMat(16, 16, g_str16tau);
    printf("\ntimes f2\n\n");
    PrintMat(16, 16, g_str16f2);
    printf("\nis\n\n");
    PrintMat(16, 16, g_str16tauf2);
    printf("\n\n");

    // tau f2 tau f1
    if(!SymMultMat(iSize, iSize, iSize, iSize, g_str16tauf2, g_str16tauf1, g_str16tauf2tauf1)) {
        printf("Cant multiply matrices\n");
        return 1;
    }
    printf("tau f2 tau f1\n\n");
    PrintMat(16, 16, g_str16tauf2tauf1);
    printf("\n\n");

    // tau f3
    if(!SymMultMat(iSize, iSize, iSize, iSize, g_str16tau, g_str16f3, g_str16tauf3)) {
        printf("Cant multiply matrices\n");
        return 1;
    }
    printf("tau\n\n");
    PrintMat(16, 16, g_str16tau);
    printf("\ntimes f3\n\n");
    PrintMat(16, 16, g_str16f3);
    printf("\nis\n\n");
    PrintMat(16, 16, g_str16tauf3);
    printf("\n\n");

    // tau f4
    if(!SymMultMat(iSize, iSize, iSize, iSize, g_str16tau, g_str16f4, g_str16tauf4)) {
        printf("Cant multiply matrices\n");
        return 1;
    }
    printf("tau\n\n");
    PrintMat(16, 16, g_str16tau);
    printf("\ntimes f4\n\n");
    PrintMat(16, 16, g_str16f4);
    printf("\nis\n\n");
    PrintMat(16, 16, g_str16tauf4);
    printf("\n\n");

    // tau f4 tau f3
    if(!SymMultMat(iSize, iSize, iSize, iSize, g_str16tauf4, g_str16tauf3, g_str16tauf4tauf3)) {
        printf("Cant multiply matrices\n");
        return 1;
    }
    printf("tauf4 tauf3\n\n");
    PrintMat(16, 16, g_str16tauf4tauf3);

    // tau f4 tau f3 tau f2 tau f1
// #define TEST1
#ifndef TEST1
    if(!SymMultMat(iSize, iSize, iSize, iSize, g_str16tauf4tauf3, g_str16tauf2tauf1, 
                   g_str16tauf4tauf3tauf2tauf1)) {
        printf("Cant multiply matrices\n");
        return 1;
    }
    printf("tauf4 tauf3 tauf2 tauf1\n\n");
    PrintMat(16, 16, g_str16tauf4tauf3tauf2tauf1);
#else
    int   t= 7;
    char* p; char* q;
    printf("\n\nPosition  tf4tf3 (Row %d)   tf2tf1 (Col %d)\n", t, t);
    for(i=0; i<iSize; i++) {
	p= g_str16tauf4tauf3[index(iSize, iSize, t-1, i)];
	q= g_str16tauf2tauf1[index(iSize, iSize, i, t-1)];
	printf("%2d         %s       %s\n", i+1, p, q);
    }
#endif

    return 0;
}


// ---------------------------------------------------------------------------


