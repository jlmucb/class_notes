#include <stdio.h>


/*
 * (c) Copyright, 1990, John L. Manferdelli. All Rights Reserved.
 */

#define XM 10
#define YM 10

int la[XM*YM]= {  /* array holding state */
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,1,0,0,0,0,0,
  0,0,0,1,1,1,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0
  };
int lb[XM*YM];    /* array holding state */


count(ip,m,n)

int *ip,m,n;

{
  int i,j,k;

  k= 0;
  for(i=0;i<n;i++) {
    for(j=0;j<m;j++) {
      if((*(ip++))>0)
        k++;
      }
    }
  return(k);
}


prstate(ip,m,n)

int *ip,m,n;

{
  int i,j,k;

  k= 0;
  for(i=0;i<n;i++) {
    printf("    ");
    for(j=0;j<m;j++) {
      if((*ip)>0)
        printf("*");
      else
        printf(" ");
      ip++;
      }
    printf("\n");
    }
  printf("%d non zero entries\n",k);
  return(k);
}



printstate(ip,m,n)

int *ip,m,n;

{
  int i,j,k;

  k= 0;
  for(i=0;i<n;i++) {
    printf("    ");
    for(j=0;j<m;j++) {
      if((*ip)>0)
        k++;
      printf("%04d ",*(ip++));
      }
    printf("\n");
    }
  printf("%d non zero entries\n",k);
  return(k);
}


int f(ii,l)

int ii[8],l;

/*
 *  next state function ii array is in the following order:
 *   -+ 0+ ++ -0 00 +0 -- 0- +-
 */

{
  register int i,j;

  i= 0;
  for(j=0;j<8;j+=2)
    if(ii[j]>0)
      i++;
  if(i>3)
    return(0);
  if(i==3)
    return(1);
  return(l);
}    


int *index(ip,m,n,i,j,out)

int *ip,m,n,i,j,*out;

{
  if((i<0)||(i>=m)||(j<0)||(j>=n))
    return(out);
  return((int *) (ip+(m*j)+i));
}


main(argn,argv)

int argn;
char *argv[];

{
  int ni;      /* number of iterations */
  int m,n;    /* x,y limits of array */
  int i,j,k;
  int *cia, *coa;    /* input array, output array */
  int oob={0};
  int ti[10];
  int weight[100];

  /*
   * pattern of sliding ti:
   *  ->->->...->|
   *                 V
   *      | <-<-<...<-
   *  V
   *  -> .......
   */

  ni= 10;
  if(argn>1)
    ni= atoi(argv[1]);
  n= 10;
  m= 10;
  cia= lb;
  coa= la;
  printf("Initial state:\n");
  weight[0]= prstate(la,m,n);
  for(i=0;i<ni;i++) {
    if(cia==la) {
      cia= lb;
      coa= la;
      }
    else {
      cia= la;
      coa= lb;
      }
    for(k=0;k<n;k++) {
      for(j=0;j<m;j++) {
        ti[0]= *index(cia,m,n,j-1,k+1,&oob);
        ti[1]= *index(cia,m,n,j,k+1,&oob);
        ti[2]= *index(cia,m,n,j+1,k+1,&oob);
        ti[3]= *index(cia,m,n,j-1,k,&oob);
        ti[4]= *index(cia,m,n,j+1,k,&oob);
        ti[5]= *index(cia,m,n,j-1,k-1,&oob);
        ti[6]= *index(cia,m,n,j,k-1,&oob);
        ti[7]= *index(cia,m,n,j+1,k-1,&oob);
        *index(coa,m,n,j,k)= f(ti,*index(cia,m,n,j,k));
        }
      }
    printf("After %d iterations:\n",i+1);
    weight[i+1]= prstate(coa,m,n);
    }
  printf("\ncount profile:\n");
  for(i=0;i<=ni;i++)
    if((i%10)==9)
      printf(" %3d\n",weight[i]);
    else
      printf(" %3d",weight[i]);
  printf("\ndone\n");
  exit();
}


/* ----------------------------------------------------------------------- */


