#include <stdio.h>`


/*
 *  (c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */

main(argn,argv)
int argn;
char *argv[];
{
  int i,j,sg;
  double k, x,y,z;

  i= atoi(argv[1]);
  j= atoi(argv[2]);
  sscanf(argv[3],"%F",&k);
  /* k= atof(argv[3]); */

  if(i<0) {
    sg= 1;
    i*= (-1);
    }
  else
    sg= 0;
  printf("%d hours %d mins %f sec\n",i,j,k);
  x= k;
  x/= 60.0;
  x+= (double) j;
  x/= 60.0;
  x+= (double) i;
  x*= 15.0;
  printf("\t%f degrees\n",x);
  exit();
}


