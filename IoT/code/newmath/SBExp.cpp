#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <string.h>
#include <io.h>
#include <unistd.h>


// SBExp.cpp
//      Experiments with AES S-box
//      John Manferdelli



extern int strcmp(const char*, const char*);
#ifndef byte
typedef unsigned char byte;
#endif


// -------------------------------------------------------------


//  Utility functions

bool ToBits(int n, int iNumBits, short unsigned int* siBits)
{
    int i;

    if(iNumBits>31)
        return false;

    for(i=0; i<iNumBits; i++) {
        siBits[i]= n&1;
        n>>= 1;
    }

    return true;
}


void printMatrix1(int n, short unsigned int rgMatrix[])
{
    int                 j, k;
    int                 iNumBits;
    short unsigned int  siBits[8];

    for(j=0; j<8;j++) {
        if(((n>>j)&1)!=0) {
            iNumBits= j;
            break;
        }
    }

    for(j=0;j<n;j++) {
        ToBits(j, 8, siBits);
        for(k=(iNumBits-1); k>=0; k--) {
            printf("%d", siBits[k]);
        }
        printf(" ");
    }
    printf("\n");
    for(j=0;j<n;j++)
        printf("   %d ", rgMatrix[j]);
    printf("\n");
}


void printBinMatrix(int n, short unsigned int rgMatrix[])
{
    int j;

    for(j=0;j<16;j++)
        printf(" %03x",j);
    printf("\n");
    printf("-----------------------------------------------------------------------\n");
    for(j=0;j<n;j++) {
        printf("   %d", rgMatrix[j]);
        if((j%16)==15 && j<(n-1))
            printf(" |  %03x\n",j);
    }
    printf(" |  %03x\n", j-1);
}


void printIntegerDataMatrix(int n, int rgMatrix[])
{
    int j;

    for(j=0;j<n;j++) {
        printf("%4d ", rgMatrix[j]);
        if((j%8)==7)
            printf("\n");
    }
    if((j%8)!=7)
        printf("\n");
}


void printDataMatrix(int n, double rgMatrix[])
{
    int j;

    for(j=0;j<n;j++)
        printf("%5.2f ", rgMatrix[j]);
    printf("\n");
}



// -------------------------------------------------------------


//  Cycle Structure


void printCycleStructure(int iSize, short unsigned int* rgP)
{
    byte*   rgBSeen= (byte*) malloc(iSize);
    int*    rgiNumCycles= (int*) malloc(iSize*sizeof(int));
    int     i, j, k;
    int     iLowest= 0;
    int     iNumCycles= 0;
    int     iCurcycLen;

    if( rgBSeen==NULL || rgiNumCycles==NULL) {
        printf("Can't allocate permutation arrays\n");
        goto Done;
    }
    for(i=0; i<iSize; i++) {
        rgBSeen[i]= 0;
        rgiNumCycles[i]= 0;
    }
    
    while(iLowest<iSize) {
        i= iLowest;
        while(rgBSeen[i]!=0 && i<iSize) {
            i++;
        }
        iLowest= i;
        if(i>=iSize)
            break;
        printf(" (  ");
        iCurcycLen= 0;
        for(;;) {
            printf("%04x ", i);
            rgBSeen[i]= 1;
            i= rgP[i];
            iCurcycLen++;
            if(rgBSeen[i]!=0)
                break;
            if((iCurcycLen%16)==0)
                printf("\n    ");
        }
        printf(")\n");
        iNumCycles++;
        rgiNumCycles[iCurcycLen]++;
    }


    printf("\n%d cycles. Length(Number): ", iNumCycles);
    for(i=0; i<iSize; i++) {
        if(rgiNumCycles[i]>0)
            printf("%d(%d) ", i, rgiNumCycles[i]);
    }


Done:
    if(rgBSeen!=NULL)
        free(rgBSeen);
    if(rgiNumCycles!=NULL)
        free(rgiNumCycles);
}


// -------------------------------------------------------------


//      Polynomial synthesis and evaluation


void printTerm(int nVars, short unsigned int term, char* strBuf= NULL)
// strBuf must already be a null terminated string if not null
{
    int     j;
    bool    fNone= true;
    int     iLen;

    if(strBuf==NULL) {
        for(j=0;j<nVars; j++) {
            if(term&1) {
                printf("%d", nVars-j);
                fNone= false;
                }
            term>>= 1;
            }
        if(fNone==true)
            printf("c");
    }
    else {
        for(j=0;j<nVars; j++) {
            if(term&1) {
                iLen= strlen(strBuf);
                sprintf(&strBuf[iLen],"%d", nVars-j);
                fNone= false;
                }
            term>>= 1;
            }
        if(fNone==true) {
            iLen= strlen(strBuf);
            sprintf(&strBuf[iLen],"%s", "c");
            }
    }
}


