#include <stdio.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#define PI 3.14159265
#define XM 800
#define YM 800

#define DEBUG


extern scale();


/*
 * (c) Copyright, 1990, John L. Manferdelli. All Rights Reserved.
 */


char map[XM*YM];    /* "bit map" */
int gr={0};


f(t,a,x,y)

double a,t,*x,*y;

{
  /* printf("evaluating t: %f, a: %f\n",t,a); */
  *x= 2.0*a*cos(t)+a*cos(3.0*t);
  *y= 2.0*a*sin(t)-a*sin(3.0*t);
}


g(t,a,x,y)

double a,t,*x,*y;

{
  *x= 3.0*a*cos(t);
  *y= 3.0*a*sin(t);
}



main(argn,argv)

int argn;
char *argv[];

{
  int i,j,k,ix,iy,ni,ixc,iyc;
  double a,x,y,t,ts;
  double xsf, ysf;

  if(InitGraphics(XM,YM)<=0) {
    printf("Can't use bitmap graphics\n");
    }
  else
    gr= 1;

  i= XM<YM?XM:YM;
  xsf= ysf= 2.1;
  i/= 2;
  ixc= (iyc= i);
  a= i/3;

  ni= 1000;
  t= 0.0;
  ts= (2.0*PI)/((double) ni);
  for(i=0;i<ni;i++) {
    f(t,a,&x,&y);
#ifdef DEBUG1
    printf("f(%f)= (%f,%f)\n",t,x,y);
#endif
    if(scale(xsf,ysf,x,y,XM,YM,ixc,iyc,&ix,&iy)>0) {
      map[iy*XM+ix]= 1;
      }
    g(t,a,&x,&y);
    if(scale(xsf,ysf,x,y,XM,YM,ixc,iyc,&ix,&iy)>0) {
      map[iy*XM+ix]= 1;
      }
    t+= ts;
    }

  displaybits(map,XM,YM,XM,YM);

  CloseGraphics();
  exit();
}


/* ----------------------------------------------------------------------- */


