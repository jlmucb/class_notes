#include <stdio.h>
#include <math.h>


/*
 *	(c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */

/*
 *	Simulate a system of NBODIES under (at worst) a central force
 *	field (usually gravity).  Collisions are assumed to be elastic
 *	which is not exactly right for a collision of stars.
 */


/* ---------------------------------------------------------------------- */


#define NBODIES 100
#define NBUF 256


char buf[NBUF];


struct body {
	char name[28];			/* name of body */
	double px,py,pz;		/* position */
	double vx,vy,vz;		/* velocity */
	double radius,mass;		/* guess */
	int hit;			/* last collision status */
	};

int nbod={0};				/* number of bodies */
char posunit[10],radunit[10],
     timeunit[10], massunit[10];
double deltat;				/* simulation time step size */
int graphics;				/* display? */
double tottime;				/* end time of simulation */
int nss;				/* number of simulation steps */
struct body bodies[NBODIES];		/* body centered data */

double G={6.67e-11};			/* gravitational constant */

int np={1};				/* number of steps before printing */

int walls={0};				/* if walls present */
double lxwall,rxwall,lywall,rywall,
	lzwall,rzwall;			/* walls, if applicable */


/* ---------------------------------------------------------------------- */


readline(in,q,n)

int in,n;
char *q;

/*
 *	read input line, get rid of comments, blank lines etc
 *	if in is input file, q is buffer, n is max buffer size;
 *	comment character is #
 */

{
	int nc;
	char *p,a;

	p= q;
	nc= 0;
	for(;;) {
		if(nc>=n) {
			while((read(in,&a,1)>0)&&(a!='\n'));
			*(--p)= '\0';
			printf("line starting with following text is too long %s\n",
				q);
			*q= '\n';
			nc--;
			return(nc);
			}
		if(read(in,&a,1)<=0)
			return(-1);
		if(a=='#') {	/* get rid of comment */
			while((read(in,&a,1)>0)&&(a!='\n'));
			a= '\n';
			}
		*(p++)= a;
		if(a=='\n')
			break;
		nc++;
		}
	*p= '\0';
	if(nc>0)
		return(nc);
	else
		return(readline(in,q,n));
}


init(s)

char *s;

/*
 *	Syntax of input file:
 *
 *	time-unit
 *	position-unit
 *	radius-unit 
 *	mass-unit
 *	number of bodies
 *	deltat
 *	total time
 *	display flag
 *	body data consisting of:
 *		name
 *		x y z  (position)
 *		vx vy vz (velocity)
 *		radius mass
 */

{
	int i,j,in;

	if((in=open(s,0))<=0) {
		printf("cannot open %s, quitting\n",s);
		exit();
		}
	/* time-unit */
	readline(in,buf,NBUF);
	sscanf(buf,"%s\n",timeunit);
	/* position-unit */
	readline(in,buf,NBUF);
	sscanf(buf,"%s\n",posunit);
	/* radius-unit */
	readline(in,buf,NBUF);
	sscanf(buf,"%s\n",radunit);
	/* mass-unit */
	readline(in,buf,NBUF);
	sscanf(buf,"%s\n",massunit);
 	/* number of bodies */
	readline(in,buf,NBUF);
	sscanf(buf,"%d\n",&nbod);
 	/* deltat */
	readline(in,buf,NBUF);
	sscanf(buf,"%lf\n",&deltat);
 	/* total time */
	readline(in,buf,NBUF);
	sscanf(buf,"%lf\n",&tottime);
 	/* display flag */
	readline(in,buf,NBUF);
	sscanf(buf,"%d\n",&graphics);
 	/* body data consisting of: */
	i= 0;
	while((readline(in,buf,NBUF)>0)&&(i<nbod)) {
 		/* name */
		sscanf(buf,"%s\n",bodies[i].name);
 		/* x y z  (position) */
		readline(in,buf,NBUF);
		sscanf(buf,"%lf %lf %lf\n",&(bodies[i].px),&(bodies[i].py),
			   &(bodies[i].pz));
 		/* vx vy vz (velocity) */
		readline(in,buf,NBUF);
		sscanf(buf,"%lf %lf %lf\n",&(bodies[i].vx),&(bodies[i].vy),
			   &(bodies[i].vz));
 		/* radius mass */
		readline(in,buf,NBUF);
		sscanf(buf,"%lf %lf\n",&(bodies[i].radius),&(bodies[i].mass));
		bodies[i].hit= 0;
		i++;
		}
	nss= tottime/deltat;
	close(in);
	return(1);
}


