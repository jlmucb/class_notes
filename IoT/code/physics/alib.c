#include "alib.h"
#include "math.h"
#include <io.h>
#include <stdio.h>
#include <string.h>

/*
 *	(c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */

char *moname[12]= {
	"january",
	"february",
	"march",
	"april",
	"may",
	"june",
	"july",
	"august",
	"september",
	"october",
	"november",
	"december"
	};

Int modays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
Int tmodays[13] = {0,31,59,90,120,151,181,212,243,273,304,334,365};
Int tmmodays[13] = {0,31,60,91,121,152,182,213,244,274,305,335,366};
Int dinyr[20]= {0,366,731,1096,1461,0,365,730,1095,1461,
		0,365,730,1096,1461,0,365,731,1096,1461};
Int lpy[4]= {0,3,2,1};

Int epoch={EPOCH};


/* ------------------------------------------------------------------ */


scmp1(Char *p,Char *q)

{
	while(*p!='\0') {
		if((*p)!=(*q))
			return(0);
		p++; q++;
		}
	return(1);
}


Int month12(char *name)


/*
 *	return month number given name
 */

{
	Int i;
	char *p, *q, n[20];

	p= name;
	q= n;
	while(*p!='\0') {
		if((*p>='A')&&(*p<='Z'))
			*q= *p+((char)'a')-((char)'A');
		else if((*p>='a')&&(*p<='z'))
			*q= *p;
		p++; q++;
		}
	*q= '\0';
	q= n;
	for(i=0;i<12;i++)
		if(scmp1(q,moname[i])==1)
			return(i+1);
	return(0);
}


char *month21(Int i)

/*
 *	return monthname given month number
 */

{
	if((i>0)&&(i<13))
		return(moname[i-1]);
	return(NULL);
}


Double intojul(Double t)

/*
 *	Jan 1, 4713 bc at 12 noon is 0
 *	1582 had only 355 days, leap centuries not divis by 400 skip
 *	Jan 1, 1980 12 noon is 2,444,240
 */

{
	return(t+2444240.0);
}


printdate(struct T2 *t2)

{
	printf("%d %s %d %d:%d:%.4f", t2->year,month21(t2->mno),
	  t2->day, t2->hr, t2->min, t2->sec);
}


Double jultoint(Double t)

{
	return(t-2444240.0);
}


intot2(Double t,struct T2 *t2,Int ep)

{
	Int i,j,k,n,*ip;
	Double x,y;

#ifdef DEBUG
	printf("intot2(%f)\n",t);
#endif
	t-= 1.0;
	i= ((Int)(t/1461.));
	t-= ((Double) i)*1461.0;
	i*= 4;
	k= (Int)t;
	ip= &dinyr[5*(ep%4)+1];
#ifdef DEBUG
	printf("intot2 ei: %d *ip: %d\n",i,*ip);
#endif
	for(j=0;j<4;j++)
		if(k<(*ip))
			break;
		else
			ip++;
	if(j>=4) {
		printf("error 1 in translating to dates\n");
		return(NULL);
		}
	i+= j;
	ip--;
	t-= ((Double) (*ip));
#ifdef DEBUG
	printf("ep: %d, j: %d, lpy[ep%4]: %d\n",ep,j,lpy[ep%4]);
#endif
	if(lpy[ep%4]==j)
		ip= &tmmodays[1];
	else
		ip= &tmodays[1];
	k= (Int)t;
	for(j=0;j<12;j++) {
#ifdef DEBUG
		printf("loop *ip: %d k: %d\n",*ip,k);
#endif
		if(k<(*ip))
			break;
		else
			ip++;
		}
	if(j>=12) {
		printf("error 2 in translating to dates\n");
		return(NULL);
		}
	ip--;
	t-= ((Double) (*ip));
#ifdef DEBUG
	printf("intot2 j: %d *ip: %d t: %f k: %d\n",j,*ip,t,k);
#endif
	t2->year= i+ep;
	t2->mno= j+1;
	k= (Int)t;
	t2->day= k+1;
	t-= ((Double) k);
	t*= 24.0;
	k= (Int)t;
	t-= ((Double) k);
	t2->hr= k;
	t*= 60.0;
	k= (Int)t;
	t-= ((Double) k);
	t2->min= k;
	t2->sec= t*60.0;
	return(1);
}


Double t2toint(struct T2 *t1,Int ep)

