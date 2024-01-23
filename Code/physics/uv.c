#include <stdio.h>
#include <math.h>
#include "windt.h"


/*
 *  Sky and telescope 7/93, p84
 */

Double oz[]={
  34,25,18,9,3.2,1.8,.9,.46,.24,.17,
  .06,.05,.02,.01,.002,.001,0,0,0,0};

Double fs[]= {    /* flux at 1 au in erg/cm^2/sec/A */
  25,31,38,45,52,58,64,70,75,79,
  83,87,91,93,95,97,99,104,107,104};

Double as[]={
  1,1,1,1,.65,.22,.074,.025,.0086,.003,.0014,.0012,.00097,.00081,
  .00068,.00057,.00048,.0004,.00034,.00029};


main(an,av)
int an;
char *av[];
{
  Int j,sh;
  Double m,l,la,t,ds,ha,cz,zs,hh,h,kk,be,s1,sa,
       et,zp,ss,tw,f1,f2,n,re,ol,ho,hg,
       xo,xg,xa,w,ra,d0;
  Double ko,kg,ka,ot,ta,d,is,id,ig,i,ef,ed;
  Double x,y;

  printf("Solar burning\n");
  printf("\nLatitude in degrees (SF 37.5): "); scanf("%lf",&l);
  la= l/57.296;
  printf("Month (1-12): "); scanf("%lf",&m);
  printf("Hours from local noon: "); scanf("%lf",&t);
  ds= .408*sin(.523*(m-3.7));      /* declin of sun */
  ha= .262*t;                          /* hour angle of sun */

  cz= sin(la)*sin(ds)+cos(la)*cos(ds)*cos(ha);
  if(cz<0.0) {
    printf("negative cz, error\n");
    exit(0);
    }
  zs= atan(sqrt(1.0-cz*cz)/cz);
  /*
   * or al= altitude of sun in degrees;
   * zs= (90.0-al)/57.296;
   */

  printf("Height above sea level (ft): "); scanf("%lf",&hh);
  h= hh/3280.0;       /* should this be 5280? */
  printf("Aerosol extinction (V mag/airmass)[.1-.3 at SL, .7 hazy&wet]: "); scanf("%lf",&kk);
  be= kk/2.36;
  printf("Surface albedo (percent)[10]: "); scanf("%lf",&s1);
  sa= s1/100.0;
  printf("Latitude: %.1lf, Month: %.1lf, Diff Noon: %.2lf, Ht:%.1lf, Aerosol: %.2lf, Albedo: %.1lf\n",
    l,m,t,hh,kk,s1);

  et= 3600.0;    /* exposure time (seconds) */
  zp= zs;      /* angle zenith to normal (rad) */
  ss= 0.0;    /* angle sun to normat (rad) */
  tw= 1.0;    /* fraction transmitted */
  f1= 1.0;    /* fraction of sky that is clear */
  f2= 1.0;    /* fraction of ground in sunlight */
  sh= 1;      /* sun in shade?, 1=no, 0= yes */
  n= 1.0;      /* normalization for skin type */
  re= 6378;    /* radius of earth (km) */
  ol= 0;      /* ozone loss (%) */
  ho= 23;      /* height of ozone level (km) */
  hg= 8.2;    /* gas scale height (km) */
  ha= 1.5;    /* aerosol scale height (km) */
  ds= 1.0;    /* distance from sun (au) */

  /* thickness of ozone */
  ra= 30.0*(m-3.7)/57.296;
  d0= (1.0-ol/100.0)*(3.0+0.4*(la*cos(ra)-cos(3.0*la)));

  /* airmass for each component */
  x= sin(zs)/(1.0+((ho-h)/re));
  xo= 1.0/sqrt(1.0-x*x);
  xg= 1.0/(cos(zs)+.01*sqrt(hg)*exp(-30.0*cos(zs)/sqrt(hg)));
  xa= 1.0/(cos(zs)+.01*sqrt(ha)*exp(-30.0*cos(zs)/sqrt(ha)));

  /* do for each wavelength */
  ed= 0.0;
  for(j=0;j<20;j++) {
    w= .275+((Double)(j+1))*.005;  /* wavelength in microns */
    /* brightness of sunlight, diffuse sky blue and ground light */
    ko= oz[j]*(d0/3.0);
    kg= .0107*exp(-h/hg)/(w*w*w*w);
    ka= be*(pow(w,-1.3));
    ot= pow(10.0,(-.4*ko*xo));
    ta= pow(10.0,(-.4*(kg*xg+ka*xa)));
    d= .5*pow(cos(zs),.33);
    is= fs[j]*ot*ta*tw*((Double) sh)*cos(ss)/(ds*ds);
    id= fs[j]*ot*(1.0-ta)*d;
    id= fs[j]*ot*(1.0-ta)*d*ta*sa+id;
    id= fs[j]*ot*(1.0-ta)*(1.0-ta)*sa*d*d+id;
    id= id*tw*f1*cos(zs/2.0)*cos(zs/2.0)/(ds*ds);
    ig= fs[j]*ot*sa*(ta+d*(1.0-ta));
    ig= ig*tw*f2*(sin(zs/2.0)*sin(zs/2.0))/(ds*ds);
    i= is+id+ig;    /* total skin flux */
    ef= i*et*(as[j]*n)*50.0/2.3e6;
    ed= ed+ef;
    printf("wavelength: %.2lf, ozone: %.2lf, air: %.2lf, action: %.5lf, dose: %.4lf\n",
         w*10000,ot,ta,as[j],ef);
    }

  /*
   *  Minimum erythema dose at 2900A is 2.3e6 erg/cm^2
   */

   /* Results */
  printf("\nTotal effective dose on cloudless day: %lf using %lf hours exposure\n",ed,et/3600.);
  printf("Burning starts after %lf hours\n",et/(3600.0*ed));
  exit(0);
}


/* ------------------------------------------------------- */

