#include <stdio.h> 
#include <string.h>
#include <io.h>
// #include <fcntl.h>
// #include <sys/types.h>
// #include <sys/stat.h>
#include <stdlib.h> 


// ---------------------------------------------------------------------------------------


void prColTable(int n, int* piTable) {
    int i;

    printf("    x     y    "); printf("    x     y    ");
    printf("    x     y    "); printf("    x     y\n");
    for(i=1;i<=n;i++) {
	    printf("%5d %5d    ",i,piTable[i]);
	    if((i%4)==0)
		    printf("\n");
    }
    printf("\n\n");
}


int main(int an, char** av) 
{ 
    int p, g, x;
    int i, n;
    int iCurExp;
    int *piLog= NULL;
    int *piExp= NULL;

    //  DiscreteLog p g x
    //  Prints ciphertext

    if(an<3) {
        printf( "DiscreteLog p g [x]\n");
        return(1);
    }

    sscanf(av[1], "%d", &p);
    sscanf(av[2], "%d", &g);
    if(an>3) {
    	sscanf(av[3], "%d", &x);
    	printf("DiscreteLog p: %d, g: %d, x: %d\n", p, g, x);
    }
    else {
    	printf("DiscreteLog p: %d, g: %d\n", p, g);
    }
    if(g<1 || g>=p) {
	    printf("Base must be positive and less than %d\n",p);
	    return(1);
    }

    piLog= (int*) malloc(sizeof(int)*p);
    if(piLog==NULL) {
	    printf("can't allocate log table\n");
	    return(1);
    }
    piExp= (int*) malloc(sizeof(int)*p);
    if(piExp==NULL) {
	    printf("can't allocate exp table\n");
	    if(piLog!=NULL) {
		    free(piLog);
		    piLog= NULL;
	    }
	    return(1);
    }

    for(i=0;i<p;i++)
	    piLog[i]= 0;

    iCurExp= g;
    for(i=1; i<p; i++) {
	    piExp[i]= iCurExp;
	    if(iCurExp==1)
		    break;
	    piLog[iCurExp]= i;
	    iCurExp= (iCurExp*g)%p;
    }

    n= i;

    // Exponent Table
    printf("exp_%d(x)=y\n", g);
    prColTable(n, piExp);

    if(n==(p-1)) {
         printf("%d is a primitive root (mod %d).  Log table follows.\n", g,p);
    }
    else {
	 printf("%d has order %d (mod %d)\n", g,n,p);
	 return(0);
    }

    // Log Table
    printf("log_%d(x)=y\n", g);
    prColTable(n, piLog);

    if(piLog!=NULL) {
	    free(piLog);
	    piLog= NULL;
    }
    if(piExp!=NULL) {
	    free(piExp);
	    piExp= NULL;
    }
    return(0);
}


// ---------------------------------------------------------------------------------------