void printPoly(int nVars, int nt, short unsigned int rgT[])
{
    int j;

    if(nt==0) {
        printf("0\n");
        return;
        }
    for(j=0;j<nt;j++) {
        printTerm(nVars, rgT[j]);
        if(j!=(nt-1))
            printf("+");
        }
    printf("\n");
}


short unsigned int processTerm(char* pBeg, char* pEnd, int maxVar)
// convert text term to internal representation
{
    char                 *p;
    short unsigned int   siTerm= 0;
    int                  iCurTerm;

    for(p=pBeg; p<=pEnd; p++) {
        if(*p<'0' || *p>'9')
            continue;
        iCurTerm= (int)(*p-'0');
        if(iCurTerm>maxVar)
            continue;
        if(iCurTerm==0)
            return 0;
        iCurTerm= maxVar+1-iCurTerm;
        siTerm|= 1<<(iCurTerm-1);
    }

    return siTerm;
}


bool scanPoly(char* szPoly, int nVar, int* pinTerms, 
              short unsigned int**  ppiPolyMat)
// conver text polynomial to internal representation
// constant term is **iPolyMat[0]

{
#if 0
    char*       p= szPoly;
    char*       qBeg= NULL;
    short unsigned int*  rgsiPoly= NULL;  // FIX  g_siTerms;
    short unsigned int   siTerm= 0;

#ifdef DEBUG2
    printf("scanPoly %s\n", szPoly);
#endif
    *ppiPolyMat= g_siTerms;
    *pinTerms= 1;
    rgsiPoly[0]= 0;

    while(*p!='\0') {
        if((*p>='0') && (*p<='9')) {
            if(qBeg==NULL)
                qBeg= p;
        }
        else { 
            if(qBeg!=NULL) {
                siTerm= processTerm(qBeg, p-1, nVar);
                if(siTerm==0)
                    rgsiPoly[0]^= 1;
                else
                    g_siTerms[(*pinTerms)++]= siTerm;
                qBeg= NULL;
            }
        }
        p++;
    }

    if(qBeg!=NULL) {
        siTerm= processTerm(qBeg, p-1, nVar);
        if(siTerm==0)
            rgsiPoly[0]^= 1;
        else
            g_siTerms[(*pinTerms)++]= siTerm;
        qBeg= NULL;
        }
#endif
    return true;
}


short unsigned int evalTerm(int n, short unsigned int* rgiVals, 
                            short unsigned int* rgiTerm)
{
    short unsigned int   a= 1;
    int                  i;
    bool                 fNoMult= true;

    for(i=0; i<n; i++) {
        if(rgiTerm[i]!=0) {
            a*= rgiVals[i];
            fNoMult= false;
        }
    }
    return fNoMult?0:a;
}


bool PolyToFunction(int nVar, int nTerms, short unsigned int* rgsiTerms, 
                    short unsigned int* rgsiFunc)
{
    int                  i, j;
    short unsigned int   c= rgsiTerms[0];
    int                  iTerm;
    short unsigned int   a;
    short unsigned int   rgiBits[32];
    short unsigned int   rgtBits[32];
    int                  iSize= 1<<nVar;

#ifdef DEBUG3
    printf("PolytoFunction %d variables, %d terms: ", nVar, nTerms);
    printMatrix(nTerms, rgsiTerms);
#endif
    for(i=0; i<iSize; i++) {
        ToBits(i, nVar, rgiBits);
        a= 0;
        for(j=1; j<nTerms; j++) {
            iTerm= rgsiTerms[j];
            ToBits(iTerm, nVar, rgtBits);
            a^= evalTerm(nVar, rgiBits, rgtBits);
        }
        rgsiFunc[i]= a^c;
    }
    return true;
}


// Calculate Polynomial representation
//      See Sloane and MacWilliams


int subset(int nVars, int a, int b)

{
    int i;

    for(i=0;i<nVars;i++) {
        if((a&1)==1)
            if((b&1)==0)
                return(0);
        a>>= 1;
        b>>= 1;
        }
    return(1);
}


bool FunctionToPoly(int nVars, int* pnTerms, short unsigned int* rgF, 
                    short unsigned int* rgT)

