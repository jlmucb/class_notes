#include <stdio.h>
#include <math.h>


/*
 *  (c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */

double test[]={
  1,2,3,10,
  5,7,9,38,
  -1,3,4,7
  };

main(argn,argv)
int argn;
char *argv[];
{
  double ans[4];
  int i;

  printf("linear equation solver test\n");
  printmat(&test[0],3,4);
  for(i=0;i<4;i++) {
    pivot(&test[0],3,4,i,i);
    printf("after pivot %d\n\n",i);
    printmat(&test[0],3,4);
    }
  evalle(&test[0],3,&ans[0]);
  printf("answer\n");
  for(i=0;i<3;i++)
    printf(" %7.2f",ans[i]);
  printf("\n\ndone\n");
  exit();
}
