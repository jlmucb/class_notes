
/*
 *  (c) Copyright, 1991, John L. Manferdelli.  All Rights Reserved.
 */

#ifndef NULL
#define NULL 0
#endif

/* -------------------------------------------------------------- */


unsigned bubble(n,u,cf)

unsigned n,u;    /* number of entries, address of first entry */
int (*cf)();    /* comparison function */

{
  register int i,j;
  register unsigned v1,v2;
  unsigned s;

#ifdef DEBUG2
  printf("bubble(%d)\n",n);
#endif
  v1= u;
  for(i=0;i<(n-1);i++) {
    v2= v1+sizeof(unsigned);
    for(j=(i+1);j<n;j++) {
      if((*cf)(*((unsigned *)v1),*((unsigned *)v2))<0) {
        s= *((unsigned *) v1);
        *((unsigned *) v1)= *((unsigned *) v2);
        *((unsigned *) v2)= s;
        }
      v2+= sizeof(unsigned);
      }
    v1+= sizeof(unsigned);
    }
    
  return(n);  
}


/* -------------------------------------------------------------- */


static unsigned comb(n,u1,u2,o,cf)

unsigned n,u1,u2,o;  /* number of entries, address of first entry */
int (*cf)();    /* comparison function */

{
  register unsigned v1,v2,v3;
  int i,j;

  v1= u1;
  v2= u2;
  v3= o;
  i= n;
  j= n;
  while((i>0)||(j>0)) {
    if(i==0) {
      *((unsigned *) v3)= *((unsigned *) v2);
      j--;
      }
    else if(j==0) {
      *((unsigned *) v3)= *((unsigned *) v1);
      i--;
      }
    else if((*cf)(*((unsigned *) v1), *((unsigned *) v2))<1) {
      *((unsigned *) v3)= *((unsigned *) v2);
      j--;
      }
    else {
      *((unsigned *) v3)= *((unsigned *) v1);
      i--;
      }
    }

  return(2*n);
}



static unsigned m_sort(n,in,out,cf)

unsigned n,in,out;  /* number of entries, address of first entry */
int (*cf)();    /* comparison function */

{
  int m;

  if(n<=1) {
    *((unsigned *)out)= *((unsigned *)in);
    return(1);
    }
  m= n/2;
  m_sort(m,in,out,cf);
  m_sort(m,in+m*sizeof(unsigned),out+m*sizeof(unsigned),cf);
  comb(m,out,out+m*sizeof(unsigned),in,cf);
  return(n);
}


unsigned merge(n,u,cf)

unsigned n,u;    /* number of entries, address of first entry */
int (*cf)();    /* comparison function */

{
  register unsigned v1, v2;
#ifdef DEBUG2
  printf("merge(%d)\n",n);
#endif
  v1= u;
  if((v2=malloc(n*sizeof(unsigned)))==NULL)
    return(-1);
  m_sort(n,v1,v2,cf);
  /* mfree(v2); */
  return(n);  
}


/* -------------------------------------------------------------- */


static int rand(i)

int i;

{
  return(i/2);
}


static q_sort(n,in,out,cf)

unsigned n,in,out;  /* number of entries, address of first entry */
int (*cf)();    /* comparison function */

