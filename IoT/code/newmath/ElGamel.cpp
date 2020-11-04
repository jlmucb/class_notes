#include <stdio.h> 
#include <string.h>
#include <io.h>
// #include <fcntl.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <stdlib.h> 


// ---------------------------------------------------------------------------------------


inline int squarep(int p, int n)
{
    return((n*n)%p);
}


int expp(int p, int g, int e)
{
    int s, x;
    int i;

    s= g;
    x= 1;
    for(i=0;i<32;i++) {     // integer size dependant
        if(e&1) {
            x= (x*s)%p;
        }
        e>>= 1;
        s= squarep(p,s);
    }
    return(x);
}


void Euclid(int iA, int iB, int* piQ, int* piR)
{
	*piQ= iA/iB;
	*piR= iA-(*piQ*iB);
	}


bool ExtendedGCD(int iA, int iB, int* piA, int* piB, int* piGCD)
{
	int ia, ib, iaold, ibold, ianew, ibnew, ir, irold, irnew, iq;

	if(iA<iB)
		return(false);
	irold=	iA;
	iaold=	1;
	ibold=	0;
	ia=	0;
	ib=	1;
	ir=	iB;
	irnew=	0;

	for(;;) {
		Euclid(irold, ir, &iq, &irnew);
		if(irnew==0)
			break;
		ianew= iaold-iq*ia;
		ibnew= ibold-iq*ib;
		iaold= ia; ibold= ib; irold= ir;
		ia= ianew; ib= ibnew; ir= irnew;
		}

	*piA= ia;
	*piB= ib;
	*piGCD= ir;
	return(true);
	}


int inversep(int p, int a)
{
    int x,y,gcd;

    if(!ExtendedGCD(a,p,&x,&y,&gcd))
        ExtendedGCD(p,a,&y,&x,&gcd);
    printf("Extended: %d*%d + %d*%d= %d\n",a,x,p,y,gcd);
    return(x%p);
}


int main(int an, char** av) 
{ 
    int p, g, a, b, m;
    int A, B, AB;
    int c1, c2;
    int BA, BAInv, cDecode;

    //  ElGamel p g a b m
    //  Prints ciphertext

    if(an<6) {
        printf( "ElGamel p g a b m\n");
        return(1);
    }

    sscanf(av[1], "%d", &p);
    sscanf(av[2], "%d", &g);
    sscanf(av[3], "%d", &a);
    sscanf(av[4], "%d", &b);
    sscanf(av[5], "%d", &m);
    printf("El Gamel p: %d, g: %d, a: %d, b: %d, m: %d\n",
            p,g,a,b,m);

    A= expp(p,g,a);
    B= expp(p,g,b);
    c1= B;
    AB= expp(p,A,b);
    c2= (m*AB)%p;
    printf("g^a: %d, g^b: %d, g^ab: %d, c: (%d, %d)\n", A, B, AB, c1, c2);

    BA= expp(p, c1, a);
    BAInv= inversep(p,BA);
    printf("BA: %d, BAInv: %d, BA*BAInv: %d\n",BA, BAInv, (BA*BAInv)%p);
    cDecode= (c2*BAInv)%p;
    printf("Decode check: %d\n", cDecode);

    return(0);
}


// ---------------------------------------------------------------------------------------




