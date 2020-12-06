#include <stdio.h>
#include <math.h>


/*
 *  (c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */

/*
 *  calculation of initial phase angle and distance of
 *  closest approach of a lunar mission.
 */


main(argn, argv)

int argn;
char *argv[];

{
  double u,d,ds,rs,um,vm,e,r1,v1,phi1,gamma1;
  double lambda1,h,a,p,r0,ta0,ecc,ea1,tof;
  double parm1, parm2, eps2, hp, rp,vp;
  double v0,phi0,v02,ta1,ea0,gamma0,v2,e2,h2,p2,ecc2,km,sec;

  /* gm of the earth normalized to one earth gm unit */
  u= 1.0e+00;

  /* earth-moon distance in earth radii */
  d= 60.27e+00;

  /* lunar sphere of influence in earth radii */
  rs= 10.395e+00;

  /* gm of the moon in km^3/sec^2 */
  um= 4.90287e+03;

  /* circular orbital velocity of the moon in km/sec */
  vm= 1.018;

  printf("enter initial r and v in du and tu/du units: ");
  scanf("%lf %lf",&r0,&v0);
  printf("enter initial flight path angle: ");
  scanf("%lf",phi0);
  printf("enter angle which specifies the point at which\n");
  printf("the geocentric trajectory crosses the lunar soi: ");
  scanf("%lf",&lambda1);

  /* energy of the geocentric orbit */
  e= (v0*v02/2.0)-(u/r0);

  /* angular momentum of the geocentric orbit */
  h= r0*v0*cos(phi0);

  /* the radius at lunar arrival */
  r1= (d*d+rs*rs)-(2.0*d*rs*cos(lambda1/57.29577951));
  r1= sqrt(r1);

  /* the speed at lunar arrival */
  v1= 2.0*(e+u/r1);
  v1= sqrt(v1);

  /* the flight path angle at lunar arrival */
  phi1= h/(r1*v1);
  phi1= acos(phi1);
  phi1= phi1*57.29577951;

  /* the phase angle at arrival */
  gamma1= (rs/r1)*sin(lambda1/57.29577951);
  gamma1= asin(gamma1);
  gamma1= gamma1*57.29577951;
  /* parameter, semimajor axis, and eccentricity of geocentric orbit */
  p= h*h/u;
  a= -u/(2.0*e);
  ecc= sqrt(1.0-p/a);

  /* true amomaly of the geocentric orbit */
  ta0= (p-r0)/(r0*ecc);
  ta0= acos(ta0);
  ta0= ta0*57.29577951;

  /* true anomaly at arrival */
  ta1= (p-r1)/(r1*ecc);
  ta1= acos(ta1);
  ta1= ta1*57.29577951;

  /* eccentric anomaly of geocentric orbit */
  ea0= (ecc+cos(ta0/57.29577951))/(1.0+ecc*cos(ta0/57.29577951));
  ea0= acos(ea0);
  ea0= ea0*57.29577951;

  /* eccentric anomaly at arrival */
  ea1= (ecc+cos(ta1/57.29577951))/(1.0+ecc*cos(ta1/57.29577951));
  ea1= acos(ea1);
  ea1= ea1*57.29577951;

  /* time of flight */
  tof= sqrt(a*a*a/u);
  parm1= ea1/57.295-ecc*sin(ea1/57.29577951);
  parm2= ea0/57.295-ecc*sin(ea0/57.29577951);
  tof= tof*(parm1-parm2);

  /* initial phase angle */
  gamma0= ta1/57.295-ta0/57.295-gamma1/57.295-(2.137e-03*tof);
  gamma0= gamma0*57.295;
  v1= v1*7.905;
  rs= rs*6378.145;

  /* selenocentric arrival speed */
  v2= (v1*v1+vm*vm)-(2.*v1*vm*cos((phi1-gamma1)/57.29577951));
  v2= sqrt(v2);

  /* angle defining the direction of the initial selenocentric */
  /* velocity relative to the moon's center. */
  eps2= (vm/v2)*cos(lambda1/57.29577951);
  eps2= eps2-((v1/v2)*cos((lambda1+gamma1-phi1)/57.29577951));
  eps2= asin(eps2);
  eps2= eps2*57.29577951;

  /* energy, angular momentum relative to the center of the moon */
  e2= (v2*v2/2.0)-(um/rs);
  h2= rs*v2*sin(eps2/57.29577951);

  /* parameter and eccentricity of the selenocentric orbit */
  p2= h2*h2/um;
  ecc2= 1.0+((2.0*e2*h2*h2)/um*um);
  ecc2= sqrt(ecc2);

  /* conditions at periselenium */
  rp= p2/(1.0+ecc2);
  hp= rp-1738.00;
  vp= sqrt(2*(e2+um/rp));
  tof= (tof*13.44686)/60.0;

  if(rp<1738.00)
    printf("lunar impact has occured since rp < rm\n");
  printf("time of flight: %lf7.4, hours\n",tof);
  printf("phase angle of moon at arrival: %lf6.2\n", gamma1);
  printf("flight path angle at arrival: %lf6.2\n", phi1);
  printf("angle of v2 with respect to lunar center: %lf6.2\n", eps2);
  printf("speed at arrival: %lf7.4\n", km/sec, v1);

  r1= r1*6378.145;
  printf("radius at lunar arrival: %lf12.4\n", km, r1);
  printf("selenocentric speed at arrival: %lf7.4\n", km/sec, v2);
  printf("selenocentric radius: %lf12.4\n", km, rs);
  printf("velocity at periselenium: %lf7.4\n", km/sec, vp);
  printf("eccentricity of selenocentric orbit: %lf6.4\n", ecc2);
  printf("initial phase angle: %lf6.2\n", gamma0);
  printf("distance of closest approach: %lf10.4\n", km, hp);

  exit();
}