report(n)

int n;

{
	int i;

	printf("\n%d bodies under gravitational force for %lf seconds\n",
		nbod,tottime);
	printf("\tTime step %d of %d, step unit: %lf seconds\n",n,nss,deltat);
	for(i=0;i<nbod;i++)
		printbodydata(&bodies[i]);
}



/* ---------------------------------------------------------------------- */


main(argn,argv)

int argn;
char *argv[];

/*
 *	n bodies interacting via a central force
 */

{
	int i;

	if(argn<2) {
		printf("syntax: interact filename\n");
		exit();
		}
	init(argv[1]);
	printf("%d bodies under gravitational force for %lf seconds\n",
		nbod,tottime);
	printf("\tNumber of steps: %d, step unit: %lf seconds\n",nss,deltat);
	printf("\nInitial state:\n");
	for(i=0;i<nbod;i++) {
		printbodydata(&bodies[i]);
		}
	displaybodies();
	for(i=0;i<nss;i++) {
		nextstate(i);
		if((i%np)==0) {
			report(i);
			displaybodies();
			}
		}
	printf("program done\n");
	exit();
}


/* ---------------------------------------------------------------------- */


displaybodies()

{
}


printbodydata(b)

struct body *b;

{
	if(b->name!=NULL)
		printf("%s: ",&(b->name[0]));
	if(b->hit>0)
		printf("Mass: %lf %s, Radius: %lf %s, hit: %d\n",b->mass,&massunit[0],
			b->radius,&radunit[0],b->hit-1);
	else
		printf("Mass: %lf %s, Radius: %lf %s, no hit\n",b->mass,&massunit[0],
			b->radius,&radunit[0]);
	printf("\tPosition (%lg,%lg,%lg)-%s\n\tVelocity (%lg,%lg,%lg)-%s/%s,\n",
		b->px,b->py,b->pz,&posunit[0],
		b->vx,b->vy,b->vz,&posunit[0],&timeunit[0]);
	return;
}


/*
 *	Collision equations:
 *
 *	Conservation of Momentum
 *	    m1(vx1)+m2(vx2)=m1(vx1p)+m2(vx2p)
 *	    m1(vy1)+m2(vy2)=m1(vy1p)+m2(vy2p)
 *	    m1(vz1)+m2(vz2)=m1(vz1p)+m2(vz2p)
 *
 *	Conservation of Energy
 *	    m1(vx1*vx1+vy1*vy1+vz1*vz1)+m2(vx2*vx2+vy2*vy2+vz2*vz2)=
 *	     m1(vx1p*vx1p+vy1p*vy1p+vz1p*vz1p)+m2(vx2p*vx2p+vy2p*vy2p+vz2p*vz2p)
 *
 *	Collision force is along line joining body centers
 *	so velocity components are unchanged in plane normal to that line
 *	let N= ((x1-x2),(y1-y2),(z1-z2)) and n= N/|N|
 *	    vx1-(vx1 dot n)n = vx1p-(vx1p dot n)n
 *	    vx2-(vx2 dot n)n = vx2p-(vx2p dot n)n
 */


/*
 *	Collision matrix
 *
 *	cm*(transpose(vx1p,vy1p,vz1p,v2xp,v2yp,v2zp,-1))=0
 */
double cm[9*7];		/* collision matrix */


collision(x1,y1,z1,vx1,vy1,vz1,r1,m1,x2,y2,z2,vx2,vy2,vz2,r2,m2,
	  x1p,y1p,z1p,vx1p,vy1p,vz1p,x2p,y2p,z2p,vx2p,vy2p,vz2p)

double x1,y1,z1,vx1,vy1,vz1,r1,m1;
double x2,y2,z2,vx2,vy2,vz2,r2,m2;
double *x1p,*y1p,*z1p,*vx1p,*vy1p,*vz1p;
double *x2p,*y2p,*z2p,*vx2p,*vy2p,*vz2p;

