#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>



/* --------------------------------------------------------------------- */


int main(int an, char** av) 

{
    char    szhostName[256];
    int     n= 256;

    if(gethostname(szhostName, n)!=0) {
        printf("Can't get hostname\n");
        return 1;
    }
    printf("hostname: %s\n", szhostName);
    return 0;
}


/* --------------------------------------------------------------------- */


