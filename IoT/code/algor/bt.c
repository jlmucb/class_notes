#include "bt.h"

/*
 *(c) Copyright, 1991, John L. Manferdelli.  All Rights Reserved.
 *  Did delete etc 12/94
 */


/* -------------------------------------------------------------- */


static unsigned nfree={NULL};
int bt_nn={0};      /* number of nodes used */


static unsigned getnode() {
  int i;
  unsigned a,b;

  if(nfree==NULL) {
    if((nfree=((unsigned)malloc(NFP*sizeof(node))))==NULL) {
      printf("Out of B-tree node space\n");
      return(NULL);
      }
    a= nfree;
    i= NFP-1;
    while((i--)>0) {
      b= a+sizeof(node);
      Btp(a)->c1= b;
      a= b;
      }
    Btp(a)->c1= NULL;
    }
  a= nfree;
  nfree= Btp(nfree)->c1;
  bt_nn++;
  return(a);
}


static freenode(a)

unsigned a;

{
  Btp(a)->c1= nfree;
  nfree= a;
  return;
}


/* -------------------------------------------------------------- */


void propagate(n,m,v)

unsigned n,m,v;

/*
 *  n is parent, m is child, v in min value
 */

{
#ifdef DEBUG1
  printf("propagate(%x %x)\n",n,m);
#endif
  if((n==NULL)||(m==NULL))
    return;
  if(Btp(n)->c1==m) {
    Btp(n)->m1= v;
    propagate((unsigned) (Btp(n)->p),n,v);
    return;
    }
  if(Btp(n)->c2==m) {
    Btp(n)->m2= v;
    return;
    }
  if(Btp(n)->c3==m) {
    Btp(n)->m3= v;
    return;
    }
  return;
}


unsigned bt_traverse(n,nf,kf,cf)

unsigned n;
int (*nf)(),(*kf)(),(*cf)();

{
#ifdef DEBUG2
  printf("bt_traverse %x\n",n);
#endif
  if(nf!=NULL)
    (*nf)(n);
  if(Btp(n)->ct==NODE) {
    switch(Btp(n)->nc){
      case 1:
      bt_traverse(Btp(n)->c1,nf,kf,cf);
      break;
      case 2:
      bt_traverse(Btp(n)->c1,nf,kf,cf);
      bt_traverse(Btp(n)->c2,nf,kf,cf);
      break;
      case 3:
      bt_traverse(Btp(n)->c1,nf,kf,cf);
      bt_traverse(Btp(n)->c2,nf,kf,cf);
      bt_traverse(Btp(n)->c3,nf,kf,cf);
      break;
      }
    return(1);
    }
  if(kf!=NULL) {
    switch(Btp(n)->nc){
      case 1:
      (*kf)(Btp(n)->c1);
      break;
      case 2:
      (*kf)(Btp(n)->c1);
      (*kf)(Btp(n)->c2);
      break;
      case 3:
      (*kf)(Btp(n)->c1);
      (*kf)(Btp(n)->c2);
      (*kf)(Btp(n)->c3);
      break;
      }
    }
  return(1);
}


unsigned bt_ischild(n,v,cf)

unsigned n,v;        /* node, value */
int (*cf)();

{
  int i;

#ifdef DEBUG1
  printf("bt_ischild(%x)\n",v);
#endif
  if(Btp(n)->ct!=LEAF)
    return(NULL);
  if((i=((*cf)(v,Btp(n)->c1)))==0)
    return(Btp(n)->c1);
  if(i<0)
    return(NULL);
  if((i=((*cf)(v,Btp(n)->c2)))==0)
    return(Btp(n)->c2);
  if(i<0)
    return(NULL);
  if((i=((*cf)(v,Btp(n)->c3)))==0)
    return(Btp(n)->c3);
  return(NULL);
}


unsigned bt_pc(n)

unsigned n;