{
    int                  i, j;
    int                  of, sum;
    int                  iSize= 1<<nVars;
    short unsigned int   term;

    *pnTerms= 0;
    for(i=0; i<iSize; i++) {
        sum= 0;
        for(j=0; j<iSize; j++) {
            if(subset(nVars, j, i)==1)
                sum+= rgF[j];
            }
        term= sum%2;
        if(term!=0) {
                rgT[*pnTerms]= (short unsigned int) i;
                (*pnTerms)++;
            }
        }
    return true;
}


// -----------------------------------------------------------


//      Walsh Transform


short unsigned int evalDot(int n, short unsigned int* rgiVal1, 
                           short unsigned int* rgiVal2)
{
    short unsigned int   a= 0;
    int                  i;

    for(i=0; i<n; i++) 
        a^= rgiVal1[i]&rgiVal2[i];
    return a;
}


double FValue(int nVar, int nArg, short unsigned int* rgiFunc)
{
    int                  i;
    short unsigned int   siArgBits[16];
    short unsigned int   siIBits[16];
    double               x= 0.0;
    int                  iSize= 1<<nVar;
    short unsigned int   a;

    // f(w)= sum (-1)**(wa + f(a))
    if(nVar>16)
        return 0;
    ToBits(nArg, nVar, siArgBits);   // nArg is w
    if(rgiFunc==NULL) {
        for(i=0; i<iSize; i++) {
            ToBits(i, nVar, siIBits);
            a= evalDot(nVar, siIBits, siArgBits);
            x+= (double) (1-2*a);
            }
    }
    else {
        for(i=0; i<iSize; i++) {
            ToBits(i, nVar, siIBits);
            a= evalDot(nVar, siIBits, siArgBits)^rgiFunc[i];
            x+= (double) (1-2*a);
            }
    }

#ifdef DEBUG1
    printf("FValue(%02d, %02d, %8.4f)\n", nVar, nArg, x);
#endif
    return x/((double) iSize);
}


void CalculateSpectrum(int nVar, short unsigned int* rgiFunc,
                       double* rgxSpec)
{
    int     i;
    int     iSize= (1<<nVar);

    for(i=0; i<iSize; i++)
        rgxSpec[i]= FValue(nVar, i, rgiFunc);
}


void BooleanReport(char* strLabel, int nVar, short unsigned int* rgF,
                   short unsigned int* rgPoly, double* rgxSpec, int* pnTerms,
                   bool fPrintFunc=true, bool fPrintSpec=false)
{
    int iSize= 1<<nVar;

    *pnTerms= 0;
    if(fPrintFunc) {
        printf("\n\n%s function:\n", strLabel);
        printBinMatrix(iSize, rgF);
    }
    else {
        printf(" %s ", strLabel);
    }
    *pnTerms= 0;
    FunctionToPoly(nVar, pnTerms, rgF, rgPoly);
    printf("%d polynomial terms:\n", *pnTerms);
    printPoly(nVar, *pnTerms, rgPoly);
    CalculateSpectrum(nVar, rgF, rgxSpec);
    if(fPrintSpec) {
        printf("\nSpectrum:\n");
        printDataMatrix(iSize, rgxSpec);
        printf("\n");
    }
}


inline bool QualifiedKeyTerm(short unsigned int t, int nKeyTerms, 
                             short unsigned int* rgKeyTerms)
{
    int i;

    for(i=0; i<nKeyTerms; i++) {
        if(t==rgKeyTerms[i])
            return true;
    }
    return false;
}


// -----------------------------------------------------------


//  Key Constraints

int g_iW[16]= {
 // 0000 0001 0010 0011 0100 0101 0110 0111
       0,   1,   1,   2,   1,   2,   2,   3,
 // 1000 1001 1010 1011 1100 1101 1110 1111
       1,   2,   2,   3,   2,   3,   3,   4
    };
int getWeight(short unsigned int su)
{
    unsigned u= (unsigned) su;
    return g_iW[u&0xf] + g_iW[(u>>4)&0xf] +
           g_iW[(u>>8)&0xf] + g_iW[(u>>12)&0xf];
}

void Exch(short unsigned int* pu1, short unsigned int* pu2)
{
    short unsigned int u;

    u= *pu1;
    *pu1= *pu2;
    *pu2= u;
}