/*
 *	Transform fron t2 format to internal format
 */

{
	Int i,j,k;
	Double x,y;

	i= (t1->year)-ep;
	j= i*365;
	j+= i/4;	 /* leap days */
#ifdef DEBUG1
	printf("i: %d, j: %d\n",i,j);
#endif
	switch(ep%4) {
	  case 0:
		switch(i%4) {
		  case 0:
			if(t1->mno>2)
				j++;
			break;
		  case 1:
		  case 2:
		  case 3:
			j++;
			break;
		  }
		break;
	  case 1:
		switch(i%4) {
		  case 0:
		  case 1:
		  case 2:
			break;
		  case 3:
			if(t1->mno>2)
				j++;
			break;
		  }
		break;
	  case 2:
		switch(i%4) {
		  case 0:
		  case 1:
			break;
		  case 2:
			if(t1->mno>2)
				j++;
			break;
		  case 3:
			j++;
			break;
		  }
		break;
	  case 3:
		switch(i%4) {
		  case 0:
			break;
		  case 1:
			if(t1->mno>2)
				j++;
			break;
		  case 2:
		  case 3:
			j++;
			break;
		  }
		break;
	  }
	if((t1->mno>0)&&(t1->mno<13))
		j+= tmodays[t1->mno-1];
	/* j+= t1->day-1;  JLM */
	j+= t1->day;
	x= (Double) j;
	x+= (((Double) t1->hr)/24.0) + (((Double) t1->min)/1440.0) +
		((t1->sec)/86400.0);
	return(x);
}


Double gmttocivil(Double t,Double l)

/*
 *	longitude in degrees (East is postive, West is negative)
 */

{
	Int i;

	i= l/360.0;
	l-= ((Double) i)*360.0;
	if(l>180.0)
		l= 360.0-l;
	l-= 7.5;
	i= l/15.0;
	t+= ((Double) i)/24.0;
	return(t);
}


Double civiltogmt(Double t,Double l)

{
	Int i;

	i= l/360.0;
	l-= ((Double) i)*360.0;
	if(l>180.0)
		l= 360.0-l;
	l-= 7.5;
	i= l/15.0;
	t-= ((Double) i)/24.0;
	return(t);
}


Double gmttolocal(Double t,Double l)

{
	Int i;

	i= l/360.0;
	if(l<0.0)
		l-= (Double) ((i-1)*360);
	else
		l-= (Double) (i*360);
	l-= 180.0;
	/* now, -180<=l<=180 */
	l/= 15.0*24.0;
	return(t+l);
}


localtogmt(Double t,Double l)

{
	Int i;

	i= l/360.0;
	if(l<0.0)
		l-= (Double) ((i-1)*360);
	else
		l-= (Double) (i*360);
	l-= 180.0;
	/* now, -180<=l<=180 */
	l/= 15.0*24.0;
	return(t-l);
}


tomins(Double x,Int *ix,Double *xx)

{
	*ix= x;
	x-= (*ix);
	if(x<0.0)
		(*xx)= x*(-60.0);
	else
		(*xx)= x*(60.0);
}


/* ------------------------------------------------------------------ */


sptriang(Double A,Double b,Double c,Double *a)

/*
 *	cos(a)= cos(b)cos(c)+sin(b)sin(c)cos(A)
 */

{
	Double x;

	if(A<0.0)
		A*= (-1.0);
	x= cos(b)*cos(c)+sin(b)*sin(c)*cos(A);
	*a= acos(x);
	return(1);
}


tospherical(Double x,Double y,Double z,Double *a1,Double *a2)

/*
 *	given x,y,z in space, get azimuth (z axis to line) and angle in xy
 *	plane put the answer in a1, a2, answer is in radians
 */

{
	Double u,v,w;

#ifdef DEBUG7
	printf("tospherical(%g,%g,%g)\n", x,y,z);
#endif
	u= sqrt(x*x+y*y+z*z);
	if(u==0.0) {
		*a1= 0;
		*a2= 0;
		return(1);
		}
	x/= u;
	y/= u;
	z/= u;
	*a1= (v= acos(z));
	w= sin(v);
#ifdef DEBUG7
	printf("\nTosphere normal %7.2e %7.2e %7.2e v: %7.2f w: %7.2e\n",
		x,y,z,v*PI/180.,w);
#endif
	if(w==0.0) {
		*a1= 0;
		*a2= 0;
		return(1);
		}
	y/= w;
	x/= w;
	*a2= acos(x);
	if(y<0.0)
		*a2= 2.0*PI-(*a2);
	return(1);
}


