
#include <stdio.h>
#define N 10

int np={0};
int w1={0}, w2={0};

int r1[6]={1,1,0,0,0,0};
int r2[6]={0,0,1,1,0,0};
int r3[6]={0,1,0,1,0,0};
int r4[6]={0,0,0,0,1,1};


pwin(w,n,nm,a)

int w,n,nm,a[];

{
  int i;

  printf("Win for player %d after %d draws: ",w,nm);
  for(i=0;i<n;i++)
    printf(" %d",a[i]);
  printf("\n");
}


int f(n,a)

int n,a[];

{
  int i,d;
  int nr1,nr2,nr3,nr4;

  d= 0;
  nr1= 0;
  nr2= 0;
  nr3= 0;
  nr4= 0;
  for(i=0;i<n;i++) {
    nr1+= r1[a[i]-1];
    nr2+= r2[a[i]-1];
    nr3+= r3[a[i]-1];
    nr4+= r4[a[i]-1];
    if((nr1>1)||(nr2>1)) {
      w1++;
      d= 1;
      pwin(1,6,i+1,a);
      }
    if((nr3>1)||(nr4>1)) {
      w2++;
      d= 1;
      pwin(2,6,i+1,a);
      }
    if(d>0)
      return(1);
    }
  return(1);
}


main(argn,argv)

int argn;
char *argv[];

{
  int i,j,n;
  int aa[N];

  for(i=0;i<6;i++)
    aa[i]= i+1;
  rperm(6,1,aa,f);
  printf("win for player 1: %d, win for player 2: %d\n",w1,w2);
  exit();
}

