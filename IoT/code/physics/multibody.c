#include <stdio.h>

/* #define DEBUG */


/*
 *	(c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */

/* ---------------------------------------------------------------------- */

#define N 100
#define NC 100

double xx1[N],yy1[N],zz1[N];
double m1[N];

double xx2,yy2,zz2;
double m2;
double vxx2,vyy2,vzz2;

double kk[N];


/* ---------------------------------------------------------------------- */


main(argn,argv)

int argn;
char *argv[];

/*
 *	multibody problem
 */

{
	int i,j,n,ns,skip,skp1;
	double t,dt;
	double ux2,uy2,uz2,uvx2,uvy2,uvz2;
	char tmp[NC];

	fprintf(stdout,"Number of planets: "); fflush(stdout);
	if((j=readit(0,tmp,NC))<=0) {
		printf("invalid number\n");
		exit();
		}
	sscanf(tmp,"%d\n",&n);
	fprintf(stdout,"Number of steps: "); fflush(stdout);
	if((j=readit(0,tmp,NC))<=0) {
		printf("invalid number\n");
		exit();
		}
	sscanf(tmp,"%d\n",&ns);
	fprintf(stdout,"Number of steps skipped before printing: "); fflush(stdout);
	if((j=readit(0,tmp,NC))<=0) {
		printf("invalid number\n");
		exit();
		}
	sscanf(tmp,"%d\n",&skip);
	fprintf(stdout,"Step size: "); fflush(stdout);
	if((j=readit(0,tmp,NC))<=0) {
		printf("invalid number\n");
		exit();
		}
	sscanf(tmp,"%F\n",&dt);

	for(i=0;i<n;i++) {
		fprintf(stdout,"Position of planet %d: ",i); fflush(stdout);
		if((j=readit(0,tmp,NC))<=0) {
			printf("invalid number\n");
			exit();
			}
		sscanf(tmp,"(%F,%F,%F)\n",&xx1[i],&yy1[i],&zz1[i]);
		fprintf(stdout,"Planet %d attraction: ",i); fflush(stdout);
		if((j=readit(0,tmp,NC))<=0) {
			printf("invalid number\n");
			exit();
			}
		sscanf(tmp,"%F\n",&kk[i]);
		}
	fprintf(stdout,"Position of sattelite: ",i); fflush(stdout);
	if((j=readit(0,tmp,NC))<=0) {
		printf("invalid number\n");
		exit();
		}
	sscanf(tmp,"(%F,%F,%F)\n",&xx2,&yy2,&zz2);
	fprintf(stdout,"Velocity of sattelite: ",i); fflush(stdout);
	if((j=readit(0,tmp,NC))<=0) {
		printf("invalid number\n");
		exit();
		}
	sscanf(tmp,"(%F,%F,%F)\n",&vxx2,&vyy2,&vzz2);
	printf("\n%d planets, %d steps, %.3e second time step, %d skipped\n",
		n,ns,dt,skip);
	printf("\nInitial positions of Planets");
	printf("\nPlanet\t\t(x,y,z)\n");

	for(i=0;i<n;i++) {
		printf("%d\t\t(%.2e,%.2e,%.2e)\n",i,xx1[i],yy1[i],zz1[i]);
		}

	t= 0.0;
	printf("\nTime\t\t(x,y,z)\t\t\t\t(vx,vy,vz)\n");
	printf("%.3e\t(%.2e,%.2e,%.2e)\t(%.2e,%.2e,%.2e)\n",
			t,xx2,yy2,zz2,vxx2,vyy2,vzz2);
	fprintf(stdout,"\nReady? "); fflush(stdout);
	if((j=readit(0,tmp,NC))<=0) {
		printf("invalid number\n");
		exit();
		}
	printf("\n");
	skp1= skip;
	j= 1;
	for(i=1;i<ns;i++) {
		newcoord(dt,2,kk,xx1,yy1,zz1,
			 xx2,yy2,zz2,vxx2,vyy2,vzz2,
			 &ux2,&uy2,&uz2,&uvx2,&uvy2,&uvz2);
		t+= dt;
		xx2= ux2;
		yy2= uy2;
		zz2= uz2;
		vxx2= uvx2;
		vyy2= uvy2;
		vzz2= uvz2;
		if((skp1--)>0)
			continue;
		skp1= skip;
		if(j>20) {
			printf("\nTime\t\t\t\t(x,y,z)\t\t(vx,vy,vz)\n");
			j= 1;
			}
		printf("%.3e\t(%.2e,%.2e,%.2e)\t(%.2e,%.2e,%.2e)\n",
			t,xx2,yy2,zz2,vxx2,vyy2,vzz2);
		}

	printf("program done\n");
	exit();
}