eqtohor(Double t,Double par,Double r,Double d,
		Double la,Double lo,Double *al,Double *azz)

/*
 *	time since epoch (days), ra of Greenwich at epoch, ra, dec,
 *	latitude, longitude all in radians
 */

{
	Double x,y,z,u,lha;
	Int j;

#ifdef DEBUGE
	printf("eqtohor(%f,%f,%f,%f,%f,%f)\n",t,par,r,d,la,lo);
#endif
	u= t*W0;
	j= u;
	u-= ((Double) j);
	lha= r-par-2.0*PI*u-lo;
#ifdef DEBUGE
	printf("lha 1: %f, ",lha);
#endif
	j= lha/(2.0*PI);
	lha-= ((Double) j)*(2.0*PI);
	if(lha<0.0)
		lha+= 2.0*PI;
#ifdef DEBUGE
	printf("lha 2: %f\n",lha);
#endif
	x= sin(la)*sin(d)+cos(d)*cos(la)*cos(lha);
	y= asin(x);
	z= (sin(d)-sin(la)*x)/(cos(la)*sqrt(1.0-(x*x)));
	u= acos(z);
#ifdef DEBUGE
	printf("x: %f, y: %f, z: %f, u: %f\n",x,y,z,u);
#endif
	*al= y;
	if(sin(lha)<=0.0)
		*azz= u;
	else
		*azz= 2.0*PI-u;
#ifdef DEBUGE
	printf("(Alt, Azm): (%7.2f %7.2f)\n",*al,*azz);
#endif

	return(1);
}


degtora(Double x,Char *b)

{
	Int i,j;

	x/= 360.0;
	i= x;
	x-= i;
	x*= 24.0;
	i= x;
	x-= i;
	x*= 60.0;
	j= x;
	x-= j;
	x*= 60.0;
	sprintf(b,"%d hr %d min %5.2f sec",i,j,x);
	return(1);
}


degtominsec(Double x,Char *b)

{
	Int i,j;

	i= x;
	x-= i;
	x*= 60.0;
	j= x;
	x-= j;
	x*= 60.0;
	sprintf(b,"%d deg %d min %5.2f sec",i,j,x);
	return(1);
}


/* ------------------------------------------------------------------------- */


Double angles(Double x,Char *p,Char *q)

/*
 *	 to ra measure from radians
 */

{
	Int i,j;
	Double a,b;

	if(strcmp(p,"rad")==0) {
		i= x/(2.*PI);
		if(i!=0) {
			a= i;
			a*= 2.*PI;
			x-= a;
			}
		}
	else if(strcmp(p,"deg")==0) {
                i= x/360.;
                if(i!=0) {
			a= i;
                        a*= 360;
                        x-= a;
                        }
		x*= (PI/180.);
		}
	else if(strcmp(p,"ra")==0) {
                i= x/24.;
                if(i!=0) {
                        a*= 24;
                        x-= a;
                        }
		x*= (PI/12.);
		}
	if(strcmp(q,"deg")==0)
		return(x*180./PI);
	if(strcmp(q,"ra")==0)
		return(24.0*x/(2.0*PI));
	return(x);
}


rotate(Double x,Double y,Double z,
	   Double *x1,Double *y1,Double *z1,Char *axis,Double t)

/*
 *	rotate
 *	to change coordinates, use -t for t
 */

{
	if(strcmp(axis,"xy")==0) {
		*x1= x*cos(t)-y*sin(t);
		*y1= x*sin(t)+y*cos(t);
		*z1= z;
		}
	if(strcmp(axis,"yz")==0) {
		*y1= y*cos(t)-z*sin(t);
		*z1= y*sin(t)+z*cos(t);
		*x1= x;
		}
	if(strcmp(axis,"zx")==0) {
		*z1= z*cos(t)-x*sin(t);
		*x1= z*sin(t)+x*cos(t);
		*y1= y;
		}
#ifdef DEBUGROT
	printf("axis: %s, t: %f; (%g,%g,%g) --> (%g,%g,%g)\n",axis,t,x,y,z,
		*x1,*y1,*z1);
#endif
	return(1);
}


Double normalize(Double x)

