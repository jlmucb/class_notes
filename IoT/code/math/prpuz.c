#include <stdio.h>


/*
 * (c) Copyright, 1990, John L. Manferdelli. All Rights Reserved.
 */

int bitpos[15];


         /* 012345678 9012345678 9012345678 9012345678 9012345678 9 */
char *mat={"    x    \n   x x   \n  x x x  \n x x x x \nx x x x x\n"};
int pt[15]= {4,13,15,22,24,26,31,33,35,37,40,42,44,46,48};


prtab(n)

int n;

{
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



main(argn,argv)

int argn;
char *argv[];

{
  int i,j;

  for(i=0;i<15;i++)
    bitpos[i]= 1<<(14-i);
  for(j=1;j<argn;j++) {
    sscanf(argv[j],"%x",&i);
    prtab(i);
    }
  exit();
}


/* ----------------------------------------------------------------------- */


