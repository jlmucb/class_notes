#include "math.h"


/*
 *  (c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */

#define FABS

static double pi2={PI/2.0};
static double tpi={2.0*PI};
static double thpi={PI+(PI/2.0)};
static double e={2.718281828459045};
static double ft[16]={1,1,2,6,24,120,720,5040,40320,362880,3628800,39916800,
    479001600,6227020800,8.71782912e10,1307674368e12};
static unsigned sq2[96]={
  0x667f57db,0x3ff6a09e,0xc2e9738,0x3ff306fe,
  0x3d687d29,0x3ff172b8,0x6d6a2476,0x3ff0b558,
  0xdad8b9a2,0x3ff059b0,0x42c5deee,0x3ff02c9a,
  0xac26d5b5,0x3ff0163d,0xfb704432,0x3ff00b1a,
  0x8764b71a,0x3ff0058c,0x6282acc,0x3ff002c6,
  0xf3b2de58,0x3ff00162,0x79d96f2c,0x3ff000b1,
  0xbcecb796,0x3ff00058,0x5e765bcb,0x3ff0002c,
  0x2f3b2de6,0x3ff00016,0x179d96f3,0x3ff0000b,
  0x8bcecb7a,0x3ff00005,0xc5e765bd,0x3ff00002,
  0x62f3b2de,0x3ff00001,0xb179d96f,0x3ff00000,
  0x58bcecb8,0x3ff00000,0x2c5e765c,0x3ff00000,
  0x162f3b2e,0x3ff00000,0xb179d97,0x3ff00000,
  0x58bcecc,0x3ff00000,0x2c5e766,0x3ff00000,
  0x162f3b3,0x3ff00000,0xb179da,0x3ff00000,
  0x58bced,0x3ff00000,0x2c5e76,0x3ff00000,
  0x162f3b,0x3ff00000,0xb179e,0x3ff00000,
  0x58bcf,0x3ff00000,0x2c5e8,0x3ff00000,
  0x162f4,0x3ff00000,0xb17a,0x3ff00000,
  0x58bd,0x3ff00000,0x2c5e,0x3ff00000,
  0x162f,0x3ff00000,0xb18,0x3ff00000,
  0x58c,0x3ff00000,0x2c6,0x3ff00000,
  0x163,0x3ff00000,0xb2,0x3ff00000,
  0x59,0x3ff00000,0x2c,0x3ff00000,
  0x16,0x3ff00000,0xb,0x3ff00000,
  };


/* -------------------------------------------------------------- */


#ifdef FABS

double fabs(x)

double x;

{
  if(x<0.0)
    return(-x);
  return(x);
}

#endif


/*
 *  sqrt, sin, cos, asin, acos, tan, atan, ln, exp
 */


double sqrt(x)

double x;

{
  double a,b;

#ifdef DEBUG7
  printf("sqrt(%f)\n",x);
#endif
  if(x<0.0)
    return(-1);
  if(x==0.0)
    return(0.0);
  if(x>(1.0e200)) {
    printf("square root error\n");
    return(ERROR);
    }
  a= x;
  while((a*a)>(100.0*x)) {
    if(a>10.0)
      a/= 10.0;
    else
      break;
    }
  for(;;) {
    a= (a+(x/a))*0.5;
    if((fabs(x-(a*a))/x)<0.0000001)
      return(a);
    }
}


/* -------------------------------------------------------------- */


double sin(x)

double x;