/*
 *	result of collision
 */

{
	int i,j,m,n,ls;
	double d1,d2,d3,x,y;
	double n1,n2,n3;
	double *fp,*gp;
	double ans[12],bans[6],cans[6];
	double energy,a,b,c;
	double roots[2];

#ifdef DEBUG
	printf("In collision\n");
	printf("x1: %lf, y1: %lf, z1: %lf, vx1: %lf, vy1: %lf, vz1: %lf, r1: %lf, m1: %lf\n",
		x1,y1,z1,vx1,vy1,vz1,r1,m1);
	printf("x2: %lf, y2: %lf, z2: %lf, vx2: %lf, vy2: %lf, vz2: %lf, r2: %lf, m2: %lf\n",
		x2,y2,z2,vx2,vy2,vz2,r2,m2);
#endif
	d1= x1-x2;
	d2= y1-y2;	
	d3= z1-z2;
	x= (d1*d1)+(d2*d2)+(d3+d3);
	if(x>((r1+r2)*(r1+r2)))
		return(0);
	if(x<1.0e-15) {
		/* too close back off to get line of action */
		printf("too close for collision\n");
		return(0);
		}

#ifdef DEBUG
	printf("d1: %lf, d2: %lf, d3: %lf, x: %lf\n",d1,d2,d3,x);
#endif
	x= sqrt(x);
	n1= d1/x;
	n2= d2/x;
	n3= d3/x;
	fp= &cm[0];

	/* CM equations */
	*(fp++)= m1; *(fp++)= 0.0; *(fp++)= 0.0;
	*(fp++)= m2; *(fp++)= 0.0; *(fp++)= 0.0;
	*(fp++)= m1*vx1+m2*vx2;
	*(fp++)= 0.0; *(fp++)= m1; *(fp++)= 0.0;
	*(fp++)= 0.0; *(fp++)= m2; *(fp++)= 0.0;
	*(fp++)= m1*vy1+m2*vy2;
	*(fp++)= 0.0; *(fp++)= 0.0; *(fp++)= m1;
	*(fp++)= 0.0; *(fp++)= 0.0; *(fp++)= m2;
	*(fp++)= m1*vz1+m2*vz2;

#ifdef DEBUG
	printf("n1: %lf, n2: %lf, n3: %lf\n",n1,n2,n3);
#endif
	/* v1p dot (v1 cross n) = 0 */
	/* new velocity is in plane of old vel and line of action */
	y= n1*vx1+n2*vy1+n3*vz1;
	*(fp++)= n3*vy1-n1*vz1;
	*(fp++)= n1*vz1-n3*vx1;
	*(fp++)= n2*vx1-n1*vy1;
	*(fp++)= 0.0;
	*(fp++)= 0.0;
	*(fp++)= 0.0;
	*(fp++)= 0.0;

	/* v1p dot (v1 cross n) = 0 */
	y= n1*vx2+n2*vy2+n3*vz2;
	*(fp++)= 0.0;
	*(fp++)= 0.0;
	*(fp++)= 0.0;
	*(fp++)= n3*vy2-n1*vz2;
	*(fp++)= n1*vz2-n3*vx2;
	*(fp++)= n2*vx2-n1*vy2;
	*(fp++)= 0.0;

	/* CE */
	energy= m1*(vx1*vx1+vy1*vy1+vz1*vz1)+m2*(vx2*vx2+vy2*vy2+vz2*vz2);
#ifdef DEBUG
	printf("2*energy: %lf\n",energy);
#endif

	m= 9;
	n= 7;
#ifdef DEBUG
	printf("\noriginal collision matrix: \n");
	printmat(&cm[0],6,7);
#endif
	ls= (m<=(n-1))?m:(n-1);
	for(i=0;i<ls;i++) {
		if(pivot(&cm[0],m,n,i,i,0.01)<0) {
			m= i;
#ifdef DEBUG
			printf("\npivot failed, final form: \n");
			printmat(&cm[0],m,n);
#endif
			break;
			}
#ifdef DEBUG
		printf("\nafter pivot %d\n",i);
		printmat(&cm[0],m,n);
#endif
		}
	ls= (m<ls)?m:ls;
	if(ls==(n-1)) {
		evalle(&cm[0],ls,&ans[0]);
#ifdef DEBUG
		printf("\nevalle: \n");
		for(i=0;i<6;i++)
			printf("%lf, ",ans[i]);
		printf("\n");
#endif
		*vx1p= ans[0];
		*vy1p= ans[1];
		*vz1p= ans[2];
		*x1p= x1+deltat*(*vx1p);
		*y1p= y1+deltat*(*vy1p);
		*z1p= z1+deltat*(*vz1p);
		*vx2p= ans[3];
		*vy2p= ans[4];
		*vz2p= ans[5];
		*x2p= x2+deltat*(*vx2p);
		*y2p= y2+deltat*(*vy2p);
		*z2p= z2+deltat*(*vz2p);
		return(1);
		}
	else if(ls==(n-2)) {
		linsubs(&cm[0],&ans[0],5,7,1);
#ifdef DEBUG
		printf("after linsubs\n");
		for(i=0;i<6;i++) {
			printf("%d, %f %f\n",i,ans[2*i],ans[2*i+1]);
			}
#endif
		a= m2*(1.0+ans[8]*ans[8]+ans[6]*ans[6]);
		a+= m1*(ans[4]*ans[4]+ans[2]*ans[2]+ans[0]*ans[0]);
		b= 2.0*m2*(ans[9]*ans[8]+ans[7]*ans[6]);
		b+= 2.0*m1*(ans[5]*ans[4]+ans[3]*ans[2]+ans[1]*ans[0]);
		c= m2*(ans[9]*ans[9]+ans[7]*ans[7]);
		c+= m1*(ans[5]*ans[5]+ans[3]*ans[3]+ans[1]*ans[1]);
		c-= energy;
#ifdef DEBUG
		printf("Quadratic coefficients: a: %f, b: %f, c: %f\n",
			a,b,c);
#endif
		d1= b*b-4.0*a*c;
		if(a==0.0) {
			if(b==0.0) {
				printf("infinite recoil\n");
				return(0);
				}
			printf("linear root: %f\n",d1);
			i= 0;
			roots[0]= -c/b;
			}
		else if(d1<0.0) {
			printf("Imaginary recoil velocity disc: %f\n",d1);
			return(0);
			}
		else if(d1==0.0) {
			printf("single root: %f\n",d1);
			i= 1;
			roots[0]= -b/(2.0*a);
			}
		else {
			d2= -b/(2.0*a);
			d1= sqrt(d1);
			d1/= 2.0*a;
			i= 2;
			roots[0]= d2+d1;
			roots[1]= d2-d1;
#ifdef DEBUG
			printf("two roots: %f %f\n",roots[0],roots[1]);
#endif
			}
		cm[41]= roots[0];
		cm[40]= 1.0;
		evalle(&cm[0],6,&bans[0]);
		if(i==2) {
			cm[41]= roots[1];
			cm[40]= 1.0;
			evalle(&cm[0],6,&cans[0]);
			j= selectans(x1,y1,z1,vx1,vy1,vz1,x2,y2,z2,vx2,vy2,vz2,
			     bans[0],bans[1],bans[2],bans[3],bans[4],bans[5],
			     cans[0],cans[1],cans[2],cans[3],cans[4],cans[5]);
			}
		else
			j= 0;
#ifdef DEBUG
		printf("\nevalle: \n");
		for(i=0;i<6;i++)
			printf("%lf, ",bans[i]);
		printf("\n");
		printf("2*energy1(after): %lf\n",
			m1*(bans[0]*bans[0]+bans[1]*bans[1]+bans[2]*bans[2])+
			m2*(bans[3]*bans[3]+bans[4]*bans[4]+bans[5]*bans[5]));
		printf("2*energy1(after): %lf\n",
			m1*(cans[0]*cans[0]+cans[1]*cans[1]+cans[2]*cans[2])+
			m2*(cans[3]*cans[3]+cans[4]*cans[4]+cans[5]*cans[5]));
#endif
		if(j==1) {
			*vx1p= cans[0];
			*vy1p= cans[1];
			*vz1p= cans[2];
			}
		else {
			*vx1p= bans[0];
			*vy1p= bans[1];
			*vz1p= bans[2];
			}
		*x1p= x1+deltat*(*vx1p);
		*y1p= y1+deltat*(*vy1p);
		*z1p= z1+deltat*(*vz1p);
		if(j==1) {
			*vx2p= cans[3];
			*vy2p= cans[4];
			*vz2p= cans[5];
			}
		else {
			*vx2p= bans[3];
			*vy2p= bans[4];
			*vz2p= bans[5];
			}
		*x2p= x2+deltat*(*vx2p);
		*y2p= y2+deltat*(*vy2p);
		*z2p= z2+deltat*(*vz2p);
		return(1);
		}

	else {
		printf("underdetermined: n: %d, ls: %d\n",n,ls);
		return(0);
		}
}


