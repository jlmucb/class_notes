#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <string.h>
#include <io.h>
#include <unistd.h>

//
//      SubByte
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


unsigned int    rgM[8*8]= { 1,0,0,0,1,1,1,1 };
unsigned int    rga[8]= { 1,1,0,0,0,1,1,0 };


void printM()
{
    int i,j;

    printf("\nM\n");
    for(i=0; i<8; i++) {
        for(j=0; j<8; j++) {
            printf("%d ", rgM[8*i+j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("\n");
}


unsigned int SubCalc(unsigned int t)
{
#if 1
    int             i,j;
    unsigned int    rgIn[8];
    unsigned int    rgOut[8];
    unsigned        u, v;

    for(i=0; i<8; i++) {
        rgIn[i]= (t>>i)&1;
    }

    u= 0;
    for(i=0; i<8; i++) {
        v= 0;
        for(j=0; j<8; j++) {
            v^= rgM[8*i+j]*rgIn[j];
        }
        u|= (v^rga[i])<<i;
    }

    return u;
#else
    return t;
#endif
}



int main(int an, char** av) 
{
    int             n= 8;
    unsigned int    m[32]= {1,1,0,1,1,0,0,0,1};
    int             i, j;
    unsigned        N= 1<<n; // field size
    unsigned        uM= 0;
    unsigned int*   rgInv= NULL;
    unsigned        u,v,w;
    int             iBitPos;
    unsigned int    rgSub[256];
    bool            fDump= false;
    char*           strOut= NULL;

    
    //
    // M= circ(1,0,0,0,1,1,1,1)
    // a= (1,1,0,0,0,1,1,0)T  Lefttmost is MSB
    // Arithmetic over GF(2) with m(x)= x8+x4+x3+x+1.
    //

    printf("Calculate AES SubByte.  [-p filename]\n");
    for(i=0;i<an;i++) {
        if(strcmp(av[i],"-p")==0) {
            i++;
            if(i<an) {
                fDump= true;
                strOut= av[i];
            }
        }
    }

    for(i=1; i<8; i++) {
        for(j=0;j<8;j++) {
            rgM[8*i+j]= rgM[(8+j-i)%8];
        }
    }

    printf("SubByte, m(x)= ");
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

    rgInv= (unsigned*) malloc(N*sizeof(unsigned));
    if(rgInv==NULL) {
        printf("Cant allocate inverse array\n");
        return(1);
    }

    for(i=0; i<N; i++)
        rgInv[i]= 0;
    
    rgInv[0]= 0;
    rgInv[1]= 1;

    for(u=2; u<N; u++) {
        if(rgInv[u]!=0)
            continue;
        for(v=u+1; v<N; v++) {
            w= FieldMultiply(n,u,v,uM);
            if(w==1) {
                rgInv[u]= v;
                rgInv[v]= u;
                break;
            }
        }
    }

    for(u=0; u<N; u++) {
        rgSub[u]= SubCalc(rgInv[u]);
        // printf("1/"); printrep(n,u);printf(" = ");printrep(n,rgInv[u]);printf("\n");
    }


    printf("\n\n\n");
    for(i=0; i<16; i++) {
        for(j=0;j<16;j++) {
            printf("%02x ", rgSub[16*i+j]);
        }
        printf("\n");
    }

    if(fDump) {
	short unsigned int rgwSub[256];
	for(i=0; i<256;i++)
		rgwSub[i]= (short unsigned int) rgSub[i];
        int iOut= open(strOut, O_RDWR | O_CREAT | O_TRUNC, 0644);
        printf("Saving to %s\n", strOut);
        if(iOut>0)
            write(iOut, rgwSub, 256*sizeof(short unsigned int));
        close(iOut);
    }

    // printM();
    printf("\n\nDone\n");
    free(rgInv);
    rgInv= NULL;
    return(0);
}


// ---------------------------------------------------------------------------------------


