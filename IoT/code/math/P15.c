#include <stdio.h>


int dypath(int,int,int [],int [],int (*)(int,int));


/*
 *		John Manferdelli, 21/07/93
 */


int perm(int n,int a[],int b[],int p[])

{
	int i;

	for(i=0;i<n;i++)
		b[i]= a[p[i]];

	return(n);
}


int tranpose(int a[],int b[],int n)

{
	int i,j;

	for(i=0;i<n;i++) {
		j= a[i];
		a[i]= b[i];
		b[i]=  j;
		}
	return(1);
}


/*
 *		numbering               desired position
 *			0123                     1234
 *			4567                     5678
 *			89ab                     9abc
 *			cdef                     def0
 */


gettarget(int a[])

{
	int i,j;

	for(i=0;i<16;i++)
		if(a[i]!=(i+1))
			return(i);
}


getspace(int a[])

{
	int i,j;

	for(i=0;i<16;i++)
		if(a[i]==0)
			return(i);
}



main(argn,argv)


int argn;
char *argv[];

/*
 *			15 puzzle
 */

{
	int i,j,t,o;
	char a[16],b[16],c[16], *q;

	q= argv[1];
	for(i=0;i<16;i++) {
		c[0]= *(q++); c[1]= '\0';
		sscanf(c,"%x",&a[i]);
		}

	printf("Initial position:\n");
	for(i=0;i<16;i++) {
		if((i%4)==0)
			printf("\n\t\t");
		printf(" %2d",a[i]);
		}
	printf("\n\n");

	/*
	 *	get target position
	 *	get occupant site
	 *		get allowable transforms
	 *		if not next to last position, be greedy
	 *		otherwise  put successor target in place
	 *	find adjacency graph and shortest path graph
	 *	apply transformations to finalize position
	 *
	 *	Step A: find unrestricted square closer to target than present location.
	 *	Step B: move square to urs via urs
	 *	Step C: xchange
	 */

	exit(1);
}