void termReorder(int nVars, int iSize, short unsigned int* rgPolyKeyMatrixReorder)
{
    int     i, j, k;
    int     iLastSorted= iSize-1;

    for(i=0; i<iSize;i++)
        rgPolyKeyMatrixReorder[i]= (short unsigned int) i;

    // move constant and linear terms to the end
    Exch(&rgPolyKeyMatrixReorder[0], &rgPolyKeyMatrixReorder[iLastSorted]);
    iLastSorted--;
    for(i=0;i<iLastSorted;i++) {
        if(getWeight(rgPolyKeyMatrixReorder[i])==1) {
            Exch(&rgPolyKeyMatrixReorder[i], &rgPolyKeyMatrixReorder[iLastSorted]);
            iLastSorted--;
        }
    }

    // move the remaining low weights first
    for(j=nVars; j>2; j--) {
        for(i=0; i<iLastSorted; i++) {
            while(iLastSorted>i) {
                k= getWeight(rgPolyKeyMatrixReorder[iLastSorted]);
                if(k==j)
                    iLastSorted--;
                else
                    break;
            }
            if(getWeight(rgPolyKeyMatrixReorder[i])>=j) {
                Exch(&rgPolyKeyMatrixReorder[i], &rgPolyKeyMatrixReorder[iLastSorted]);
                iLastSorted--;
            }
        }
    }
}


void  AddRowTally(int iSize, int iRowGets, int iRowPuts, short unsigned int* rguRowSums)
{
    int i, j, m, n;

#ifdef DEBUG4
    printf("** AddRowTally, Gets: %d, Puts: %d\n", iRowGets,iRowPuts);
#endif
    m= iRowGets*iSize;
    n= iRowPuts*iSize;

    for(i=0;i<iSize;i++) {
        rguRowSums[m+i]^= rguRowSums[n+i];
    }
}


void AddToRow(int sn, short unsigned int* rgbDest, short unsigned int* rgbToBeAdded)
{
    int i;

#ifdef DEBUG4
    printf("** AddToRow %d\n", sn);
#endif
    for(i=0; i<sn; i++) 
        rgbDest[i]^= rgbToBeAdded[i];
}


void printConstrainMatrix(int nVars, int iSize, 
                          short unsigned int* rgPolyKeyMatrixReorder,
                          short unsigned int* rgCoeffMatrix, int* rgiRearrange=NULL)
{
    int     i, j, k, m, n, r;
    char    rgbString[32];

    printf("Column reorder:\n");
    for(i=0;i<iSize;i++) {
        printTerm(nVars, rgPolyKeyMatrixReorder[i]); 
        if((i%16)==15)
            printf(",\n");
        else
            printf(", ");
    }
    printf("\nMatrix:\n");
    for(i=0;i<iSize;i++) {
        rgbString[0]= 0;
        if(rgiRearrange==NULL)
            r= i;
        else
            r= rgiRearrange[i];
        printTerm(nVars, (short unsigned int) r, rgbString); 
        k= strlen(rgbString);
        printf("%s", rgbString);
        while(k++<8)
            printf(" ");
        printf(":  ");
        m= r*iSize;
        for(j=0;j<iSize;j++) {
            n= rgPolyKeyMatrixReorder[j]; 
            printf("%1d", rgCoeffMatrix[m+n]);
            if((j%64)==63)
                printf("\n           ");
        }
        printf("\n");
    }
}


void fillRelationMatrix(int nVars, int iSize, short unsigned int uTerm, 
                        short unsigned int* rgPolyKeyMatrixReorder,
                        int nTerms,
                        short unsigned int* rgPolyCoeffs,
                        short unsigned int* rgCoeffMatrix)

//  Rows indexed by plaintext variables
//  Columns indexed by key variables (reordered)
{
    int     t, rt;
    int     i;

    rt= ((int) uTerm)*iSize;    // fill this row
    for(i=0; i<nTerms;i++) {
        t= rgPolyKeyMatrixReorder[rgPolyCoeffs[i]];
        rgCoeffMatrix[rt+t]= 1;
    }
}


#define DEBUG4


