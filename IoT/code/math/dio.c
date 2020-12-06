#include <stdio.h>

#define XM 400
#define YM 300
#define ZM 100

int x1[XM], y1[YM], z1[ZM];


inarray(n,a,i)

int n,i,a[];

{
  int j;

  for(j=0;j<i;j++) {
    if(a[j]>=n) {
      if(a[j]==n)
        return(j);
      return(-1);
      }
    }
  return(-1);
}


main(argn,argv)

int argn;
char *argv[];

{
  int i,j,k,m,n;

  for(i=0;i<XM;i++)
    x1[i]= i*i;
  for(i=0;i<YM;i++)
    y1[i]= i*i;
  for(i=0;i<ZM;i++)
    z1[i]= i*i;

  for(i=1;i<XM;i++) {
    for(j=1;j<YM;j++) {
      if((k=inarray(x1[i]+y1[j],z1,ZM))>0)
        printf("Solution: (%d,%d,%d), %d+%d=%d\n",i,j,k,
          x1[i],y1[j],z1[k]);
      }
    }
  printf("\ndone\n");
  exit();
}


/* ----------------------------------------------------------------------- */


