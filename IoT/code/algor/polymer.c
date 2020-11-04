#include <stdio.h>


char p1[8]={7,6,5,4,3,2,1,0};

int diff1[256];

permute(bits,perm,n)

char bits[], perm[];
int n;

/*
 *      permute first n bits of bits[] using perm[].
 *	  perm[0..(n-1)]
 */
 
{
        int i;
        char tmp[256];

        for(i=0;i<n;i++)
                tmp[i]= bits[i];
#ifdef DEBUGP
        printf("\nBefore permutation by: ");
        for(i=0;i<n;i++)
                printf(" %d", perm[i]);
        printf("\n");
        for(i=0;i<n;i++)
                printf(" %o",tmp[i]);
#endif

        for(i=0;i<n;i++)
                bits[i]= tmp[perm[i]];
#ifdef DEBUGP
        printf("\nafter permutation: ");
        for(i=0;i<n;i++)
                printf("%o ",bits[i]);
#endif
        return;
}


tobits(x,p,n)

int x,n;
char p[];

{
	int i,j;

	for(i=0;i<n;i++) {
		if((x&01)!=0)
			p[i]= 1;
		else
			p[i]= 0;
		x>>= 1;
		}
	return(0);
}


frombits(p,n)

int n;
char p[];

{
	int i,j,k;

	k= 0;
	for(i=(n-1);i>=0;i--) {
		k<<= 1;
		if(p[i]!=0)
			k|= 1;
		}
	return(k);
}


main(argn,argv)

int argn;
char *argv[];

{
	int i,j,k,n;
	char q1[8],q2[8];
	
	n= 0;
	for(i=0;i<256;i++) {
		tobits(i,q1,8);
		permute(q1,p1,8);
		j= frombits(q1,8);
		for(k=0;k<n;k++)
			if((diff1[k]==i)||(diff1[k]==j))
				break;
		if(k>=n)
			diff1[n++]= i;
		}

	printf("%d configurations\n",n);
	for(k=0;k<n;k++) {
		printf("%d.\t%o\t",k,diff1[k]);
		tobits(diff1[k],q1,8);
		for(i=0;i<4;i++)
			if(q1[i]!=0)
				printf("%d ",i+1);
		for(i=4;i<8;i++)
			if(q1[i]!=0)
				printf("%d ",i+2);
		printf("\n");
		}
	printf("\ndone\n");

}