void findKeyConstraints(int nVars, int iSize, short unsigned int* rgPolyKeyMatrixReorder,
                        short unsigned int* rgCoeffMatrix)
{
    int                 i, j, k, m, n;
    short unsigned int* rguRowSums= 
                    (short unsigned int*) malloc(iSize*iSize*sizeof(short unsigned int));
    int*                rgiRearrange= (int*) malloc(iSize*sizeof(int));

    printf("Initial Relations matrix:\n");
    printConstrainMatrix(nVars, iSize, rgPolyKeyMatrixReorder, rgCoeffMatrix);
    printf("\n");

    if(rguRowSums==NULL || rgiRearrange==NULL) {
        printf("Can't allocate RowSums\n");
        return;
    }

    for(i=0; i<iSize; i++) {
        rgiRearrange[i]= i;
        for(j=0; j<iSize; j++) {
            n= i*iSize+j;
            if(i==j)
                rguRowSums[n]= 1;
            else
                rguRowSums[n]= 0;
        }
    }

    // goal is to eliminate all but the linear terms

    int                 iCurCol= 0;
    int                 iCurRow= 0;
    int                 iLastNL= iSize-nVars;
    int                 iRowGets;
    int                 iRowPuts;

    for(iCurCol=0; iCurCol<iLastNL; iCurCol++) {
        // Find 1 if current column in current row or below
        k= rgPolyKeyMatrixReorder[iCurCol];
        for(i=iCurRow; i<iSize; i++) {
            j= rgiRearrange[i];
            n= iSize*j;
            if(rgCoeffMatrix[n+k]==1) {
                break;
            }
        }
        if(i>=iSize || rgCoeffMatrix[n+k]==0)
            continue;
        // 1 was found rearrange, so make this current row
        if(i!=iCurRow) {
            rgiRearrange[i]= rgiRearrange[iCurRow];
            rgiRearrange[iCurRow]= j;
        }
        // Add current row to any row with a 1 in this column
        iRowPuts= rgiRearrange[iCurRow];
        for(i=(iCurRow+1); i<iSize; i++) {
            m= iSize*rgiRearrange[i];
            if(rgCoeffMatrix[m+k]==1) {
                iRowGets= rgiRearrange[i];
                // Add iCurRow to this row
                AddToRow(iSize, &rgCoeffMatrix[iSize*iRowGets], 
                                &rgCoeffMatrix[iSize*iRowPuts]);
                // Fix Rowsums
                AddRowTally(iSize, iRowGets, iRowPuts, rguRowSums);
            }
        }
        iCurRow++;
    }

    // Report
    printf("\nSolved Relations matrix:\n");
    printConstrainMatrix(nVars, iSize, rgPolyKeyMatrixReorder, 
                         rgCoeffMatrix, rgiRearrange);
    printf("\n");

    // print rowsums
    printf("From  %d down\n", iLastNL+1);
    for(i=iLastNL+1;i<iSize;i++) {
        j= rgiRearrange[i];
        m= j*iSize;
        printf("\nPlaintext coefficients row ");
        printTerm(nVars, (short unsigned int) j, NULL);
        printf(" : ");
        n= 0;
        for(k=0; k<iSize; k++) {
            if(rguRowSums[m+k]!=0) {
                n++;
                printTerm(nVars, (short unsigned int) k, NULL);
                    printf(", ");
                if((n%8)==7) 
                    printf("\n        ");
            }
        }
        printf("\n");
    }

    // Clean up
    if(rguRowSums!=NULL) {
        free(rguRowSums);
    rguRowSums= NULL;
    }
    if(rgiRearrange!=NULL) {
        free(rgiRearrange);
    rgiRearrange= NULL;
    }
}


// -----------------------------------------------------------


int main(int an, char** av) 

