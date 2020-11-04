#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>


// GFSolve.cpp


#ifndef byte
typedef unsigned char byte;
#endif

extern int strcmp(const char*, const char*);

const int N= 8;


byte rgbCoeff[N*N]= {
    0, 1, 1, 0, 0, 1, 1, 1, 
    1, 1, 0, 0, 1, 1, 1, 1, 
    1, 0, 0, 1, 1, 1, 1, 1, 
    0, 0, 1, 1, 1, 1, 1, 0, 
    0, 1, 1, 1, 1, 1, 0, 1, 
    1, 1, 1, 1, 1, 0, 1, 1, 
    1, 1, 1, 1, 0, 1, 1, 1, 
    1, 1, 1, 0, 1, 1, 1, 1
};


byte rgC[N]= {
    1, 1, 0, 1, 1, 1, 1, 0
};



// --------------------------------------------------------------------------


void printBitMatrix(int nr, int nc, byte rgMatrix[])
{
    int i, j;

    for(i=0; i<nr; i++) {
        printf("\t");
        for(j=0; j<nc; j++) {
            printf("%d ", rgMatrix[nc*i+j]);
        }
        printf("\n");
    }
}


void AddToRow(int sn, byte* rgbDest, byte* rgbToBeAdded)
{
    int i;

    for(i=0; i<sn; i++) 
        rgbDest[i]^= rgbToBeAdded[i];
}


bool GF2RowGE(int sn, byte* rgMat, byte* rgC, byte* rgB)

//
//  Solve linear equations of form M [a0, a1, ..., a15]^M = [c0, c1, ... , c15]^T
//
//      rgB gets answers
//

{
    int*    rgiRearrange= (int*) malloc(sn*sizeof(int));
    byte*   rgM= (byte*) malloc(sn*sn);
    byte*   rgA= (byte*) malloc(sn);
    int     i,j,k;
    bool    fSwapFound= false;

    if(rgiRearrange==NULL || rgM==NULL || rgA==NULL) {
        printf("GF2Solver: Can't get memory\n");
        return false;
    }

    // Copy and init
    for(i=0; i<sn; i++) {
        rgiRearrange[i]= i;
        rgA[i]= rgC[i];
        for(j=0; j<sn; j++) {
            rgM[sn*i+j]= rgMat[sn*i+j];
        }
    }

    for(i=0; i<sn; i++) {
        
        // Make sure ith row has 1 in ith column
        if(rgM[sn*rgiRearrange[i]+i]==0) {
            // swap with another row
            fSwapFound= false;
            for(j= (i+1); j<sn; j++) {
                if(rgM[sn*rgiRearrange[j]+i]!=0) {
                    k= rgiRearrange[j];
                    rgiRearrange[j]= rgiRearrange[i];
                    rgiRearrange[i]= k;
                    fSwapFound= true;
                    break;
                }
            }
            if(fSwapFound==false) {
                printf("GF2Solver: Equations not solvable at %d\n",i);
                goto Failed;
            }
        }

        // add row i to the remaining ones
        for(k=(i+1); k<sn; k++)  {
            if(rgM[sn*rgiRearrange[k]+i]!=0) {
                AddToRow(sn, &rgM[sn*rgiRearrange[k]], &rgM[sn*rgiRearrange[i]]);
                rgA[rgiRearrange[k]]^= rgA[rgiRearrange[i]];
            }
        }
    }

    // now do backwards pass
    for(i=(sn-1);i>0;i--) {
        for(j=(i-1); j>=0; j--) {
            if(rgM[sn*rgiRearrange[j]+i]!=0) {
                AddToRow(sn, &rgM[sn*rgiRearrange[j]], &rgM[sn*rgiRearrange[i]]);
                rgA[rgiRearrange[j]]^= rgA[rgiRearrange[i]];
            }
        }
    }

    for(i=0; i<sn; i++)
        rgB[i]= rgA[rgiRearrange[i]];

    if(rgiRearrange!=NULL) {
        free(rgiRearrange);
        rgiRearrange= NULL;
    }
    if(rgM!=NULL) {
        free(rgM);
        rgM= NULL;
    }
    if(rgA!=NULL) {
        free(rgA);
        rgA= NULL;
    }
    return true;

Failed:
    if(rgiRearrange!=NULL) {
        free(rgiRearrange);
        rgiRearrange= NULL;
    }
    if(rgM!=NULL) {
        free(rgM);
        rgM= NULL;
    }
    if(rgA!=NULL) {
        free(rgA);
        rgA= NULL;
    }
    return false;
}


int main(int an, char** av) 

{
    int     i, j, k;
    bool    fDebugPrint= false;
    byte    rgbAns[N];


    printf("\nMatrix:\n");
    printBitMatrix(N, N, rgbCoeff);
    printf("\nConstants:\n");
    printBitMatrix(N, 1, rgC);

    // Gaussian Elimination
    if(!GF2RowGE(N, rgbCoeff, rgC, rgbAns)) {
        printf("Cant solve linear equations\n");
        return 1;
    }

    printf("\nSolution:\n");
    printBitMatrix(N, 1, rgbAns);

    return 0;
}


// ---------------------------------------------------------------------------


