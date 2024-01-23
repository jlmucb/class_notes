#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <string.h>
#include <io.h>
#include <unistd.h>

//
//      TestByte
//


// ---------------------------------------------------------------------------------------




int main(int an, char** av) 
{
    int                 n= 8;
    int                 i, j, k;
    short unsigned int  rgSub[256];
    bool                fDump= false;
    char*               strOut= NULL;


    printf("Linear testbyte.  [-p filename]\n");
    for(i=0;i<an;i++) {
        if(strcmp(av[i],"-p")==0) {
            i++;
            if(i<an) {
                fDump= true;
                strOut= av[i];
            }
        }
    }


    for(i=0; i<256; i++) {
        rgSub[i]= (short unsigned int) i;
    }

    printf("\n\n");
    for(i=0; i<16; i++) {
        for(j=0;j<16;j++) {
            printf("%02x ", rgSub[16*i+j]);
        }
        printf("\n");
    }

    if(fDump) {
        int iOut= open(strOut, O_RDWR | O_CREAT | O_TRUNC, 0644);
        printf("Saving to %s\n", strOut);
        if(iOut>0)
            write(iOut, rgSub, 256*sizeof(short unsigned int));
        close(iOut);
    }

    printf("\n\nDone\n");
    return(0);
}


// ---------------------------------------------------------------------------------------