{
    //        number of variables in poly
    int                     nVar= 8;             
    //        bijective fuction
    short unsigned int*     rgBox= NULL;

    bool                    fReadFile= false;
    bool                    fSubProd= false;
    bool                    fComponents= false;
    bool                    fCycle= false;
    bool                    fKeyCoeffs= false;
    bool                    fPreKeyCoeffs= false;
    bool                    fPostKeyCoeffs= false;
    bool                    fAllKeyTerms= true;
    bool                    fKeyConstraints= false;

    int                     i, j, k, n;
    char*                   szInFile= NULL;
    int                     iIn;
    int                     iSize;
    int                     nTerms;
    short unsigned int      uProdMask= 0xff;
    short unsigned int      uKey;
    short unsigned int      uValue;
    short unsigned int      t;
    int                     nKeyTerms= 0;


    iSize= 1<<nVar;

    // Arrays for calculations
    short unsigned int* rgFProjections= 
                    (short unsigned int*) malloc(nVar*iSize*sizeof(short unsigned int));
    double*             rgxSpec= 
                    (double*) malloc(iSize*sizeof(double));
    short unsigned int* rgPolyCoeffs= 
                    (short unsigned int*) malloc(iSize*sizeof(short unsigned int));
    short unsigned int* rgFProd=
                    (short unsigned int*) malloc(iSize*sizeof(short unsigned int));
    short unsigned int  uMask;
    char                rgLabel[128];
    int                 iShift;
    short unsigned int* rgPolyCoeffsPost= 
                    (short unsigned int*) malloc(iSize*iSize*sizeof(short unsigned int));
    short unsigned int* rgPolyCoeffsPre= 
                    (short unsigned int*) malloc(iSize*iSize*sizeof(short unsigned int));
    short unsigned int* rgKeyTerms= 
                    (short unsigned int*) malloc(iSize*sizeof(short unsigned int));
    short unsigned int* rgPolyPreKeyMatrix= 
                    (short unsigned int*) malloc(iSize*iSize*sizeof(short unsigned int));
    short unsigned int* rgPolyPostKeyMatrix= 
                    (short unsigned int*) malloc(iSize*iSize*sizeof(short unsigned int));
    short unsigned int* rgPolyKeyMatrixReorder= 
                    (short unsigned int*) malloc(iSize*iSize*sizeof(short unsigned int));

    // Process arguments
    for(i=0;i<an; i++) {
        if(strcmp(av[i], "-KeyCoefficients")==0) {
            fKeyCoeffs= true;
            fPreKeyCoeffs= true;
            fPostKeyCoeffs= true;
        }
        if(strcmp(av[i], "-Components")==0) {
            fComponents= true;
        }
        if(strcmp(av[i], "-KeyConstraints")==0) {
            fKeyConstraints= true;
        }
        if(strcmp(av[i], "-Cycles")==0) {
            fCycle= true;
        }
        if(strcmp(av[i], "-In")==0) {
            i++;
            if(i<an) {
                fReadFile= true;
                szInFile= av[i];
            }
        }
        if(strcmp(av[i], "-SubProd")==0) {
            i++;
            if(i<an) {
                fSubProd= true;
                sscanf(av[i], "%x", &n);
                uProdMask= (short unsigned int) n;
            }
        }
        if(strcmp(av[i], "-PostKeyCoefficients")==0) {
            i++;
            if(i<an) {
                fPostKeyCoeffs= true;
                sscanf(av[i], "%x", &n);
                uProdMask= (short unsigned int) n;
            }
        }
        if(strcmp(av[i], "-PreKeyCoefficients")==0) {
            i++;
            if(i<an) {
                fPreKeyCoeffs= true;
                sscanf(av[i], "%x", &n);
                uProdMask= (short unsigned int) n;
            }
        }
        if(strcmp(av[i], "-KeyTerms")==0) {
            if(i<an) {
                fAllKeyTerms= false;
                i++;
                while(i<an) {
                    sscanf(av[i], "%x", &n);
                    rgKeyTerms[nKeyTerms++]= (short unsigned int) n;
                    i++;
                }
            printf("%d post key terms\n", nKeyTerms);
            }
        }
    }

    if(an<2) {
        printf("SBExp -In filename [-Cycles] [-Components] [-SubProd mask] [-KeyCoefficients] [-PreKeyCoefficients mask] [-PostKeyCoefficients mask] -KeyConstraints\n");
        return 1;
    }
    printf("\nAnalysis of AES S-Box, Product mask: %02x\n", uProdMask);


    // Read S-Box
    iIn= open(szInFile, O_RDONLY);
    if(iIn<0) {
        printf("Cant open: %s\n", szInFile);
        return 1;
        }

    // Print it
    rgBox= (short unsigned int *) malloc(iSize*sizeof(short unsigned int));
    if(rgBox==NULL) {
        printf("Cant allocate rgBox[%d]\n", iSize);
        goto Done;
        }
    read(iIn, rgBox, iSize*sizeof(short unsigned int));
    close(iIn);

    printf("\n\n");
    for(i=0; i<16; i++)
        printf("%02x ", i);
    printf("\n");
    printf("-----------------------------------------------------\n");
    for(i=0; i<16; i++) {
        for(j=0;j<16;j++) {
            printf("%02x ", rgBox[16*i+j]);
        }
        printf(" | %01xf\n", i);
    }

    // Compute cycle structure
    if(fCycle) {
        printf("\n\nCycle Structure:\n");
        printCycleStructure(iSize, rgBox);
    }

    if(rgFProjections==NULL || rgxSpec==NULL || rgPolyCoeffs==NULL || rgFProd==NULL ||
            rgPolyCoeffsPost==NULL || rgPolyCoeffsPre==NULL || rgKeyTerms==NULL ||
            rgPolyPostKeyMatrix==NULL || rgPolyPreKeyMatrix==NULL || 
            rgPolyKeyMatrixReorder==NULL) {
        printf("Can't allocate data structures\n");
        goto Done;
    }
    
    if(fComponents || fSubProd) {
        for(i=0; i<nVar; i++) {
            n= iSize*i;
            // iShift= nVar-i-1;
            uMask= 1<<i;
            for(j=0; j<iSize; j++) {
                if((rgBox[j]&uMask)!=0)
                    rgFProjections[n+j]= 1;
                else
                    rgFProjections[n+j]= 0;
            }
        }
    }

    // For each function component:
    //      1. Write as polynomial
    //      2. Calculate spectrum
    if(fComponents) {
        printf("\n\nComponent functions:\n");
        for(i=0; i<nVar; i++) {
            sprintf(rgLabel, "Projection %d", i);
            BooleanReport(rgLabel, nVar, &rgFProjections[iSize*i], rgPolyCoeffs, rgxSpec, &nTerms);
        }
    
        // For the product of the component functions:
        //      1. Write as polynomial
        //      2. Calculate spectrum
        for(i=0; i<iSize; i++) {
            rgFProd[i]= 1;
            for(j=0; j<nVar; j++) {
                n= iSize*j;
                rgFProd[i]*= rgFProjections[n+i];
            }
        }
        BooleanReport("Product", nVar, rgFProd, rgPolyCoeffs, rgxSpec, &nTerms);
    }
    
    if(fSubProd) {
        for(i=0; i<iSize; i++) {
            rgFProd[i]= 1;
            for(j=0; j<nVar; j++) {
                if((uProdMask&(1<<j))!=0) {
                    n= iSize*j;
                    rgFProd[i]*= rgFProjections[n+i];
                }
            }
        }
        sprintf(rgLabel, "Subproduct of %02x mask projections", uProdMask);
        BooleanReport(rgLabel, nVar, rgFProd, rgPolyCoeffs, rgxSpec, &nTerms);
    }

    //  For each key:
    //      1. Calculate g(k,x)= Prod(i) (f_i(x)+k)
    //      2. Calculate polynomial and spectrum
    //      3. Store polynomial coefficients for each k
    //      4. Use stored coefficients to get polynomial representation and spectrum
    //         of each coefficient (i.e., the coefficients of the x terms in 
    //         g(k,x)= Sum(e) a(e)x^e
    
    //  Set up term reorder for constraints
    if(fKeyConstraints) {
        termReorder(nVar, iSize, rgPolyKeyMatrixReorder);
        for(i=0; i<iSize*iSize;i++) {
            rgPolyPreKeyMatrix[i]= 0;
            rgPolyPostKeyMatrix[i]= 0;
        }
    }

    // Calculate key term coefficients
    if(fPostKeyCoeffs) {
        printf("\n\nProfile with post-substitution key addition\n");
        for(i=0;i<iSize*iSize;i++) 
            rgPolyCoeffsPost[i]= 0;
        for(uKey=0; uKey<(short unsigned int) iSize; uKey++) {
            for(i=0; i<iSize; i++) {
                uValue= (rgBox[i]+uKey)&0xff;
                t= 1;
                for(j=0; j<nVar; j++) {
                    if((uProdMask&(1<<j))!=0) {
                        t*= uValue&1;
                    }
                    uValue>>= 1;
                }
                rgFProd[i]= t; 
            }
            // now get the polynomial representation
            nTerms= 0;
            FunctionToPoly(nVar, &nTerms, rgFProd, rgPolyCoeffs);
            // fill the coefficient matrix
            n= ((int) uKey)*iSize;
            for(j=0; j<nTerms;j++) {
                k= n+(int)rgPolyCoeffs[j];
                rgPolyCoeffsPost[k]= 1;
            }
        }
    
        // now go down the columns an calculate the key coefficients as polynomials
        for(i=0; i<iSize; i++) {    // i indexes terms
            if(!fAllKeyTerms && 
               !QualifiedKeyTerm((short unsigned int) i, nKeyTerms, rgKeyTerms))
                continue;
            n= i;
            for(j=0;j<iSize;j++) {
                rgFProd[j]= rgPolyCoeffsPost[n];
                n+= iSize;
            }
            sprintf(rgLabel,"\nTerm ",NULL);
            printTerm(nVar, (short unsigned int) i, rgLabel);
            k= strlen(rgLabel);
            sprintf(&rgLabel[k], ", key bit polynomial coefficient, ", NULL);
            BooleanReport(rgLabel, nVar, rgFProd, rgPolyCoeffs, rgxSpec, 
                          &nTerms, false, false);
            if(fPostKeyCoeffs) {
                fillRelationMatrix(nVar, iSize, (short unsigned int) i, 
                                   rgPolyKeyMatrixReorder,
                                   nTerms, rgPolyCoeffs, rgPolyPostKeyMatrix);
            }
        }
    }

    //  Do the same as above for
    //      g(k,x)= Prod(i) f_i(x+k)
    if(fPreKeyCoeffs) {
        printf("\n\nProfile with pre-substitution key addition\n");
        for(i=0;i<iSize*iSize;i++) 
            rgPolyCoeffsPre[i]= 0;

        for(uKey=0; uKey<(short unsigned int) iSize; uKey++) {
            for(i=0; i<iSize; i++) {
                k= (i+((int)uKey))&0xff;
                uValue= rgBox[k];
                t= 1;
                for(j=0; j<nVar; j++) {
                    if((uProdMask&(1<<j))!=0) {
                        t*= uValue&1;
                    }
                    uValue>>= 1;
                }
                rgFProd[i]= t; 
            }
#ifdef DEBUG
            printf("Debug Print, uKey: %02x\n", (int)uKey);
            BooleanReport("DEBUG", nVar, rgFProd, rgPolyCoeffs, rgxSpec, &nTerms);
            continue;
#endif
            // now get the polynomial representation
            nTerms= 0;
            FunctionToPoly(nVar, &nTerms, rgFProd, rgPolyCoeffs);
            // fill the coefficient matrix
            n= ((int) uKey)*iSize;
            for(j=0; j<nTerms;j++) {
                k= n+(int)rgPolyCoeffs[j];
                rgPolyCoeffsPre[k]= 1;
            }
        }
    
        // now go down the columns an calculate the key coefficients as polynomials
        for(i=0; i<iSize; i++) {    // i indexes terms
            if(!fAllKeyTerms && 
               !QualifiedKeyTerm((short unsigned int) i, nKeyTerms, rgKeyTerms))
                continue;
            n= i;
            for(j=0;j<iSize;j++) {
                rgFProd[j]= rgPolyCoeffsPre[n];
                n+= iSize;
            }
            sprintf(rgLabel,"\nTerm ",NULL);
            printTerm(nVar, (short unsigned int) i, rgLabel);
            k= strlen(rgLabel);
            sprintf(&rgLabel[k], ", key bit polynomial coefficient, ", NULL);
            BooleanReport(rgLabel, nVar, rgFProd, rgPolyCoeffs, rgxSpec, 
                          &nTerms, false, false);
            if(fPreKeyCoeffs) {
                fillRelationMatrix(nVar, iSize, (short unsigned int) i, 
                                rgPolyKeyMatrixReorder, nTerms, rgPolyCoeffs, 
                                rgPolyPreKeyMatrix);
            }
        }
    }

    // Now get minimum weight linear combinations
    if(fPreKeyCoeffs && fKeyConstraints) {
        printf("\nPreKey Constraints\n");
        findKeyConstraints(nVar, iSize, rgPolyKeyMatrixReorder, rgPolyPreKeyMatrix);
    }
    if(fPostKeyCoeffs && fKeyConstraints) {
        printf("\nPostKey Constraints\n");
        findKeyConstraints(nVar, iSize, rgPolyKeyMatrixReorder, rgPolyPostKeyMatrix);
    }

Done:
    if(rgBox!=NULL)
        free(rgBox);
    rgBox= NULL;
    if(rgFProjections!=NULL)
        free(rgFProjections);
    rgFProjections= NULL;
    if(rgxSpec!=NULL)
        free(rgxSpec);
    rgxSpec= NULL;
    if(rgPolyCoeffs!=NULL)
        free(rgPolyCoeffs);
    rgPolyCoeffs= NULL;
    if(rgFProd!=NULL)
        free(rgFProd);
    rgFProd= NULL;
    if(rgPolyCoeffsPost!=NULL)
        free(rgPolyCoeffsPost);
    rgPolyCoeffsPost = NULL;
    if(rgPolyCoeffsPre!=NULL)
        free(rgPolyCoeffsPre);
    rgPolyCoeffsPre= NULL;
    if(rgKeyTerms !=NULL)
        free(rgKeyTerms);
    rgKeyTerms = NULL;

    if(rgPolyPostKeyMatrix!=NULL)
        free(rgPolyPostKeyMatrix);
    rgPolyPostKeyMatrix= NULL;

    if(rgPolyPreKeyMatrix!=NULL)
        free(rgPolyPreKeyMatrix);
    rgPolyPreKeyMatrix= NULL;
    if(rgPolyKeyMatrixReorder!=NULL)
        free(rgPolyKeyMatrixReorder);
    rgPolyKeyMatrixReorder= NULL;

    printf("\n\nDone\n");
    return(0);
}


// ----------------------------------------------------------------


