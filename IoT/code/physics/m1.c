#define PI 3.141592653589793
#define LN2 0.69314718

double sqrt(), sin(), cos(), asin(), acos(), tan(), atan(), ln(), exp();


/*
 *	(c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */

#define FABS

double pi2={PI/2.0};
double tpi={2.0*PI};
double thpi={PI+(PI/2.0)};
double e={2.718281828459045};

/* -------------------------------------------------------------- */


double fabs(x)

double x;

{
	if(x<0.0)
		return(-x);
	return(x);
}


/*
 *	sqrt, sin, cos, asin, acos, tan, atan, ln, exp
 */


double sqrt(x)

double x;

{
	double a,b;

#ifdef DEBUG7
	printf("sqrt(%f)\n",x);
#endif
	if(x<0.0)
		return(-1);
	if(x==0.0)
		return(0.0);
	if(x>(1.0e200)) {
		printf("square root error\n");
		return(0);
		}
	a= x;
	while((a*a)>(100.0*x)) {
		if(a>10.0)
			a/= 10.0;
		else
			break;
		}
	for(;;) {
		a= (a+(x/a))*0.5;
		if((fabs(x-(a*a))/x)<0.0000001)
			return(a);
		}
}


/* ---------------------------------------------------------------- */


#define NBITS 50
static double sqr2[NBITS];


main(an,av)

int an;
char *av[];

{
	unsigned *u;
	double x,y,z;
	int i,j,k;

	x= 2.0;
	for(i=0;i<NBITS;i++) {
		y= sqrt(x);
		sqr2[i]= y;
		x= y;
		}

	u= (unsigned *)(&sqr2[0]);
	for(i=0;i<NBITS;i++) {
		printf("%f, %x, ",sqr2[i],*(u++));
		printf("%x\n",*(u++));
		};
	u= (unsigned *)(&sqr2[0]);
	printf("={\n");
	for(i=0;i<(NBITS/2);i++) {
		printf("0x%x,",*(u++)); printf("0x%x,",*(u++));
		printf("0x%x,",*(u++)); printf("0x%x,\n",*(u++));
		};
	printf("};\n");
	exit(1);
}



/* ---------------------------------------------------------------- */


