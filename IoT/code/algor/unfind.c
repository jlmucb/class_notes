#define NS 100
#define NE 100


/*
 *	(c) Copyright, 1991, John L. Manferdelli.  All Rights Reserved.
 */


/*
 *	union find
 *		n union/finds takes O(n lg n) time
 *	can be made nearly linear with tarjan trick
 */


/* -------------------------------------------------------------- */


int ns;				/* number of sets */
int ne;				/* number of elements */

int inttoext[NS];		/* map from internal to external set names */
int exttoint[NS];		/* map from external to internal set names */
int size[NS];			/* number of elts in set i */
int list[NS];			/* number of elts in set i */
int R[NE];			/* element to internal set name */
int nxtR[NE];			/* linked list of elements of set */


find(i)

int i;

{
	return(inttoext[R[i]]);
}


union(i,j,k)

int i,j,k;

{
	int e,l,a,b,c;

	a= exttoint[i];
	b= exttoint[j];
	if(size[a]>size[b]) {
		c= a;
		a= b;
		b= c;
		}

	e= list[a];
	while(e>=0) {
		R[e]= b;
		l= e;
		e= next[e];
		}
	next[l]= list[b];
	list[b]= list[a];
	size[b]+= size[a];
	exttoint[k]= b;
	inttoext[b]= k;
	return;
}


/* -------------------------------------------------------------- */