{
  int i,j;
  double a,b,c,d,y;

#ifdef DEBUG71
  printf("debug sin: %f; ",x);
#endif
  if(x<0.0) {
    a= x*(-1.0);
    y= (-1.0);
    }
  else {
    a= x;
    y= 1.0;
    }
  i= a/tpi;
  a-= ((double) i)*tpi;
#ifdef DEBUG70
  printf("normalized: %f;",x);
#endif
  if((a>=pi2)&&(a<PI))  {
    a= PI-a;
    }
  else if((a>=PI)&&(a<thpi)) {
    a-= PI;
    y*= (-1.0);
    }
  else if((a>=thpi)&&(a<=tpi)) {
    a= tpi-a;
    y*= (-1.0);
    }
#ifdef DEBUG70
  printf("adjusted a: %f, y: %f\n",a,y);
#endif
  d= a;
  c= 0.0;
  b= (-1.0)*a*a;
  for(j=0;j<8;j++) {
    c+= d/ft[2*j+1];
    d*= b;
    }
#ifdef DEBUG71
  printf("= %f\n",c*y);
#endif
  return(c*y);
}


double cos(x)

double x;

{
  int i;
  double y,z,u;

#ifdef DEBUG71
  printf("cos(%f)= ",x);
#endif
  i= x/tpi;
  if(i>=0.0)
    z= x-(((double) i)*tpi);
  else
    z= x+(((double) ((-i)+1))*tpi);
  if(z<0.0)
    z+= tpi;
  y= sin(x);
  if((z<=pi2)||(z>=thpi))
    u= sqrt(1-(y*y));
  else
  u= (-sqrt(1-(y*y)));
#ifdef DEBUG71
  printf("%f\n",u);
#endif
  return(u);
}


double tan(x)

double x;

{
  double y,z;

#ifdef DEBUG7
  printf("tan(%f)\n",x);
#endif
  y= sin(x);
  z= 1-(y*y);
  if(z<(1.0e-200)) {
    if(y>0.0)
      return(pi2);
    else
      return(-pi2);
    }
  return(y/sqrt(z));
}


double asin(x)

double x;

{
  double y;

#ifdef DEBUG8
  printf("asin(%f)\n",x);
#endif
  y= fabs(x);
  if(y>1.0) {
    printf("asine error %f\n",y);
    return(ERROR);
    }
  if(x==1.0)
    return(pi2);
  if(x==(-1.0))
    return(-pi2);
  y= fabs(1.0-(x*x));
  if(y<(1.0e-200)) {
    if(x>0)
      return(pi2);
    else
      return(-pi2);
    }
  return(atan(x/sqrt(y)));
}


double acos(x)

double x;

{
  double y,z;

#ifdef DEBUG8
  printf("acos(%f)\n",x);
#endif
  z= sqrt(1.0-x*x);
  if(x<0.0)
    return(PI-asin(z));
  else
    return(asin(z));
}


double atan(x)

double x;

/*
 *  make tan(x) small by using half angle identities
 *  then use taylor series
 */

{
  int i,j;
  double a,b,c,d,y;

#ifdef DEBUG8
  printf("arctan(%f) ",x);
#endif
  i= 1;
  if(x<0.0)
    a= (-x);
  else
    a= x;

  while(a>0.1) {
    b= 1.0/sqrt(1.0+(a*a));    /* to cosine */
    c= sqrt((1.0+b)/2.0);    /* half angle */
    a= sqrt(1.0-(c*c))/c;    /* tan of half angle */
    i*= 2;
#ifdef DEBUG1
    printf("1.0 a: %f, b: %f, c: %f i: %d\n",a,b,c,i);
#endif
    }

  /* x - x**3/3 + x**5/5 - x**7/7 + ... - x**11/11 */
  c= (-1.0)*a*a;
  d= a;
  y= a;
#ifdef DEBUG1
  printf("1.7 b: %f, c: %f i: %d\n",b,c,i);
#endif
  for(j=3;j<12;j+=2) {
#ifdef DEBUG1
    printf("1.5 d: %f, y: %f, c: %f\n",d,y,c);
#endif
    d*= c;
    y+= d/((double) j);
    }
#ifdef DEBUG1
  printf("2.0 d: %f, y: %f, c: %f\n",d,y,c);
#endif

  if(x<0.0)
    y*= -1.0;
  y*= ((double) i);
#ifdef DEBUG8
  printf("= %f\n",y);
#endif
  return(y);
}