/* ---------------------------------------------------------------------- */


newcoord(dt,n,k,x1,y1,z1,xx,yy,zz,vxx,vyy,vzz,x2,y2,z2,vx2,vy2,vz2)

int n;				/* number of bodies */
double dt;			/* time interval */
double k[],			/* constants */
       x1[],y1[],z1[],		/* position vectors other objects */
       xx,yy,zz,		/* old position */
       vxx,vyy,vzz,		/* old velocity */
       *x2,*y2,*z2,		/* new position */
       *vx2,*vy2,*vz2;		/* new velocity */

/*
 *	calculate new state vectors
 *		f is the force function per unit mass
 */

{
	int i;
	double bx,by,bz;

#ifdef DEBUG1
	printf("newcoord\n");
#endif
	*vx2= vxx;
	*vy2= vyy;
	*vz2= vzz;
	for(i=0;i<n;i++) {
		f(k[i],x1[i],y1[i],z1[i],xx,yy,zz,&bx,&by,&bz);
		*vx2+= dt*bx;
		*vy2+= dt*by;
		*vz2+= dt*bz;
		}
	*x2= xx+dt*vxx;
	*y2= yy+dt*vyy;
	*z2= zz+dt*vzz;
#ifdef DEBUG
	printf("old pos: (%.3e,%.3e,%.3e), new: (%.3e,%.3e,%.3e)\n",xx,yy,zz,*x2,*y2,*z2);
	printf("old vel: (%.3e,%.3e,%.3e), new: (%.3e,%.3e,%.3e)\n",vxx,vyy,vzz,*vx2,*vy2,*vz2);
#endif
	return;
}


f(k,x1,y1,z1,x2,y2,z2,ax,ay,az)

double k,x1,y1,z1,x2,y2,z2,*ax,*ay,*az;

{
	double x,y,z,d,d3;
	double sqrt();

#ifdef DEBUG
	printf("calculating f: %.3e (%.3e,%.3e,%.3e) (%.3e,%.3e,%.3e)\n",k,x1,y1,z1,x2,y2,z2);
#endif
	x= x1-x2;
	y= y1-y2;
	z= z1-z2;
	d= x*x+y*y+z*z;
	/* d= x*x; d+= y*y; d+= z*z; */
#ifdef DEBUG1
	printf("final d: %.3e\n",d);
#endif
	d= sqrt(d);
#ifdef DEBUG1
	printf("sqrt(d): %.3e\n",d);
#endif
	x/= d;
	y/= d;
	z/= d;
	d3= d*d;
	*ax= k*x/d3;
	*ay= k*y/d3;
	*az= k*z/d3;
#ifdef DEBUG
	printf("calculating f done: (%.3e,%.3e,%.3e)\n",*ax,*ay,*az);
#endif
}


readit(i,q,n)

int i,n;
char *q;

{
	int k;
	char *p;

	p= q;
	k= 0;
	while(k<n) {
		if(read(i,p,1)<1)
			return(k);
		k++;
		if(*p=='\n') {
			*(++p)= '\0';
#ifdef DEBUG
			printf("readit(%d): %s",k,q);
#endif
			return(k);
			}
		p++;
		}
	return(k);
}


/* ---------------------------------------------------------------------- */



