#include <stdio.h>

/* #define DEBUG */


/*
 *  (c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */


#define N 100
#define NC 100

double xx1[N],yy1[N],zz1[N];
double m1[N];
double xx2,yy2,zz2;
double m2;
double vxx2,vyy2,vzz2;
double kk[N];

/*
 *  multibody problem
 */
main(int argn, char** argv) {
  int i,j,n,ns,skip,skp1;
  double t,dt;
  double ux2,uy2,uz2,uvx2,uvy2,uvz2;
  char tmp[NC];

  fprintf(stdout,"Number of planets: "); fflush(stdout);
  if((j=readit(0,tmp,NC))<=0) {
    printf("invalid number\n");
    exit();
    }
  sscanf(tmp,"%d\n",&n);
  fprintf(stdout,"Number of steps: "); fflush(stdout);
  if((j=readit(0,tmp,NC))<=0) {
    printf("invalid number\n");
    exit();
    }
  sscanf(tmp,"%d\n",&ns);
  fprintf(stdout,"Number of steps skipped before printing: "); fflush(stdout);
  if((j=readit(0,tmp,NC))<=0) {
    printf("invalid number\n");
    exit();
    }
  sscanf(tmp,"%d\n",&skip);
  fprintf(stdout,"Step size: "); fflush(stdout);
  if((j=readit(0,tmp,NC))<=0) {
    printf("invalid number\n");
    exit();
    }
  sscanf(tmp,"%F\n",&dt);

  for(i=0;i<n;i++) {
    fprintf(stdout,"Position of planet %d: ",i); fflush(stdout);
    if((j=readit(0,tmp,NC))<=0) {
      printf("invalid number\n");
      exit();
      }
    sscanf(tmp,"(%F,%F,%F)\n",&xx1[i],&yy1[i],&zz1[i]);
    fprintf(stdout,"Planet %d attraction: ",i); fflush(stdout);
    if((j=readit(0,tmp,NC))<=0) {
      printf("invalid number\n");
      exit();
      }
    sscanf(tmp,"%F\n",&kk[i]);
    }
  fprintf(stdout,"Position of sattelite: ",i); fflush(stdout);
  if((j=readit(0,tmp,NC))<=0) {
    printf("invalid number\n");
    exit();
    }
  sscanf(tmp,"(%F,%F,%F)\n",&xx2,&yy2,&zz2);
  fprintf(stdout,"Velocity of sattelite: ",i); fflush(stdout);
  if((j=readit(0,tmp,NC))<=0) {
    printf("invalid number\n");
    exit();
    }
  sscanf(tmp,"(%F,%F,%F)\n",&vxx2,&vyy2,&vzz2);
  printf("\n%d planets, %d steps, %.3e second time step, %d skipped\n",
    n,ns,dt,skip);
  printf("\nInitial positions of Planets");
  printf("\nPlanet\t\t(x,y,z)\n");

  for(i=0;i<n;i++) {
    printf("%d\t\t(%.2e,%.2e,%.2e)\n",i,xx1[i],yy1[i],zz1[i]);
    }

  t= 0.0;
  printf("\nTime\t\t(x,y,z)\t\t\t\t(vx,vy,vz)\n");
  printf("%.3e\t(%.2e,%.2e,%.2e)\t(%.2e,%.2e,%.2e)\n",
      t,xx2,yy2,zz2,vxx2,vyy2,vzz2);
  fprintf(stdout,"\nReady? "); fflush(stdout);
  if((j=readit(0,tmp,NC))<=0) {
    printf("invalid number\n");
    exit();
    }
  printf("\n");
  skp1= skip;
  j= 1;
  for(i=1;i<ns;i++) {
    newcoord(dt,2,kk,xx1,yy1,zz1,
       xx2,yy2,zz2,vxx2,vyy2,vzz2,
       &ux2,&uy2,&uz2,&uvx2,&uvy2,&uvz2);
    t+= dt;
    xx2= ux2;
    yy2= uy2;
    zz2= uz2;
    vxx2= uvx2;
    vyy2= uvy2;
    vzz2= uvz2;
    if((skp1--)>0)
      continue;
    skp1= skip;
    if(j>20) {
      printf("\nTime\t\t\t\t(x,y,z)\t\t(vx,vy,vz)\n");
      j= 1;
      }
    printf("%.3e\t(%.2e,%.2e,%.2e)\t(%.2e,%.2e,%.2e)\n",
      t,xx2,yy2,zz2,vxx2,vyy2,vzz2);
    }

  printf("program done\n");
  exit();
}