selectans(x1,y1,z1,vx1,vy1,vz1,x2,y2,z2,vx2,vy2,vz2,
	  ux1,uy1,uz1,ux2,uy2,uz2,wx1,wy1,wz1,wx2,wy2,wz2)

double x1,y1,z1,vx1,vy1,vz1,x2,y2,z2,vx2,vy2,vz2,
          ux1,uy1,uz1,ux2,uy2,uz2,wx1,wy1,wz1,wx2,wy2,wz2;

/*
 *	pick the solution
 */

{
	printf("Collision velocities (1): (%.4lf,%.4lf,%.4lf), (%.4lf,%.4lf,%.4lf)\n",
		ux1,uy1,uz1,ux2,uy2,uz2);
	printf("Collision velocities (2): (%.4lf,%l.4f,%lf), (%.4lf,%.4lf,%.4lf)\n",
		wx1,wy1,wz1,wx2,wy2,wz2);
	if((fabs(vx1-ux1)<0.001)&&(fabs(vy1-uy1)<0.001)&&
	   (fabs(vz1-uz1)<0.001)) {
		printf("selected 2\n");
		return(1);
		}
	printf("selected 1\n");
	return(0);
}


bounce(x,y,z,nx,ny,nz,xv,yv,zv,nxv,nyv,nzv)

