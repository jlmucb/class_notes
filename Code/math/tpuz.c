#include <stdio.h>


/*
 * (c) Copyright, 1990, John L. Manferdelli. All Rights Reserved.
 */

int freep;
int toexam;
int seen;
#define LISPN 10000
struct {
  int v,nx;
  } list[LISPN];

#define NMMV 36
struct {
  int n1,n2,n3;
  } nmvs[NMMV]={
    1,2,4, 1,3,6,
    2,4,7, 2,5,9,
    3,5,8, 3,6,10,
    4,2,1,4,5,6,4,7,11,4,8,13,
    5,8,12,5,9,14,
    6,3,1,6,9,13,6,5,4,6,10,15,
    7,4,2,7,8,9,
    8,5,3,8,9,10,
    9,5,2,9,8,7,
    10,6,3,10,9,8,
    11,7,4,11,12,13,
    12,8,5,12,13,14,
    13,8,4,13,9,6,13,12,11,13,14,15,
    14,9,5,14,13,12,
    15,10,6,15,14,13
    };


int posind[15];
int bitpos[15];


getnode() {
  int i;

  i= freep;
  freep= list[i].nx;
  return(i);
}


wasseen(int n) {
  int i;

  i= seen;
  while(i>=0) {
    if(n==list[i].v)
      return(1);
    i= list[i].nx;
    }
  return(0);
}


frnode(int n) {
  list[n].nx= freep;
  freep= n;
  return(1);
}


/* 012345678 9012345678 9012345678 9012345678 9012345678 9 */
char *mat={"    x    \n   x x   \n  x x x  \n x x x x \nx x x x x\n"};
int pt[15]= {4,13,15,22,24,26,31,33,35,37,40,42,44,46,48};


prtab(int n) {
  int i,j,k;
  int pr[15];
  char *p;

  printf("Print(%04x)\n",n);
  for(i=0;i<15;i++) {
    p= mat+pt[i];
    if(bitpos[i]&n)
      *p= 'x';
    else
      *p= '0';
    }
  printf("%s",mat);
  return;
}



main(int argn, char** argv) {
  int h,i,j,k,l,m,n;
  int m1,m2;

  for(i=0;i<15;i++)
    bitpos[i]= 1<<(14-i);
  j= 0;
  for(i=0;i<15;i++) {
    for(;j<NMMV;j++) {
      if(nmvs[j].n1==(i+1)) {
        posind[i]= j;
        break;
        }
      }
    }
  for(i=0;i<LISPN;i++)
    list[i].nx= i+1;
  list[LISPN-1].nx= -1;
  freep= 1;
  toexam= 0;
  seen= -1;
  list[0].v= 0x7bff;
  list[0].nx= -1;
#ifdef DEBUG1
  printf("bitpos:\n");
  for(i=0;i<15;i++) {
    printf("%x ",bitpos[i]);
    }
  printf("\n");
  printf("posind:\n");
  for(i=0;i<15;i++) {
    printf("%x ",posind[i]);
    }
  printf("\n");
  printf("nmvs:\n");
  for(i=0;i<36;i++) {
    printf("%d %d %d\n",nmvs[i].n1,nmvs[i].n2,nmvs[i].n3);
    }
  printf("\n");
#endif
  printf("Initial position:\n");
  prtab(0x7bff);
  printf("\n");

  while(toexam>=0) {
    j= toexam;
    i= list[j].v;
    toexam= list[j].nx;
    frnode(j);
    k= 0;    /* number of branches */
    printf("%04x: ",i);
    for(n=0;n<15;n++) {
#ifdef DEBUG1
        printf("n: %d\n",n);
        h= posind[n];
        m= bitpos[nmvs[h].n1-1]|bitpos[nmvs[h].n2-1]|
              bitpos[nmvs[h].n3-1];
        printf("h: %d, m: %04x\n",h,m);
#endif
        for(h=posind[n];nmvs[h].n1==(n+1);h++) {
            m1= bitpos[nmvs[h].n1-1]|bitpos[nmvs[h].n2-1];
      m2= m1|bitpos[nmvs[h].n3-1];
      if((i&m2)==m1) {
        l= (i&~m2)|bitpos[nmvs[h].n3-1];
#ifdef DEBUG
      printf("m1: %x, m2: %x, i&m1: %x\n",m1,m2,i&m2);
      printf("i: %x, l: %x\n",i,l);
#endif
        printf(" %04x",l);
        k++;
        if(wasseen(l)==0) {
          if((m=getnode())<0) {
            printf("out of node space\n");
            break;
            }
          list[m].nx= toexam;
          toexam= m;
          list[m].v= l;
          if((m=getnode())<0) {
            printf("out of node space\n");
            break;
            }
          list[m].v= l;
          list[m].nx= seen;
          seen= m;
          }
        }
          }
      }
    if(k==0)
      printf(" no descendants\n");
    else
      printf("\n");
    }

  printf("\ndone\n");
  exit();
}


/* ----------------------------------------------------------------------- */


