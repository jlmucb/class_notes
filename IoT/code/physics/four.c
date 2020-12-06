#include <stdio.h>
#include <math.h>


/*
 *	(c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */


#define PI 3.1415926535


double fourstep(x,n)

double x;
int n;

/*
 *	Evaluate 1/2+ sum-to-n [Am sin(m pi x)]
 *	where Am= -(2/(pi m)) when m is odd, 0 when even
 *	this should be a fourier appx to step fnctn
 */

{
	int i,j;
	double y,z,c;

	y= 0.0;
	for(j=1;j<=n;j++) {
		if((j&01)!=0) {
			c= j;
			y+= sin(c*PI*x)/c;
			}
		}
	return(0.5-((2.0/PI)*y));
}

main(argn,argv)

int argn;
char *argv[];

{
	int i,j,k;
	double d,x,y;

	x= 0.0;
	d= 0.1;
	j= atoi(argv[1]);
	printf("\n\nApproximation with %d terms\n",j);
	for(i=0;i<20;i++) {
		y= fourstep(x,j);
		if(x<1.0)
		    printf("x: %f, F(x): 0, appx: %f, diff: %f\n",x,y,fabs(y));
		else
		    printf("x: %f, F(x): 1, appx: %f, diff: %f\n",x,y,fabs(1.0-y));
		x+= d;
		}

	exit();
}
