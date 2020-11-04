#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <io.h>

//
//      InvGf2
//


// ---------------------------------------------------------------------------------------


unsigned reduce(int n, long long unsigned u, unsigned uM)
{
    long long unsigned luM= uM;
    long long luMask = 1;

#ifdef DEBUG1
    unsigned* pu= (unsigned*) &u;
    printf("reduce(%d, %08x %08x %08x) --- ", n, *(pu+1), *pu, uM);
#endif
    luM<<= n-1;
    luMask<<= 2*n-1;
    for(int i= (2*n); i>n; i--) {
        if(u&luMask)
            u^= luM;
        luM>>= 1;
        luMask>>= 1;
    }

#ifdef DEBUG1
    printf("returns %08x\n", (unsigned)u);
#endif
    return((unsigned) u);
}


unsigned FieldMultiply(int n, unsigned u, unsigned v, unsigned uM)
{
    long long unsigned w= 0;
    long long unsigned t= (long long unsigned) v;
    unsigned uMask= 1;

    for(int i=0; i<=n; i++) {
        if((uMask&u)!=0)
            w^= t;
        t<<= 1;
        uMask<<= 1;
    }
    return(reduce(n, w, uM));
}


void printrep(int n, unsigned u)
{
    unsigned uMask= 1<<n;

    for(int i=n; i>=0; i--) {
        if(u&uMask)
            printf("1");
        else
            printf("0");
        uMask>>= 1;
    }
}


int main(int an, char** av) 
{
    int             i, j, n; 
    unsigned        N;         // field size
    unsigned int    m[32];
    unsigned        uM= 0;
    unsigned int*   rgInv= NULL;
    unsigned        u,v,w;
    char*	    pFile= NULL;
    int		    iBitPos;


    if(an<6) {
        printf( "InvGf2 exp min-poly(atart@const) -extract filename bitpos\n");
        return(1);
    }
    sscanf(av[1], "%d", &n);
    if(n>31) {
        printf("Field too large must be < 2^32\n");
        return(1);
    }
    if(an<(n+2)) {
        printf("minimum poly too small n: %d, argn: %d\n", n, an);
        return(1);
    }
    for(i=0; i<=n; i++) {
        sscanf(av[i+2], "%d", &m[i]);
        }
    if(an>= (n+5)) {
	if(strcmp(av[n+3], "-extract")==0) {
            pFile= av[n+4];
            sscanf(av[n+5], "%d", &iBitPos);
	}
    }

    printf("InvGF2 for 2^%d, m(x)= ", n);
    for(i=n; i>=0; i--) {
        if(m[i]!=0) {
            if(i==0)
                printf("1");
            else
                printf("x^%d + ", i);
        }
    }
    printf("\n");

    for(i=n; i>=0; i--) {
        uM<<= 1;
        if(m[i]!=0)
            uM|= 1;
    }
#ifdef DEBUG1
    printf("uM: %08x\n", uM);
#endif

    N= (1<<n);
    rgInv= (unsigned*) malloc(N*sizeof(unsigned));
    if(rgInv==NULL) {
        printf("Cant allocate inverse array\n");
        return(1);
    }

    for(i=0; i<N; i++)
        rgInv[i]= 0;
    
    rgInv[1]= 1;
#ifdef DEBUG1
    printf("Entering loop\n");
#endif
    for(u=2; u<N; u++) {
        if(rgInv[u]!=0)
            continue;
        for(v=u+1; v<N; v++) {
#ifdef DEBUG1
            printf("FieldMultiply(%d, %08x, %08x, %08x)\n", n, u, v, uM);
#endif
            w= FieldMultiply(n,u,v,uM);
            if(w==1) {
                rgInv[u]= v;
                rgInv[v]= u;
                break;
            }
        }
    }

    for(u=0; u<N; u++) {
        printf("1/"); printrep(n,u);printf(" = ");printrep(n,rgInv[u]);printf("\n");
    }

    if(pFile!=NULL) {
	printf("\nPrintfile: %s, bit position %d\n", pFile, iBitPos);
	int out= creat(pFile,0x0080);
	for(u=0; u<N; u++) {
	    rgInv[u]= (rgInv[u]>>iBitPos)&0x01;
	    write(out,&rgInv[u], 1);
	}
	close(out);
    }
    free(rgInv);
    rgInv= NULL;
    return(0);
}


// ---------------------------------------------------------------------------------------


