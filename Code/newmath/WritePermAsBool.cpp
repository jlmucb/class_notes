#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


// WritePermAsBool.cpp


#ifndef byte
typedef unsigned char byte;
#endif


int iNumBits= 4;
int iSizeofPerm= 16;
byte rgPerm[512]= { 
     1,     //  0
     2,     //  1
     3,     //  2
     0,     //  3
     5,     //  4
     4,     //  5
     7,     //  6
     8,     //  7
     6,     //  8
     9,     //  9
    10,     // 10
    11,     // 11
    13,     // 12
    14,     // 13
    12,      // 14
    15,      // 15
    0
};


byte rgSeen[512];
byte rgf[512];



// --------------------------------------------------------------------- 


void Print(int iTxtOut)
{
    int     i, j, n;
    int     iNext;
    char    rgLine[1024];
    char    rgTmp[128];

    printf("Print called %d\n", iTxtOut);
    // Write permutation
    for(j=0;j<iSizeofPerm; j++)
        rgSeen[j]= 0;

    sprintf(rgLine, "Analysis of boolean functions underlying permutation on %d letters below\n",
            iSizeofPerm);
    write(iTxtOut, rgLine, strlen(rgLine));
    strcpy(rgLine, "Permutation: ");

    n= 0;  // <iSizeofPerm
    while(n<iSizeofPerm) {
        // new cycle
        if(rgSeen[n]!=0) {
            n++;
            continue;
        }
        sprintf(rgTmp, "(%d", n);
        strcat(rgLine, rgTmp);

        rgSeen[n]= 1;
        iNext= rgPerm[n];
        for(;;) {
            if(rgSeen[iNext]!=0) {
                strcat(rgLine,") ");
                break;
            }
            sprintf(rgTmp, " %d", iNext);
            strcat(rgLine, rgTmp);
            rgSeen[iNext]= 1;
            iNext= rgPerm[iNext];
        }
    }
    strcat(rgLine,"\n\n");
    write(iTxtOut, rgLine, strlen(rgLine));
}



int main(int an, char** av) 

{
    int     i, j;
    int     iOut, iTxtOut;
    char    rgBuf[128];

    if(an<2 || av[1]==NULL) {
        printf("No file\n");
        return(1);
    }

    strcpy(rgBuf, av[1]);
    strcat(rgBuf, ".dat");
    if((iOut= _creat(rgBuf, _S_IREAD | _S_IWRITE))<0) {
        printf("Cant creat %s\n", rgBuf);
        return(1);
    }
    strcpy(rgBuf, av[1]);
    strcat(rgBuf, ".txt");
    if((iTxtOut= _creat(rgBuf, _S_IREAD | _S_IWRITE))<0) {
        printf("Cant creat %s\n", rgBuf);
        return(1);
    }


    // Calculate permutation as cycles
    Print(iTxtOut);

    for(i=(iNumBits-1);i>=0;i--) {
        for(j=0;j<iSizeofPerm; j++) {
            rgf[j]= (rgPerm[j]>>i)&1;
        }
        write(iOut, rgf, iSizeofPerm);
    }
    close(iOut);
    close(iTxtOut);

    printf("Done %s\n", av[1]);
    return(0);
}


/* --------------------------------------------------------------------- */