double x,y,z,*nx,*ny,*nz,xv,yv,zv,*nxv,*nyv,*nzv;

{
	if(walls==0)
		return(0);
	if(x>rxwall) {
		*nx= rxwall+rxwall-x;
		*ny= y;
		*nz= z;
		*nxv= -xv;
		*nyv= yv;
		*nzv= zv;
		return(1);
		}
	if(x<lxwall) {
		*nx= lxwall+lxwall-x;
		*ny= y;
		*nz= z;
		*nxv= -xv;
		*nyv= yv;
		*nzv= zv;
		return(1);
		}
	if(y>rywall) {
		*ny= rywall+rywall-y;
		*nx= x;
		*nz= z;
		*nxv= xv;
		*nyv= -yv;
		*nzv= zv;
		return(1);
		}
	if(y<lywall) {
		*ny= lywall+lywall-y;
		*nx= x;
		*nz= z;
		*nxv= xv;
		*nyv= -yv;
		*nzv= zv;
		return(1);
		}
	if(z>rzwall) {
		*nz= rzwall+rzwall-z;
		*nx= x;
		*ny= y;
		*nxv= xv;
		*nyv= yv;
		*nzv= -zv;
		return(1);
		}
	if(z<lzwall) {
		*nz= lzwall+lzwall-z;
		*nx= x;
		*ny= y;
		*nxv= xv;
		*nyv= yv;
		*nzv= -zv;
		return(1);
		}
	return(0);
}


newcoord(k,x,y,z,xv,yv,zv)

int k;				/* body number */
double *x,*y,*z,		/* new position */
       *xv,*yv,*zv;		/* new velocity */

/*
 *	calculate new state vectors
 *	f is the force function per unit mass
 */

{
	int i;
	double bx,by,bz,ax,ay,az;
	struct body *b1, *b2;

#ifdef DEBUG
	printf("newcoord(%d)\n",k);
#endif
	ax= 0.0;
	ay= 0.0;
	az= 0.0;
	b1= &bodies[k];
	for(i=0;(i<nbod);i++) {
		if(i==k)
			continue;
		b2= &bodies[i];
		f(G,b1->px,b1->py,b1->pz,b2->px,b2->py,b2->pz,&bx,&by,&bz);
		ax+= bx;
		ay+= by;
		az+= bz;
		}
	*xv= b1->vx+deltat*ax;
	*yv= b1->vy+deltat*ay;
	*zv= b1->vz+deltat*az;
	*x= b1->px +((b1->vx+(*xv))/2.0)*deltat;
	*y= b1->py +((b1->vy+(*yv))/2.0)*deltat;
	*z= b1->pz +((b1->vz+(*zv))/2.0)*deltat;
	return;
}