{
  if(Btp(n)->ct==LEAF)
    return;
  if(((unsigned)(Btp(Btp(n)->c1)->p))!=n) {
    printf("***Error at parent %x\n",n);
    return;
    }
  bt_pc(Btp(n)->c1);
  if(Btp(n)->nc<2)
    return;
  if(((unsigned)(Btp(Btp(n)->c2)->p))!=n) {
    printf("***Error at parent %x\n",n);
    return;
    }
  bt_pc(Btp(n)->c2);
  if(Btp(n)->nc<3)
    return;
  if(((unsigned)(Btp(Btp(n)->c3)->p))!=n) {
    printf("***Error at parent %x\n",n);
    return;
    }
  return;
}



unsigned bt_find(n,v,cf)

unsigned n,v;        /* node, value */
int (*cf)();

{
#ifdef DEBUG2
  printf("bt_find(%x),",n);
  printf("nc: %d, %x %x %x\n",Btp(n)->nc,Btp(n)->c1,
    Btp(n)->c2,Btp(n)->c3);
#endif
  if((Btp(n)->ct)==LEAF)
    return(n);
  if(((Btp(n)->nc)<2)||((*cf)(v,Btp(n)->m2)<0))
    return(bt_find(Btp(n)->c1,v,cf));
  if(((Btp(n)->nc)<3)||((*cf)(v,Btp(n)->m3)<0))
    return(bt_find(Btp(n)->c2,v,cf));
  return(bt_find(Btp(n)->c3,v,cf));
}


static unsigned splitnode(r,n,oc,m,cf)

unsigned *r;      /* root */
unsigned n,oc,m;    /* node (intree), old child of n, new node */
int (*cf)();      /* comparison function */

{
  unsigned a,b;

#ifdef DEBUG1
  printf("splitnode(%x %x)\n",n,m);
#endif
  if(n==NULL) {    /* split root */
    if((b=getnode())==NULL)
      return(NULL);
    Btp(b)->nc= 2;
    Btp(b)->ct= NODE;
    Btp(b)->c1= *r;
    Btp(b)->c2= m;
    Btp(b)->m1= Btp((*r))->m1;
    Btp(b)->m2= Btp(m)->m1;
    Btp(b)->p= NULL;
    Btp(m)->p= (node *)b;
    Btp((*r))->p= (node *)b;
    *r= b;
    return(m);
    }
  if(Btp(n)->nc==2) {
    (Btp(n)->nc)++; 
    Btp(m)->p= (node *) n;
    if(Btp(n)->c1==oc) {
      Btp(n)->m3= Btp(n)->m2;
      Btp(n)->c3= Btp(n)->c2;
      Btp(n)->m2= Btp(m)->m1;
      Btp(n)->c2= m;
      }
    else {
      Btp(n)->m3= Btp(m)->m1;
      Btp(n)->c3= m;
      }
    return(n);
    }
  if((a=getnode())==NULL)
    return(NULL);
  Btp(n)->nc= 2;
  Btp(a)->ct= NODE;
  Btp(a)->nc= 2;
  if(Btp(n)->c1==oc) {
    Btp(a)->c2= Btp(n)->c3;
    Btp(a)->c1= Btp(n)->c2;
    Btp(n)->c2= m;
    Btp(a)->m1= Btp(n)->m2;
    Btp(a)->m2= Btp(n)->m3;
    Btp(n)->m2= Btp(m)->m1;
    Btp(Btp(a)->c1)->p= (node *)a;
    Btp(Btp(a)->c2)->p= (node *)a;
    Btp(Btp(n)->c2)->p= (node *)n;
    }
  else if(Btp(n)->c2==oc) {
    Btp(a)->c2= Btp(n)->c3;
    Btp(a)->c1= m;
    Btp(a)->m1= Btp(m)->m1;
    Btp(a)->m2= Btp(n)->m3;
    Btp(Btp(a)->c1)->p= (node *)a;
    Btp(Btp(a)->c2)->p= (node *)a;
    }
  else {
    Btp(a)->c2= m;
    Btp(a)->c1= Btp(n)->c3;
    Btp(a)->m2= Btp(m)->m1;
    Btp(a)->m1= Btp(n)->m1;
    Btp(Btp(a)->c1)->p= (node *)a;
    Btp(Btp(a)->c2)->p= (node *)a;
    }
  splitnode(r,(unsigned)Btp(n)->p,n,a,cf);
  return(b);
}


