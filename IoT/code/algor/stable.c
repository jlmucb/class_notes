#include <stdio.h>


/*
 *	Stable marriage, male optimal solution
 */


/*
 *	Data structures:
 *		n: number of couples
 *		ip: pointer to 2*n*n integers,
 *		    first n are ratings of boy 1,
 *		    second n are the ratings of boy 2,
 *				... ...
 *		    nth n are the ratings of boy n,
 *		followed by the same for the girls
 *		ap: pointer to n pairs of integers
 *		    (a,b) meaning boy n is assigned to girl b
 *		    and girl n is assigned to boy b
 *		ep: pointer to n*n integers (boy/girl eligible/ineligible)
 *
 */


int *midpointer(base,dsize,nelem)

int *base,dsize,nelem;

{
	return((int *) (((unsigned) base)+(dsize*(nelem-1))));
}


prefer(n,w,m1,m2,ip)

int n,w,m1,m2,*ip;

{
	int i,*kp;

	kp= midpointer(ip,n*sizeof(int),n+w);
	for(i=0;i<n;i++) {
		if(*kp==m1)
			return(1);
		if(*kp==m2)
			return(0);
		kp++;
		}
	return(0);
}


eligible(n,m,w,ep)

int n,m,w,*ep;

/*
 *	is w eligible to m and m eligible to w
 */

{
	int *kp;

	kp= midpointer(ep,n*sizeof(int),m);
	kp= midpointer(kp,sizeof(int),w);
	if(*kp==1)
		return(0);
	kp= midpointer(ep,n*sizeof(int),n+w);
	kp= midpointer(kp,sizeof(int),m);
	if(*kp==1)
		return(0);
	return(1);
}


cancel(n,m,w,ep)

int n,m,w,*ep;

/*
 *	make m ineligible to w list and w ineligible to m
 */

{
	int *kp;

	kp= midpointer(ep,n*sizeof(int),m);
	kp= midpointer(kp,sizeof(int),w);
	*kp= 1;
	kp= midpointer(ep,n*sizeof(int),n+w);
	kp= midpointer(kp,sizeof(int),m);
	*kp= 1;
	return;
}


accept(n,m,w,ap)

int n,m,w,*ap;

/*
 *	accept m as w's partner and vice versa
 */

{
	int *kp;

	kp= midpointer(ap,2*sizeof(int),m);
	*kp= w;
	kp= midpointer(ap,2*sizeof(int),w);
	*(++kp)= m;
	return;
}


propose(n,m,ip,ap,ep)

int n,m,*ip,*ap;

{
	int i,w,*jp;

	/*
	 *	get the first uncancelled woman on m's list
	 */
	jp= midpointer(ip,n*sizeof(int),m);	/* list of preferences */
	for(i=0;i<n;i++) {
		w= *(jp++);
		if(eligible(n,m,w,ep)==1)
			break;
		}
	if(i>=n)
		return(-1);
	refuse(n,m,w,ip,ap,ep);
	return;
}


refuse(n,m,w,ip,ap,ep)

int n,m,w,*ip,*ap,*ep;

{
	int m1, w1, *kp;

	/* let m1 be w's current partner */
	kp= midpointer(ap,2*sizeof(int),w);
	m1= *(++kp);
	if(m1==0) {				/* w has no current partner */
		accept(n,m,w,ip);
		}
	else if(prefer(n,w,m1,m,ip)==1) {		/* w prefers m1 to m? */
		cancel(n,m,w,ep);
		propose(n,m,ip,ap,ep);
		}
	else {					/* no, w prefers m */
		accept(n,m,w,ip);
		cancel(n,m1,w,ep);
		propose(n,m1,ip,ap,ep);
		}
	return;
}


assignall(n,ip,ap)

int n, *ip, *ap;

{
	int *ep;
	register int i,j,*kp;

	ep= (int *) malloc(n*n*sizeof(int));
	if(ep==NULL) {
		printf("Assignall: cant malloc eligible list\n");
		return(-1);
		}

	kp= ep;
	for(i=0;i<n;i++)
	    for(j=0;j<n;j++)
		*(kp++)= 0;
	
	for(i=0;i<n;i++)
		propose(n,i,ip,ap,ep);
	return;
}


