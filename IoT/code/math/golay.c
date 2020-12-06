#include "stdio.h"
#include "io.h"
#include "string.h"


char mseed[11]={1,1,0,1,1,1,0,0,0,1,0};
char mm[121];

char gm[288];               // golay code generator matrix
long gcbv[24];              // golay code basis vectors
long int gcw[4096];         // golay code vectors
int wtgcw[4096];		// weight of codeword
long int cweights[25];         // code vector weights


void mton(int nb, char m[],long *n)

{
	int i;

	nb--;
	*n= (long) 0;
	for(i=0;i<=nb;i++)
		(*n)|= ((long) (m[nb-i]))<<i;
}


void ntom(int nb, long n, char *m)

{
	int i;

	nb--;
	for(i=0;i<=nb;i++)
		m[nb-i]= (char) ((n>>i)&1);
}



void circmseed()

{
	int i,j;

	j= mseed[0];
	for(i=0;i<10;i++)
		mseed[i]= mseed[i+1];
	mseed[10]= (char)  j;
}



void prmat (char m[],int r,int c)

// print matrix m; r rows, c columns

{
	int i,j;

	for(i=0;i<r;i++) {
		for(j=0;j<c;j++) {
			printf("%d ",(int)m[c*i+j]);
			}
		printf("\n");
		}
}


void lrmat (char m[],int r,int c, char *cp)

// print matrix m; r rows, c columns

{
	int i,j;

	for(i=0;i<r;i++) {
		for(j=0;j<c;j++) {
			sprintf(cp,"%d ",(int)m[c*i+j]);
			cp+= 2;
			}
		sprintf(cp,"\n\0");
		}
}



main(an,av)

int an;
char *av[];

// generate golay code

{
	int i,j,k;
	long x;
	char cw[25];
	char line[100];
	int out;


	// printf("Golay code generator, seed\n");
	// prmat(mseed,1,11);

	// initialize M
	for(i=0;i<11;i++) {
		for(j=0;j<11;j++)
			mm[11*i+j]= mseed[j];
		circmseed();
		}
	// prmat(mm,11,11);

	for(i=0;i<11;i++) {
		gm[24*i]= 1;
		gm[24*i+12]= 0;
		for(j=0;j<11;j++)
			gm[24*i+1+j]= (char) (i==j);
		for(j=0;j<11;j++)
			gm[24*i+13+j]= mm[11*i+j];
		}
	for(i=0;i<24;i++)
		gm[24*11+i]= (char)(i>=12);

	printf("\nGolay Generator:\n\n");
	prmat(gm,12,24);

	for(i=0;i<12;i++)
		mton(24,&gm[i*24],&gcbv[i]);
	printf("\n\n");

		printf("\n\nBasis vectors\n");
		for(i=0;i<12;i++){
			printf("%06lx ",gcbv[i]);
			if(i==5)
				printf("\n");
			}
		printf("\n");

	for(i=0;i<4096;i++) {
		j= i;
		x= (long)0;
		for(k=0;k<12;k++) {
			if((j&1)!=0)
				x^= gcbv[k];
			j>>= 1;
			}
		gcw[i]= x;
		}

    out= 0;
    if((out=creat("goout.txt",0))<0)
    	printf("Cant create\n");

    for(i=0;i<25;i++)
	    cweights[i]= 0;
    for(i=0;i<4096;i++) {
    	ntom(24,gcw[i],cw);
    	k= 0;
    	for(j=0;j<24;j++) {
    		if(cw[j]!=0)
    			k++;
	}
	wtgcw[i]= k;
	cweights[k]++;
    	sprintf(line,"%02d %06lx ",k,gcw[i]);
    	lrmat(cw,1,24,&line[strlen(line)]);
    	if(out>0)
    		write(out,&line,strlen(line));
    	// if(i<10) printf("%s",line);
    	}
    for(i=0; i<25; i++) {
	    if(cweights[i]==0)
		    continue;
	    printf("%04d codewords of weight %2d\n",cweights[i], i);
    }

    if(out>0)
    	close(out);
	printf("done\n");
	return(0);
}
