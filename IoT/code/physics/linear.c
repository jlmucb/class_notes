
/*
 *	(c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */


pivot(fp,m,n,i,j,zt)

double *fp;
int i,j,m,n;
double zt;

/*
 *	in f.p. array of m rows and n cols pivot on row i, col j
 *	as in Gaussian elimination
 */

{
	int k,l;
	double *d1,*d2,*d3,x;

#ifdef DEBUG
	printf("pivot(%x,%d,%d,%d,%d,%f)\n",fp,m,n,i,j,zt);
#endif
	for(k=i;k<m;k++) {
		/* get non zero pivot */
		d1= (double *) (((unsigned) fp)+(k*n+j)*(sizeof(double)));
		x= *d1;
		if(x<0.0)
		x= -x;
		if(x>zt)
			break;
		}
	if(k>=m)
		return(-1);
	if(k!=i)
		switchrow(fp,m,n,k,i);
	d1= (double *) (((unsigned) fp)+(i*n+j)*(sizeof(double)));
	d3= d1;
	x= *d1;
	for(k=j;k<n;k++)
		*(d1++)/= x;
	for(k=(i+1);k<m;k++) {
		d2= (double *) (((unsigned) fp)+(k*n+j)*(sizeof(double)));
		x= *d2;
		if(x==0.0)
			continue;
		d1= d3;
		for(l=j;l<n;l++) {
			*d2-= (*d1)*x;
			d1++; d2++;
			}
		}
	return(1);
}


switchrow(fp,m,n,i,j)

double *fp;
int i,j,m,n;

{
	double *d1, *d2, x;
	int k;

	if((i==j)||(i<0)||(j<0)||(i>=m)||(j>=m))
		return;
	d1= (double *) (((unsigned) fp)+(i*n)*(sizeof(double)));
	d2= (double *) (((unsigned) fp)+(j*n)*(sizeof(double)));
	for(k=0;k<n;k++) {
		x= *d1;
		*d1= *d2;
		*d2= x;
		d1++; d2++;
		}
	return;
}


printmat(fp,m,n)

double *fp;
int m,n;

{
	int i,j;

	for(i=0;i<m;i++) {
		for(j=0;j<n;j++)
			printf("%7.2f ",*(fp++));
		printf("\n");
		}
	return;
}


evalle(fp,m,ov)

double *fp,*ov;
int m;

/*
 * for m by (m+1) upper triangular matrix with 1 on diag, evaluate vars
 */

{
	int j,k;
	double x,y,*d1,*d2;

#ifdef DEBUG
	printf("evalle(m: %d)\n",m);
#endif
	for(k=(m-1);k>=0;k--) {
		d1= (double *) (((unsigned) fp)+((k+1)*(m+1))*(sizeof(double)));
		d1--;
		d2= (double *) (((unsigned) ov)+(m-1)*(sizeof(double)));
		x= *(d1--);
		for(j=(m-1);j>k;j--) {
			x-= (*d1)*(*d2);
			d1--; d2--;
			}
		*d2= x;
		}
#ifdef DEBUG
	printf("evalle returning\n");
#endif
	return(1);
}


linsubs(fp,gp,m,n,k)

double *fp, *gp;
int m,n,k;

/*
 *	given eqns in upper triangular form (m rows, n cols), 
 *	solve for the first (n-k) variables in terms of the
 *	last k variables.  put answer in gp [must be (n-k)x(k+1)]
 */

{
	int i,j,l;
	double x,y;
	double *p, *q, *vp, *up, *rp;

#ifdef DEBUG
	printf("linsubs(m: %d, n: %d, k: %d)\n",m,n,k);
	printmat(fp,m,n);
	p= gp;
	for(i=0;i<(k+1)*(n-k);i++)
		*(p++)= 0.0;
#endif

	if((n-1)!=(m+k)) {
		printf("linsubs: under or over determined\n");
		printf("\trows: %s, cols: %d, independant vars: %d\n",m,n,k);
		return(0);
		}
	for(i=m;i>0;i--) {
		p= (double *) (((unsigned) fp)+(sizeof(double)*(i*n-1)));
		q= (double *)(((unsigned)gp)+(sizeof(double)*(i*(k+1)-1)));
		/* transpose last k+1 */
		vp= p;
		up= q;
		*(up--)= *(vp--);
		for(j=1;j<=k;j++)
			*(up--)= -(*(vp--));
#ifdef DEBUG
		printf("ans after transpos: \n");
		printmat(gp,n-k,k+1);
#endif
		/* now do each var */
		for(j=0;j<(n-1-k-i); j++) {
			x= *(vp--);
			rp= (double *)(((unsigned)gp)+(sizeof(double)*((m-j)*(k+1)-1)));
#ifdef DEBUG
		printf("pivot val: %f rp: %f\n",x,*rp);
#endif
			up= q;
			for(l=0;l<=k;l++)
				*(up--)-= x*(*(rp--));
			}
#ifdef DEBUG
		printf("ans after var loop: \n");
		printmat(gp,n-k,k+1);
#endif
		}
	return(1);
}