/* -------------------------------------------------------------- */


double exp(x)

double x;

/*
 *  use straight exponentiation for integer portion
 *  use taylor series for fractional part
 */

{
  int i,j;
  double a,b,c,d;

#ifdef DEBUG12
  printf("debug exp: %f\n",x);
#endif
  if(x<0.0)
    a= x*(-1.0);
  else
    a= x;
  if(a>2147483648.0) {
    printf("exp error\n");
    return(ERROR);
    }
  i= a;
  a-= ((double) i);
#ifdef DEBUG12
  printf("debug i: %d, a: %f\n",i,a);
#endif
  b= e;
  c= 1.0;
  for(j=0;j<30;j++) {
#ifdef DEBUG12
    printf("debug i: %d, b: %f, c: %f\n",i,b,c);
#endif
    if(i<=0)
      break;
    if((i&1)!=0)
      c*= b;
    b*= b;
    i>>= 1;
    }
#ifdef DEBUG12
  printf("debug int c: %f\n",c);
#endif
  b= 1.0;
  d= a;
  for(j=1;j<18;j++) {
    b+= d/ft[j];
    d*= a;
    }
#ifdef DEBUG12
  printf("debug end c: %f\n",c);
#endif
  c*= b;
  if(x<0.0)
    return(1.0/c);
  return(c);
}


static double tw={1073741824.0};
static double tx={32768.0};


double ln(x)

double x;

/*
 *  find a.bcde... (binary) st 2**(a.bcde...) == x
 */

{
  int i,j;
  double a,b,y,*fp;

#ifdef DEBUG2
  printf("ln(%f)\n",x);
#endif
  i= 0;
  if(x<0.0) {
    printf("ln error\n");
    return(ERROR);
    }
  if(x<1.0)
    a= 1.0/x;
  else
    a= x;
  while(tw<a) {
    a/= tw;
    i+= 30;
    }
  
#ifdef DEBUG2
  printf("3.0 i: %d\n",i);
#endif
  /* get rid of exponential part */
  y= tx;
  for(j=15;j>0;j--) {
    if(a>=y) {
      a/= y;
      i+= j;
      }
    y/= 2.0;
    }
#ifdef DEBUG2
  printf("4.0 i: %d\n",i);
#endif
  b= (double) i;
  fp= (double *) (&sq2[0]);
  y= 0.5;
  for(j=0;j<30;j++) {
#ifdef DEBUG2
    printf("4.5 a: %f b: %f y: %f sq: %f\n",a,b,y,*fp);
#endif
    if(a>=(*fp)) {
      b+= y;
      a/= (*fp);
      }
    fp++;
    y/= 2.0;
    }
#ifdef DEBUG2
  printf("5.0 b: %f\n",b);
#endif
  b*= LN2;
#ifdef DEBUG2
  printf("6.0 b: %f\n",b);
#endif
  if(x<1.0)
    return(-b);
  return(b);
}


/* -------------------------------------------------------------- */


#ifdef DEBUG


main(argn,argv)

int argn;
char *argv[];

{
  int i,j;
  double x,y;

  printf("argv[1]: %s\n",argv[1]);
  sscanf(argv[1],"%F",&x);
  printf("x: %f\n",x);
  y= exp(x);
  printf("exp(%f)= %f\n",x,y);
  y= cos(x);
  printf("cos(%f)= %f\n",x,y);
  y= sin(x);
  printf("sin(%f)= %f\n",x,y);
  y= sqrt(x);
  printf("sqrt(%f)= %f\n",x,y);
  y= asin(x);
  printf("asin(%f)= %f\n",x,y);
  y= acos(x);
  printf("acos(%f)= %f\n",x,y);
  y= atan(x);
  y= ln(x);
  printf("ln(%f)= %f\n",x,y);
  exit(1);
}

#endif


/* -------------------------------------------------------------- */