unsigned bt_insert(r,v,cf)

unsigned *r;      /* root */
unsigned v;      /* node, value */
int (*cf)();      /* comparison function */

{
  int i;
  unsigned a,b;

#ifdef DEBUG1
  printf("bt_insert(%s)\n",v);
#endif
  if((*r)==NULL)  {
    if((b=getnode())==NULL)
      return(NULL);
    *r= b;
    Btp(b)->ct= LEAF;
    Btp(b)->nc= 1;
    Btp(b)->c1= v;
    Btp(b)->m1= v;
    Btp(b)->p= NULL;
    Btp(b)->c2= NULL;
    Btp(b)->c3= NULL;
    return(b);
    }
  b= *r;
  if(Btp(b)->nc==1) {
    (Btp(b)->nc)++; 
    if((*cf)(v,Btp(b)->m1)<0) {
      Btp(b)->m2= Btp(b)->m1;
      Btp(b)->c2= Btp(b)->c1;
      Btp(b)->c1= v;
      Btp(b)->m1= v;
      }
    else {
      Btp(b)->c2= v;
      Btp(b)->m2= v;
      }
    return(b);
    }
  if((b=bt_find(*r,v,cf))==NULL)
    return(NULL);
#ifdef DEBUG1
  printf("find at insert returns %x\n",b);
#endif
  if(Btp(b)->nc==2) {
    (Btp(b)->nc)++; 
    if((*cf)(v,Btp(b)->c1)<0) {
      Btp(b)->c3= Btp(b)->c2;
      Btp(b)->m3= Btp(b)->m2;
      Btp(b)->c2= Btp(b)->c1;
      Btp(b)->m2= Btp(b)->m1;
      Btp(b)->m1= v;
      Btp(b)->c1= v;
      propagate(Btp(b)->p,b,v);
      }
    else if ((*cf)(v,Btp(b)->c2)<0) {
      Btp(b)->c3= Btp(b)->c2;
      Btp(b)->m3= Btp(b)->m2;
      Btp(b)->m2= v;
      Btp(b)->c2= v;
      }
    else {
      Btp(b)->m3= v;
      Btp(b)->c3= v;
      }
    return(b);
    }
  if((a=getnode())==NULL)
    return(NULL);
  Btp(a)->ct= LEAF;
  Btp(a)->nc= 2;
  Btp(b)->nc= 2;
  if((*cf)(v,Btp(b)->c1)<0) {
    Btp(a)->c1= Btp(b)->c2;
    Btp(a)->m1= Btp(b)->m2;
    Btp(a)->c2= Btp(b)->c3;
    Btp(a)->m2= Btp(b)->m3;
    Btp(b)->c2= Btp(b)->c1;
    Btp(b)->m2= Btp(b)->m1;
    Btp(b)->c1= v;
    Btp(b)->m1= v;
    }
  else if((*cf)(v,Btp(b)->c2)<0) {
    Btp(a)->c1= Btp(b)->c2;
    Btp(a)->m1= Btp(b)->m2;
    Btp(a)->c2= Btp(b)->c3;
    Btp(a)->m2= Btp(b)->m3;
    Btp(b)->c2= v;
    Btp(b)->m2= v;
    }
  else if((*cf)(v,Btp(b)->c3)<0) {
    Btp(a)->c1= v;
    Btp(a)->c2= Btp(b)->c3;
    Btp(a)->m1= v;
    Btp(a)->m2= Btp(b)->m3;
    }
  else {
    Btp(a)->c2= v;
    Btp(a)->c1= Btp(b)->c3;
    Btp(a)->m2= v;
    Btp(a)->m1= Btp(b)->m3;
    }
  splitnode(r,Btp(b)->p,b,a,cf);
  return(b);
}


unsigned bt_delete(r,n,v,cf)

unsigned *r;      /* root */
unsigned n,v;      /* node (intree), node to attach */
int (*cf)();      /* comparison function */

{
  printf("No deletion allowed yet\n");
  return(NULL);
}


/* -------------------------------------------------------------- */