f(G,x1,y1,z1,x2,y2,z2,ax,ay,az)

double G,x1,y1,z1,x2,y2,z2,*ax,*ay,*az;

{
	double x,y,z,d,d3;
	double sqrt();

	*ax= 0.0;
	*ay= 0.0;
	*az= 0.0;
	return;
#ifdef DEBUG
	printf("calculating f: %.3e (%.3e,%.3e,%.3e) (%.3e,%.3e,%.3e)\n",G,x1,y1,z1,x2,y2,z2);
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
	*ax= G*x/d3;
	*ay= G*y/d3;
	*az= G*z/d3;
#ifdef DEBUG
	printf("calculating f done: (%.3e,%.3e,%.3e)\n",*ax,*ay,*az);
#endif
}


double tmpx[NBODIES], tmpy[NBODIES], tmpz[NBODIES],
       tmpvx[NBODIES], tmpvy[NBODIES], tmpvz[NBODIES];


nextstate(i)

int i;

{
	int j,k;
	double m1,m2,r1,r2;
	double tx,ty,tz;
	double tvx,tvy,tvz;
	double txp,typ,tzp;
	double tvxp,tvyp,tvzp;
	struct body *b;

#ifdef DEBUG
	printf("nextstate(%d)\n",i);
#endif
	for(j=0;j<nbod;j++) {
		b= &bodies[j];
		r1= b->radius;
		m1= b->mass;
		newcoord(j,&tmpx[j],&tmpy[j],&tmpz[j],
			   &tmpvx[j],&tmpvy[j],&tmpvz[j]);
		/* bounce? */
		if(bounce(tmpx[j],tmpy[j],tmpz[j],&tx,&ty,&tz,
			  tmpvx[j],tmpvy[j],tmpvz[j],&tvx,&tvy,&tvz)>0) {
			printf("body %d, hit a wall\n",j);
			tmpx[j]= tx;
			tmpy[j]= ty;
			tmpz[j]= tz;
			tmpvx[j]= tvx;
			tmpvy[j]= tvy;
			tmpvz[j]= tvz;
			}
		/* collision ? */
		for(k=0;k<j;k++) {
			b= &bodies[k];
			r2= b->radius;
			m2= b->mass;
			if(collision(tmpx[j],tmpy[j],tmpz[j],
			   	     tmpvx[j],tmpvy[j],tmpvz[j],r1,m1,
				     tmpx[k],tmpy[k],tmpz[k],
			   	     tmpvx[k],tmpvy[k],tmpvz[k],r2,m2,
				     &tx,&ty,&tz,&tvx,&tvy,&tvz,
				     &txp,&typ,&tzp,&tvxp,&tvyp,&tvzp)>0) {
				if((bodies[j].hit!=(k+1))&&(bodies[k].hit!=(j+1))) {
					printf("body %d hit body %d\n",j,k);
					bodies[j].hit= k+1;
					bodies[k].hit= j+1;
					tmpx[j]= tx;
					tmpy[j]= ty;
					tmpz[j]= tz;
					tmpvx[j]= tvx;
					tmpvy[j]= tvy;
					tmpvz[j]= tvz;
					tmpx[k]= txp;
					tmpy[k]= typ;
					tmpz[k]= tzp;
					tmpvx[k]= tvxp;
					tmpvy[k]= tvyp;
					tmpvz[k]= tvzp;
					}
				}
			else {
				bodies[j].hit= 0;
				bodies[k].hit= 0;
				}
			}
		}
	for(j=0;j<nbod;j++) {
		b= &bodies[j];
		b->px= tmpx[j];
		b->py= tmpy[j];
		b->pz= tmpz[j];
		b->vx= tmpvx[j];
		b->vy= tmpvy[j];
		b->vz= tmpvz[j];
		}
	return(1);
}


/* ---------------------------------------------------------------------- */