{
	Int i;
	Double y;

#ifdef DEBUG1
	printf("normalize in: %f, ",x);
#endif
	if(x<0) {
		i= -(x/360.0);
		y= (i+1);
		y*= 360.0;
		x+= y;
		return(x);
		}
	i= x/360.0;
	y= i;
	y*= 360.0;
	x-= y;
#ifdef DEBUG1
	printf("normalize out: %f\n",x);
#endif
	return(x);
}


/* ------------------------------------------------------------------ */


#ifdef DEBUG2
main(argn,argv)

Int argn;
char *argv[];

{
	struct T2 t2, t3;
	Double x;

	epoch= atoi(argv[1]);
	t2.year= atoi(argv[2]);
	t2.mno= atoi(argv[3]);
	t2.day= atoi(argv[4]);
	t2.hr= 0;
	t2.min= 0;
	t2.sec= 0.0;

	printf("epoch: %d, year: %d, mno: %d day: %d\n",
		epoch,t2.year,t2.mno,t2.day);
	x= t2toint(&t2,epoch);
	printf("days: %f\n",x);
	intot2(x,&t3,epoch);
	printdate(&t3); printf("\n\n");
	exit();
}
#endif


/* ------------------------------------------------------------------ */


char *unknownloc={"Unknown"};

#define NLOCS 39

struct {
	char *l_name;
	Double l_lat, l_long;
	} locs[NLOCS] = {
		"Berkeley",37.87,-122.27,
		"San Francisco", 37.45, -122.33,
		"SF", 37.45, -122.33,
		"New York", 41.44, -73.8,
		"NY", 41.44, -73.8,
		"Albuquerque",35.08,-106.65,
		"Ann Arbor",42.27,-83.75,
		"Baltimore",39.28,-76.62,
		"Boston",42.35,-71.05,
		"Chicago",41.87,-87.63,
		"Dallas",32.78,-96.78,
		"Madison",43.07,-89.38,
		"Santa Fe",35.68,-105.93,
		"Seattle",47.61,-122.33,
		"Tucson",32.22,-110.97,
		"Washington",38.88,-77.0,
		"Yakima",46.6,-120.51,
		"Denver",39.75,-104.99,
		"Atlanta",33.75,-84.39,
		"Orlando",28.53,-81.38,
		"London",53.5,0.0,
		"Paris",48.83,2.3,
		"Berlin",52.5,13.42,
		"Rome",41.88,12.5,
		"Moscow",55.75,37.7,
		"Athens",37.97,23.75,
		"Jerusalem",31.75,35.22,
		"Tokyo",35.75,139.75,
		"Sydney",33.87,151.2,
		"Mana Kea",19.826,-155.47,
		"Lick",37.34,-121.64,
		"Mt Hopkins",31.69,-110.89,
		"Kitt Peak",31.96,-111.6,
		"Cerro Tololo",-70.82,-30.17,
		"Sacramento Peak",32.79,-105.82,
		"MacDonald",30.67,-104.02,
		"Palomar",33.36,-116.86,
		"Kyoto",35.36,135.79,
		"Bejing",40.1,116.33
		};



printlocs()

{
	Int i;

	for(i=0;i<NLOCS;i++) {
		if(strlen(locs[i].l_name)<8)
			printf("%s\t\t %8.4f deg Lat, %8.4f deg Long\n",
				locs[i].l_name,locs[i].l_lat,locs[i].l_long);
		else
			printf("%s\t %8.4f deg Lat, %8.4f deg Long\n",
				locs[i].l_name,locs[i].l_lat,locs[i].l_long);
		}
	return(1);
}


Int fetchpos(char **na,Double *la,Double *lo,char *arg[])

{
	char a;
	Int i;

	a= *arg[0];
	if(((a>='a')&&(a<='z'))||((a>='A')&&(a<='Z'))) {
		for(i=0;i<NLOCS;i++)
			if(strcmp(arg[0],locs[i].l_name)==0) {
				*na= locs[i].l_name;
				*la= locs[i].l_lat;
				*lo= locs[i].l_long;
				break;
				}
		if(i>=NLOCS) {
				*na= locs[0].l_name;
				*la= locs[0].l_lat;
				*lo= locs[0].l_long;
				}
		return(1);
		}
	else {
		*na= unknownloc;
		sscanf(arg[0],"%lf",&la);
		sscanf(arg[1],"%lf",&lo);
		return(2);
		}
}


/* ---------------------------------------------------------------------- */
