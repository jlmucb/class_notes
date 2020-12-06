#include <stdio.h>
#include <math.h>


//
//	location on earth using altitudes from 2 know objects
//
//	loc [-l1star]|dec1 ra1 [-l2star2] dec2 ra2 alt1 alt2
//


typedef int Int;
typedef double Double;


#define PI 3.141592653589793
#define todegree(xx) ((xx)*180.0/PI)
#define toradian(xx) ((xx)*PI/180.0)

Double ha={0.0};			// hour angle of aries at epoch
Double t= {0.0};			// time elapsed since epoch in seconds
Double w= {1.0/86160.0};   	// rate of rotation


/* ------------------------------------------------------------------------ */


spt1 (Double A,Double b,Double c,Double *a)

//	cos(a)= cos(b)cos(c)+sin(b)sin(c)cos(A)

{
	Double x;

	if(A<0.0)
		A*= (-1.0);
	x= cos(b)*cos(c)+sin(b)*sin(c)*cos(A);
	*a= acos(x);
	return(1);
}



spt2 (Double a,Double b,Double c,Double *A)

//	cos(a)= cos(b)cos(c)+sin(b)sin(c)cos(A)

{
	Double x;

	x= (cos(a)-cos(b)*cos(c))/(sin(b)*sin(c));
	*A= acos(x);
	return(1);
}


/* ------------------------------------------------------------------------ */


main(Int argn,char *argv[])

{
	Int i,j,n;
	Double d1,r1,d2,r2,a1,a2;				// declination, ra
	Double pp,p,z,zp,ss,lat,lng;
	Double x,y,u;

	if(argn<6) {
		printf("Calling sequence: loc dec ra dec ra alt1 alt2\n" );
		exit();
		}
	sscanf(argv[1],"%lf",&d1); sscanf(argv[2],"%lf",&r1);
	sscanf(argv[3],"%lf",&d2); sscanf(argv[4],"%lf",&r2);
	sscanf(argv[5],"%lf",&a1); sscanf(argv[6],"%lf",&a2);
	printf("Object1: (%.3lf,%.3lf), Object2: (%.3lf,%.3lf)\n",d1,r1,d2,r2);
	printf("Altitude 1: %.3lf, Altitude 2: %.3lf\n", a1,a2);
	p= toradian(90.0-d1);
	pp= toradian(90.0-d2);
	z= toradian(90.0-a1);
	zp= toradian(90.0-a2);

	spt1(toradian(r2-r1),p,pp,&ss);
	spt2(pp,ss,p,&x);
	spt2(zp,ss,z,&y);
	spt1(y-z,p,z,&lat);
	spt2(lat,p,z,&u);
	lng= r1-u-ha-2.0*PI*w*t;

	lat=todegree(lat);
	lng= todegree(lat);
	printf("lat: %.3f, lng: %.3f\n",lat,lng);

	exit();
}


/* ------------------------------------------------------------------------ */

