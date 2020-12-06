#include <stdio.h>
#include <math.h>

/*
 *	(c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */


#define SM 111.3171 /* km */
#define SMM 69.169 /* miles */
#define PI 3.1415926536


main(argn,argv)

int argn;
char *argv[];

{
	char *name1, *name2;
	int i,j,k,n;
	double lt1,lg1,lt2,lg2; 
	double dlt1,dlg1,dlt2,dlg2; 
	double dr,dd,dm,dkm;
	double x,y;
	double x1,x2,x3,x4,x5;

	if(argn==9) {
		name1= "";
		name2= "";
		i= atoi(argv[1]);
		j= atoi(argv[2]);
		k= atoi(argv[3]);
		n= atoi(argv[4]);
		if(i<0)
			j*= (-1);
		if(k<0)
			n*= (-1);
		dlt1= (double)j;
		dlt1/= 60.;
		dlt1+= (double) i;
		dlg1= (double)n;
		dlg1/= 60.;
		dlg1+= (double) k;
		i= atoi(argv[5]);
		j= atoi(argv[6]);
		k= atoi(argv[7]);
		n= atoi(argv[8]);
		if(i<0)
			j*= (-1);
		if(k<0)
			n*= (-1);
		dlt2= (double)j;
		dlt2/= 60.;
		dlt2+= (double) i;
		dlg2= (double)n;
		dlg2/= 60.;
		dlg2+= (double) k;
		}
	else if(argn==5) {
		name1= "";
		name2= "";
		sscanf(argv[1],"%F",&dlt1);
		sscanf(argv[2],"%F",&dlg1);
		sscanf(argv[3],"%F",&dlt2);
		sscanf(argv[4],"%F",&dlg2);
		}
	else if(argn==7) {
		name1= argv[1];
		name2= argv[4];
		sscanf(argv[2],"%F",&dlt1);
		sscanf(argv[3],"%F",&dlg1);
		sscanf(argv[5],"%F",&dlt2);
		sscanf(argv[6],"%F",&dlg2);
		}
	else if(argn==11) {
		name1= argv[1];
		name2= argv[6];
		i= atoi(argv[2]);
		j= atoi(argv[3]);
		k= atoi(argv[4]);
		n= atoi(argv[5]);
		if(i<0)
			j*= (-1);
		if(k<0)
			n*= (-1);
		dlt1= (double)j;
		dlt1/= 60.;
		dlt1+= (double) i;
		dlg1= (double)n;
		dlg1/= 60.;
		dlg1+= (double) k;
		if(i<0)
			j*= (-1);
		if(k<0)
			n*= (-1);
		i= atoi(argv[7]);
		j= atoi(argv[8]);
		k= atoi(argv[9]);
		n= atoi(argv[10]);
		dlt2= (double)j;
		dlt2/= 60.;
		dlt2+= (double) i;
		dlg2= (double)n;
		dlg2/= 60.;
		dlg2+= (double) k;
		}
	else {
		printf("Wrong number of arguments %d\n",argn);
		printf("name1 lat (degrees minutes) long (degrees minutes)\n");
		exit();
		}

	printf("%s (%7.2f,%7.2f) and %s (%7.2f,%7.2f)\n",name1,dlt1,dlg1,name2,dlt2,dlg2);
	lt1= dlt1*(PI/180.);
	lg1= dlg1*(PI/180.);
	lt2= dlt2*(PI/180.);
	lg2= dlg2*(PI/180.);
	y= fabs(lg1-lg2);
	x1= cos(lt1);
	x2= cos(lt2);
	x3= cos(y);
	x4= sin(lt1);
	x5= sin(lt2);
#ifdef DEBUG
	printf("TRIGS: %f %f %f %f %f\n",x1,x2,x3,x4,x5);
#endif
	x= x1*x2*x3+x4*x5;
	dr= acos(x);
	dd= dr*(180./PI);
	dm= SMM*dd;
	dkm= SM*dd;
	printf("\t%7.2f degrees, %8.2f miles apart\n",dd,dm);
	exit();
}
