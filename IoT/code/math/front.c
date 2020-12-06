#include "windt.h"
#define DEBUG
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <string.h>
#include <stdio.h>


extern Int simplex(Int n,Int r,Double *a,Double *c);
extern void printall(Int m,Int n,Double *c,Double *a,char *bv, int *cn);
extern Int gelim(Int n,Int m,Double *a,int *vn);


/* ----------------------------------------------------------------------- */



Int jm={-1};
Int jjn={-1};


int ncf={0};
Double cf[4096];
int nof={0};
Double of[50];


#define NVN 100
int vvn[NVN];


/* ----------------------------------------------------------------------- */



int nc={0};
char *cp;
#define NBUF  2048
char buf[NBUF];

#define MAXLINE 256
char line[MAXLINE];


int getac(int in)

{

	if(nc<=0) {
		if((nc=read(in,buf,NBUF))<=0)
			return(0);
		cp= buf;
		}
	nc--;
	return((int) (*(cp++)));
}



int getline(int in)

{
	int i,n;

	n= 0;
	while((i=getac(in))>0) {
		line[n++]= (char) i;
		if(i==((int)('\n'))){
			line[n]= '\0';
			return(n);
			}
		}
	return(0);
}



/* ----------------------------------------------------------------------- */


#define MAXFLDS 100
char *flds[MAXFLDS];



int getnum(char *p, Double *f)

{
	if(sscanf(p,"%lf",f)>0)
		return(1);
	return(0);
}



void asgn(int n)

{
	int i;
	Double x;

	if(jjn<0)
		for(i=0;i<n;i++)
			if(getnum(flds[i],&x)>0) {
				jjn= (Int) x;
				return;
				}
	if(jm<0)
		for(i=0;i<n;i++)
			if(getnum(flds[i],&x)>0) {
				jm= (Int) x;
				return;
				}
	if(strcmp(flds[0],"max")==0)  {
	    for(i=0;i<n;i++)
			if(getnum(flds[i],&x)>0) {
				of[ncf++]= x;
				}
		return;
		}

	for(i=0;i<n;i++)
		if(getnum(flds[i],&x)>0) {
			cf[nof++]= x;
			}
	return;
}


int parse(int n)

{
	int i;
	char *p,*q,c1;

	p= (q= line);
	i= 0;
	for(;;) {
		c1= *q;
		if((c1=='\n')||(c1=='\r')) {
			*q= '\0';
			flds[i++]= p;
			p= q+1;
			return(i);
			}
		if(c1==',') {
			*q= '\0';
			flds[i++]= p;
			p= q+1;
			}
		q++;
		}
}



/* ----------------------------------------------------------------------- */



main(an,av)

int an;
char *av[];

{
    int  i, j, ln, in;
    char *routine="simplex";


	if(an<1)
		printf("Bad input file spec\n");
	if((in=open(av[1],O_RDONLY,S_IREAD))<0) {
		printf("Bad open: %s\n",av[1]);
		exit(0);
		}
	if(an>2)
		routine= av[2];

    ln= 0;
	for(;;) {
		if((i=getline(in))<=0)
			break;
		j= parse(i);
		ln++;
		asgn(j);
		}
	close(in);

    if(strcmp(routine,"simplex")==0) {
    	printf("Variables: %ld, constraints: %ld\n",jm,jjn);
    	printf("Initial objective:\n");
		for(j=0;j<nof;j++)
			printf("%7.2f, ", of[j]);
		printf("\nCoefficients\n");
		for(i=0;i<((int)jm);i++) {
	    	for(j=0;j<=((int)jjn);j++)
				printf("%7.2f, ", cf[i*(jjn+1)+j]);
			printf("\n");
			}
		simplex(jjn,jm,cf,of);
		}

	if(strcmp(routine,"gauss")==0) {
		printf("Variables: %ld, constraints: %ld\n",jjn,jm);
		for(j=0;j<((int)jjn);j++)
			vvn[j]= j;
		printf("\nCoefficients\n");
		for(i=0;i<((int)jm);i++) {
	    	for(j=0;j<=((int)jjn);j++)
				printf("%7.2f, ", cf[i*(jjn+1)+j]);
			printf("\n");
			}
		j= gelim(jjn,jm,cf,vvn);
		printf("%d rows reduced.  Variable names: ",jm,jjn);
		for(j=0;j<((int)jjn);j++)
			printf("%d ",vvn[j]);
		printf("\n\nCoefficients\n");
		for(i=0;i<((int)jm);i++) {
	    	for(j=0;j<=((int)jjn);j++)
				printf("%7.2f, ", cf[i*(jjn+1)+j]);
			printf("\n");
			}
		}

	exit(0);
}



/* ----------------------------------------------------------------------- */