{
  int m,i1,i2,i3;
  unsigned u,v;
  unsigned v1,v2,v3;

  if(n<=1) {
    *((unsigned *)out)= *((unsigned *)in);
    return(1);
    }
  i1= 0;
  i2= 0;
  i3= 0;
  m= rand(n);
  u= *((unsigned *) (in+m*sizeof(unsigned)));
  v= in;
  for(m= 0; m<n; m++) {
    switch((*cf)(*((unsigned *) v),u)) {
      case -1:
      i1++;
      break;
      case 0:
      i2++;
      break;
      case 1:
      i3++;
      break;
      }
    v+= sizeof(unsigned);
    }
  v= in;
  v1= out;
  v2= out+i1*sizeof(unsigned);
  v3= out+(i1+i2)*sizeof(unsigned);
  for(m=0;m<n;m++) {
    switch((*cf)(*((unsigned *) v),u)) {
      case -1:
      *((unsigned *) v1)= *((unsigned *) v);
      v1+= sizeof(unsigned);
      break;
      case 0:
      *((unsigned *) v2)= *((unsigned *) v);
      v2+= sizeof(unsigned);
      break;
      case 1:
      *((unsigned *) v3)= *((unsigned *) v);
      v3+= sizeof(unsigned);
      break;
      }
    v+= sizeof(unsigned);
    }
  v1= in;
  v2= out;
  for(m=0;m<n;m++) {
    *((unsigned *) v1)= *((unsigned *) v2);
    v1+= sizeof(unsigned);
    v2+= sizeof(unsigned);
    }
  q_sort(i1,in,out,cf);
  q_sort(i3,in+(i1+i2)*sizeof(unsigned),out+(i1+i2)*sizeof(unsigned),cf);
  return(n);
}


unsigned quick(n,u,cf)

unsigned n,u;    /* number of entries, address of first entry */
int (*cf)();    /* comparison function */

{
  register unsigned v1, v2;
#ifdef DEBUG2
  printf("quick(%d)\n",n);
#endif
  v1= u;
  if((v2=malloc(n*sizeof(unsigned)))==NULL)
    return(-1);
  q_sort(n,v1,v2,cf);
  /* mfree(v2); */
  return(n);  
}


/* -------------------------------------------------------------- */


static heapify(n,i,j,u,cf)

int i,j;
unsigned n,u;    /* number of entries, address of first entry */
int (*cf)();    /* comparison function */

{
  int s1,s2,s;
  register unsigned v1, v2, v;
#ifdef DEBUG2
  printf("heapify(%d %d %d)\n",i,j,n);
#endif
  if((i>n)||(j>n))
    return;
  v= u+(i-1)*sizeof(unsigned);
  s= 0;
  s1= 2*i;
  s2= 2*i+1;
  if((s1<=n)&&(s2<=n)) {
    v1= u+(s1-1)*sizeof(unsigned);
    v2= u+(s2-1)*sizeof(unsigned);
    if((*cf)(*((unsigned *) v1),*((unsigned *) v2))>0) {
      if((*cf)(*((unsigned *) v1),*((unsigned *) v))>0)
        s= 1;
      }
    else {
      if((*cf)(*((unsigned *) v2),*((unsigned *) v))>0)
        s= 2;
      }
    }
  else if(s1<=n) {
    v1= u+(s1-1)*sizeof(unsigned);
    if((*cf)(*((unsigned *) v1),*((unsigned *) v))>0)
      s= 1;
    }
  if(s==1) {
    v2= *((unsigned *) v1);
    *((unsigned *) v1)= *((unsigned *) v);
    *((unsigned *) v)= v2;
    heapify(n,s1,j,u,cf);
    }
  if(s==2) {
    v1= *((unsigned *) v2);
    *((unsigned *) v2)= *((unsigned *) v);
    *((unsigned *) v)= v1;
    heapify(n,s2,j,u,cf);
    }
  return(n);
}


unsigned heap(n,u,cf)

unsigned n,u;    /* number of entries, address of first entry */
int (*cf)();    /* comparison function */

{
  int i;
  register unsigned v1, v2, v;
#ifdef DEBUG2
  printf("heap(%d)\n",n);
#endif
  /* buildheap */
  for(i=n;i>0;i--)
    heapify(n,i,n,u,cf);
  v1= u;
  v2= u+(n-1)*sizeof(unsigned);
  for(i=n;i>1;i--) {
    v= *((unsigned *) v1);
    *((unsigned *) v1)= *((unsigned *) v2);
    *((unsigned *) v2)= v;
    heapify(i-1,1,i-1,u,cf);
    }
  return(n);  
}


/* -------------------------------------------------------------- */


