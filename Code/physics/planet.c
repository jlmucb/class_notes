#include <stdio.h>
#include "math.h"
#include "alib.h"

#define FANCY
#define STARS


/*
 *  (c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */


#define MERCURY 0
#define VENUS 1
#define EARTH 2
#define MARS 3
#define JUPITER 4
#define SATURN 5
#define URANUS 6
#define NEPTUNE 7
#define PLUTO 8

#define DIST 0
#define PER 1
#define ECC 2
#define INCL 3
#define NODE 4
#define PERH 5
#define PEPOCH 6

#define dat(p,d) (7*p+d)


/*
 *
 *  TPEPOCH is time since epoch of planetary data
 *
 *  Planetary data:
 *    mean distance from sun in millions of km
 *    period of revolution in days
 *    eccentricity of orbit
 *    inclination of orbit from ecliptic (degrees)
 *    longitude of ascending node (degrees from aries)
 *    longitude of perhelion (degrees from ascending node)
 *    longitude at epoch (degrees from aries 0 Jan, 1980)
 *  All longitudes referred from node (aries)
 *
 *  PS: 1 AU = 1.496*10**8 km
 */
#define TPEPOCH 2202.0
double planetdat[7*(PLUTO+1)]= {      /* epoch 1986.1.10 */
    57.9,        87.97,   .2,  7.0,  48.35,  77.43, 242.9,
   108.2,        224.7, .0068,  3.4,  76.6, 131.3, 283.9,
   149.6,       365.26, .0167,    0,     0, 102.8, 109.42,
   227.9,       686.98, .0933,  1.85,  49.5, 335.8, 200.52,
   778.3,  11.8*365.26, .048,  1.3, 100.0,  15.5, 330.3,
  1427.0,  29.4*365.26, .0515,  2.48, 113.6,  92.2, 239.3,
  2869.0,   84.*365.26, .046,   .77,  74.0, 175.2, 253.5,
  4496.0, 164.8*365.26, .008,  1.77, 131.7,  7.41, 274.5,
  5900.0, 247.7*365.26, .250, 17.13, 110.2, 224.2, 218.9
  };

/* 
 *   Routines to plot the position of the planets
 *  Time measured in days since epoch (Jan 0, 1980)
 *  Distance measured in kilometers
 *   sun is at (0,0,0)
 *   aries is at (1,0,0)
 */

/*
 *  Earth/moon barycenter: 
 *    E center= barycenter-.0000312 (cos L, sin L)
 *    L= 218+481268(t-2451545.0)
 */

double x1[9], y1[9], z1[9];    /* position of planet i in km */
double dec[9], ra[9];      /* declination and right asc of i */
double az[9],alt[9];      /* azimuth, bearing */
double lat, lng;      /* latitude, longitude */
double paries={(PARII/24.0)*2.0*PI};  /* posn of aries at epoch (rad) */
double w0;        /* angular speed of earth (per 24hrs) */
double decsun,rasun,azsun,altsun;
double decmoon,ramoon,azmoon,altmoon;
int dostars={1};
int localtime={0};


/* ------------------------------------------------------------------------- */


double qplanetdat(int a) {
  int pni, i;
  char inp[80];
  double x;
  extern char *toplanetname();

  pni= a/7;
  x= planetdat[a];
  printf("planet %s, supplied: %7.2f: ",toplanetname(pni),x);
  fflush(stdout);
  if((i=read(0,inp,80))>1)
    sscanf(inp,"%F\n",&x);
  printf("qplanet (%d) value: %7.2f\n",i,x);
  return(x);
}


char *toplanetname(int i) {
  switch(i) {
    case 0:
    return("mercury");
    case 1:
    return("venus");
    case 2:
    return("earth");
    case 3:
    return("mars");
    case 4:
    return("jupiter");
    case 5:
    return("saturn");
    case 6:
    return("uranus");
    case 7:
    return("neptune");
    case 8:
    return("pluto");
    }
  return("?");
}


frplanetname(char* p) {
  if(strcmp(p,"mercury")==0)
    return(MERCURY);
  if(strcmp(p,"venus")==0)
    return(VENUS);
  if(strcmp(p,"earth")==0)
    return(EARTH);
  if(strcmp(p,"mars")==0)
    return(MARS);
  if(strcmp(p,"jupiter")==0)
    return(JUPITER);
  if(strcmp(p,"saturn")==0)
    return(SATURN);
  if(strcmp(p,"uranus")==0)
    return(URANUS);
  if(strcmp(p,"neptune")==0)
    return(NEPTUNE);
  if(strcmp(p,"pluto")==0)
    return(PLUTO);
}


double datetime(yr,m,d,hr,min,secs,t2)

int yr,m,d,hr,min;
double secs;
struct T2 *t2;

{
  t2->year= yr;  /* year */
  t2->mno= m;  /* month number */
  t2->day= d;  /* day of month */
  t2->hr= hr;  /* hour of day */
  t2->min= min;  /* minutes of hour */
  t2->sec= secs;  /* seconds in hour */
  return(t2toint(t2,EPOCH));
}


orbelts(e,d,a,b,f)
double e,d,*a,*b,*f;

/*
 *  given ecc and mean distance calculate orb elts a>b, focal length
 */

{
  double x,y;

#ifdef DEBUG
  printf("orbelts\n");
#endif
  /* b/a= (1-e**2)**(1/2) */
  x= sqrt(1-e*e);
  *a= d;
  *b= d*x;
  *f= d*e;
#ifdef DEBUG
  printf("orbital elements: e: %8.3e d: %8.3e a: %8.3e b: %8.3e f: %8.3e\n",e,d,*a,*b,*f);
#endif
  return(1);
}


ellipse(a,b,f,u,x,y,z)
double a,b,f,u;   /* semi maj, minor, focal len, angle */
double *x,*y,*z;
{
  *x= a*cos(u)-f;
  *y= b*sin(u);
  *z= 0;
#ifdef DEBUG
  printf("ellipse: u: %6.2e x: %6.2e y: %6.2e\n",
        u,*x,*y);
#endif
  return(0);
}


kepler(ecc,ww,t1,pe,E)
double ecc,ww,t1,pe,*E;
{
  int i;
  double M,p0,p1,xi;

#ifdef DEBUG
  printf("Kepler e: %f, t: %f, ww: %f, pe: %f\n",ecc,t1,ww,pe);
#endif
  /* solve tan(f/2)= sqrt((1+e)/(1-e)) tan (E/2) */
  p1= sqrt((1.0-ecc)/(1.0+ecc))*tan(pe/2.0);
  p0= 2.0*atan(p1);
  M= ww*t1+pe;
#ifdef DEBUG
  printf("Kepler p0: %f, M: %f\n",p0,M);
#endif
  /* solve M= E-e sin(E) */
  /* ie g(x)= x-e(sin(x))-M = 0 */
  xi= M;
  for(i=0;i<3;i++)
    xi= xi-(xi-(ecc*(sin(xi)))-M)/(1.0-(ecc*cos(xi)));
  *E= xi;
#ifdef DEBUG
  printf("Kepler E: %f\n",*E);
#endif
  return(1);
}


planet(i,t,px,py,pz)
int i;
double t,*px,*py,*pz;
/*
 *  1. calculate new T0'= T0-(ln+ph) -- long of node, perhelion
 *  2. calculate position on ellipse with  T0'
 *  3. rotate in xy by ph
 *  4. rotate back by -ln so ascending node is at +x and DN is ar -x
 *  5. incline so pts on neg y axis up towards pos z
 *  6. rotate back by ln
 */
{
  double ww,w,ecc,ph,ln,incl,pepoch,a,b,f,dist;
  double x1,y1,z1,x2,y2,z2;
  double appcorr,E;

#ifdef DEBUG
  printf("planet %s %7.2f\n",toplanetname(i),t);
#endif

  /* get orbital data */
  w= planetdat[dat(i,PER)];        /* period */
  ph= angles(planetdat[dat(i,PERH)],"deg","rad");    /* perhelion */
  ln= angles(planetdat[dat(i,NODE)],"deg","rad");    /* long node */
  incl= angles(planetdat[dat(i,INCL)],"deg","rad");  /* inclinatn */
  pepoch= angles(planetdat[dat(i,PEPOCH)],"deg","rad");  /* pos/epoch */
  ecc= planetdat[dat(i,ECC)];
  dist=planetdat[dat(i,DIST)];
  ww= 2.0*PI/w;

#ifdef DEBUG
  printf("per: %5.2f, ",w);
  printf("perh: %5.2f, ",ph);
  printf("Lnode: %5.2f, ",ln);
  printf("incl: %5.2f, ",incl);
  printf("pepoch: %5.2f\n",pepoch);
  printf("dist: %5.2f\n",dist);
  printf("ecc: %5.2f\n",ecc);
#endif

#ifdef FANCY

  /* get major and minor axis plus focus */
  orbelts(ecc,dist,&a,&b,&f);
  appcorr= dist/299792.458;
  appcorr/= 3600.0*24.0;
  appcorr*= 2.0*PI;

  kepler(ecc,ww,t-TPEPOCH,pepoch-ph,&E);
  /* calculate point on standard ellipse */
  ellipse(a,b,f,E,&x1,&y1,&z1);

  /* rotate to perhelion */
  rotate(x1,y1,z1,&x2,&y2,&z2,"xy",ph-ln);
  *px= x2;
  *py= y2;
  *pz= z2;

  /* incline if not earth */
  if(i!=EARTH) {
    /*
     * rotate(x2,y2,z2,&x1,&y1,&z1,"xy",-ln);
     * rotate(x1,y1,z1,&x2,&y2,&z2,"yz",incl);
     * rotate(x2,y2,z2,px,py,pz,"xy",ln);
     */
     rotate(x2,y2,z2,&x1,&y1,&z1,"yz",incl);
     rotate(x1,y1,z1,px,py,pz,"xy",ln);
    } 
#else
  a= dist;
  b= ww*t+pepoch;
  *px= a*cos(b);
  *py= a*sin(b);
  *pz= 0.0;
#endif

#ifdef DEBUG
  printf("after rot to perh: %6.2e %6.2e %6.2e\n",x2,y2,z2);
#endif

  return;
}

initplanet() {
  int i;

  /* distances in planetdat should be multiplied by 10**6 */
  for(i=0;i<=PLUTO;i++)
    planetdat[dat(i,DIST)]*= 1.0e6;
  return;
}

positions(t)

double t;    /* days since epoch */

/*
 *  aries is at (1 0 0)
 */

{
  int i,j,k;

#ifdef DEBUG
  printf("positions()\n");
#endif
  initplanet();

  for(i=0;i<9;i++) {
    planet(i,t,&x1[i],&y1[i],&z1[i]);
#ifdef DEBUG
    printf("Position of %s: (%8.3e %8.3e %8.3e) in km\n\n",
      toplanetname(i),x1[i],y1[i],z1[i]);
#endif
    }

  return;
}



/*
 *  earth coordinates of planets
 *    center back on earth, calculate azimuth and altitude
 *    rotate earth by 23.5
 *    calculate declination and right ascension
 */
earthcoord(double t) {
  int i,j,k;
  double x,y,z,xe,ye,ze,u,v;
  double xa,ya,za;
  double o,ddec,cdec,cra;

#ifdef DEBUG
  printf("earthcoord()\n");
#endif
  xe= x1[2];
  ye= y1[2];
  ze= z1[2];
  o= angles(23.5,"deg","rad");

  for(i=0;i<9;i++) {
    if(i==2)
      continue;

    /* to spherical coordinates, a1: azimuth, a2: xy axis angle */
    x= x1[i]-xe;
    y= y1[i]-ye;
    z= z1[i]-ze;
#ifdef DEBUG
    printf("\nCoordinates of %s rel to earth: (%g,%g,%g)\n",
       toplanetname(i),x,y,z);
#endif
    /*
     *  rotate so earth is tilted
     */
    rotate(x,y,z,&xa,&ya,&za,"yz",o);
#ifdef DEBUG
    printf("\nCoordinates of %s in equitorial coord: (%g,%g,%g)\n",
       toplanetname(i),xa,ya,za);
#endif
    tospherical(xa,ya,za,&cdec,&cra);
#ifdef DEBUG
    printf("Spherical angles for tilted earth: (%7.2f, %7.2f)\n",
       cdec,cra);
#endif

    /* to declination, right ascension */
    ddec= PI/2.0-cdec;
    dec[i]= ddec*180.0/PI;
    ra[i]= cra*180.0/PI;
    if(dec[i]>90.0)
      dec[i]-= 180.0;
    if(ra[i]<0.0)
      ra[i]+= 360.0;
    if(ra[i]>360.0)
      ra[i]-= 360.0;
#ifdef DEBUG
    printf("(dec, ra): (%7.2f %7.2f)\n",dec[i],ra[i]);
#endif
    /* tilt earth towards equatorial */
    eqtohor(t-TAEPOCH,paries,cra,ddec,lat,lng,&u,&v);
    alt[i]= u*180.0/PI;
    az[i]= v*180.0/PI;
#ifdef DEBUG
    printf("(Alt, Azm): (%7.2f %7.2f)\n",alt[i],az[i]);
#endif
    }
  /*
   *  for sun
   */
  x= -xe;
  y= -ye;
  z= -ze;
  rotate(x,y,z,&xa,&ya,&za,"yz",o);
  tospherical(xa,ya,za,&cdec,&cra);
  ddec= PI/2.0-cdec;
  decsun=ddec*180.0/PI;
  rasun= cra*180.0/PI;
  if(decsun>90.0)
    decsun-= 180.0;
  if(rasun<0.0)
    rasun+= 360.0;
  if(rasun>360.0)
    rasun-= 360.0;
  eqtohor(t,paries,cra,ddec,lat,lng,&u,&v);
  altsun= u*180.0/PI;
  azsun= v*180.0/PI;
  if(decmoon>90.0)
    decmoon-= 180.0;
  if(ramoon<0.0)
    ramoon+= 360.0;
  if(ramoon>360.0)
    ramoon-= 360.0;
  eqtohor(t-TAEPOCH,paries,ramoon*PI/180.0,decmoon*PI/180.0,lat,lng,&u,&v);
  altmoon= u*180.0/PI;
  azmoon= v*180.0/PI;
  return;
}


#define NSTARS 23

struct star {
  char *name;
  int ra1,ra2;
  int d1,d2;
  double vmag;
  double dist;
  } stars[NSTARS]= {
  /* Name    RA  Dec  Vmag  Dist(lt yr) */
    "Alpheratz",  00,  3,  28, 32,  2.07,  31,
    "Polaris",   1, 23,  88, 46,  2.06,  200,
    "Aldeberan",   4, 30,  16, 19,  .8,  21,
    "Capella",   5,  9,  45, 54,  .09,  14,
    "Rigel",   5, 10,  -8, 19,.11,  270,
    "Bellatrix",   5, 20,   6, 16,  1.63,  140,
    "Betelgeuse",   5, 50,   7, 23,  .4,  180,
    "Sirius",   6, 41,  -16, 35,-1.44,  2.7,
    "Canopus",   6, 22,  -52, 38,-.72,  0,
    "Castor",   7, 28,  32,  6,  1.56,  14,
    "Procyon",   7, 34,   5, 29,  .36,  3.5,
    "Pollux",   7, 39,  28, 16,  1.15,  10.7,
    "Regulus",  10,  3,  12, 27,  1.34,  26,
    "Merak",  10, 56,  56, 55,  2.36,  23,
    "Dubhe",  10, 58,  62, 17,  1.81,  30,
    "Mizar",  13, 20,  55, 27,  2.12,  26,
    "Spica",  13, 20,  -10, 38,.97,  65,
    "Arcturus",  14, 11,  19, 42,  -.05,  11,
    "Antares",  16, 23,  -26, 13,.94,  130,
    "Vega",  18, 34,  38, 41,  .03,  8.1,
    "Altair",  19, 46,   8, 36,  .77,  4.9,
    "Deneb",  20, 38,  44, 55,  1.25,  500,
    "Formalhaut",  22, 52,  -30,  9,1.16,  7
    };


double sra[NSTARS],sdec[NSTARS],salt[NSTARS],saz[NSTARS];


cstars(t,la,lo)
double t,la,lo;
{
  int i,j,k;
  double x,y,z,u;

  for(i=0;i<NSTARS;i++) {
    x= stars[i].ra1/24.0+stars[i].ra2/1440.0;
    x*= 2.0*PI;
    if(stars[i].d1>=0.0)
      y= stars[i].d1/360.0+stars[i].d2/21600.0;
    else
      y= stars[i].d1/360.0-stars[i].d2/21600.0;
    y*= 2.0*PI;
    sra[i]= angles(x,"rad","deg");
    sdec[i]= angles(y,"rad","deg");
    eqtohor(t,paries,x,y,la,lo,&z,&u);
    salt[i]= angles(z,"rad","deg");
    saz[i]= angles(u,"rad","deg");
    }
  return;
}



#define TMEPOCH 0.0
#define EPSG 278.83354
#define OMEGG 282.596403
#define ECCSUN 0.016718
#define ECCMOON .0546
#define IMOON 5.145396
#define L0MOON 64.975464
#define P0MOON 349.383063
#define N0MOON 151.950429
#define MSUN 53.315
#define LAMSUN 337.448


moon(double t) {
  int i;
  double Ns,Ms,Ecs,Ls;
  double Mm,Lnm,Evm,Aem,A3m,A4m,Ecm,lpm,l;
  double lppm,Mpm,nu,Lm,Np,bm,N;
  double a,r,o,x1,y1;

  t-= TMEPOCH;
  Ns= t*360.0/365.2422;
  Ms= Ns+EPSG-OMEGG;
  if(Ms<0.0) {
    i= -Ms/360.0;
    Ms+= ((double)(i+1))*360.0;
    }
  Ecs= ECCSUN*(360.0/PI)*sin(angles(Ms,"deg","rad"));
  Ls= Ns+Ecs+EPSG;
  if(Ls<0.0) {
    i= -Ls/360.0;
    Ls+= ((double)(i+1))*360.0;
    }
#ifdef DEBUGMOON
  printf("Ns: %f, Ms: %f, Ls: %f\n",Ns,Ms,Ls);
#endif
  l= (13.1763966*t)+L0MOON;
  if(l<0.0) {
    i= -l/360.0;
    l+= ((double)(i+1))*360.0;
    }
  if(l>360.0) {
    i= l/360.0;
    l-= ((double)i)*360.0;
    }
  Mm= l-(0.1114041*t)-P0MOON;
  if(Mm<0.0) {
    i= -Mm/360.0;
    Mm+= ((double)(i+1))*360.0;
    }
  if(Mm>360.0) {
    i= Mm/360.0;
    Mm-= ((double)i)*360.0;
    }
  N= N0MOON-0.0529539*t;
  Evm= 1.2739*sin(angles(2.0*(l-Ls)-Mm,"deg","rad"));
  Aem= 0.1858*sin(angles(Ms,"deg","rad"));
  A3m= 0.37*sin(angles(Ms,"deg","rag"));
#ifdef DEBUGMOON
  printf("l: %f, Mm: %f, N: %f, Evm: %f, Aem: %f, A3m: %f\n",l,Mm,N,Evm,Aem,A3m);
#endif
  Mpm= Mm+Evm-Aem-A3m;
  Ecm= 6.2886*sin(angles(Mpm,"deg","rad"));
  A4m= 0.214*sin(angles(2.0*Mpm,"deg","rad"));
  lpm= l+Evm+Ecm-Aem+A4m;
  nu= 0.6583*sin(angles(2.0*(lpm-Ls),"deg","rad"));
#ifdef DEBUGMOON
  printf("Mpm: %f, Ecm: %f, A4m: %f, lpm: %f, nu: %f\n",Mpm,Ecm,A4m,lpm,nu);
#endif
  lppm= lpm+nu;
  Np= N-0.16*sin(angles(Ms,"deg","rad"));
#ifdef DEBUGMOON
  printf("lppm: %f, Np: %f\n",lppm,Np);
#endif
  y1= sin(angles(lppm-Np,"deg","rad"))*cos(angles(IMOON,"deg","rad"));
  x1= cos(angles(lppm-Np,"deg","rad"));
  Lm= atan(y1/x1);
  if((y1<0.0)&&(x1<0.0))
    Lm+= PI;
  if((y1>0.0)&&(x1<0.0))
    Lm+= PI;
  Lm+= angles(Np,"deg","rad");
#ifdef DEBUGMOON
  printf("y1: %f, x1: %f, Lm: %f\n",y1,x1,angles(Lm,"rad","deg"));
#endif
  bm= asin(sin(angles((lppm-Np),"deg","rad"))*sin(angles(IMOON,"deg","rad")));
#ifdef DEBUGMOON
  printf("(rad) Lm: %f, bm: %f\n",Lm,bm);
  printf("(deg) Lm: %f, bm: %f\n",angles(Lm,"rad","deg"),angles(bm,"rad","deg"));
#endif
  o= angles(23.442,"deg","rad");
  y1= (sin(Lm)*cos(o)-tan(bm)*sin(o));
  x1= cos(Lm);
  r= atan(y1/x1);
  if((y1<0.0)&&(x1<0.0))
    r+= PI;
  if((y1>0.0)&&(x1<0.0))
    r+= PI;
  a= asin(sin(bm)*cos(o)+cos(bm)*sin(o)*sin(Lm));
  ramoon= angles(r,"rad","deg");
  if(ramoon<0.0) {
    i= -ramoon/360.0;
    ramoon+= ((double)(i+1))*360.0;
    }
  if(ramoon>360.0) {
    i= ramoon/360.0;
    ramoon-= ((double)i)*360.0;
    }
  decmoon= angles(a,"rad","deg");
#ifdef DEBUGMOON
  printf("RA: %f, Dec: %f\n",ramoon,decmoon);
#endif
  return;
}


#define MAIN
#ifdef MAIN

main(argn,argv)
int argn;
char *argv[];

/*
 *  aries is at (1 0 0)
 */

{
  int i,j,k,n;
  int ir,id,ial,iaz;
  double ram,dem,alm,azm;
  int yr,d,m,hr,min;
  double secs,t,dist,lt;
  double xe,ye,ze,xpe,ype,zpe;
  char *locname;
  struct T2 t2, t3;
  char bb1[50], bb2[50];

#ifdef DEBUG
  printf("planet programs %d\n",argn);
  for(i=0;i<argn;i++)
    printf("argv[%d]: %s\n",i,argv[i]);
#endif
  if(argn<=1) {
    printf("Calling sequence: planet [-pos position] year month day min d hr min secs [in GMT] [-localtime] [-nostars]\n");
    exit();
    }

  /*
   *  San Francisco  37 49 N  122 25 W
   */
  locname= "San Francisco";
  lat= 0.66;
  lng= -2.137;
  n= 0;
  for(i=0;i<argn;i++) {
    if(strcmp(argv[i],"-nostars")==0) {
      dostars= 0;
      }
    if(strcmp(argv[i],"-localtime")==0) {
      localtime= 1;
      }
    if(strcmp(argv[i],"-pos")==0) {
      n+= fetchpos(&locname,&lat,&lng,&argv[i+1]);
      lat= angles(lat,"deg","rad");
      lng= angles(lng,"deg","rad");
      n++;
      }
    }
  w0= W0;

  printf("\nObserver location: %s, Latitude: %6.2f, Longitude: %6.2f\n",
    locname,lat*180.0/PI,lng*180.0/PI);

  /* time */
  yr= atoi(argv[n+1]);
  m= atoi(argv[n+2]);
  d= atoi(argv[n+3]);
  hr= atoi(argv[n+4]);
  min= atoi(argv[n+5]);
  sscanf(argv[n+6],"%F",&secs);
#ifdef DEBUG
  printf("yr: %d, m: %d, d: %d, hr: %d, min: %d, secs: %f\n",yr,m,d,hr,min,secs);
#endif
  t= datetime(yr,m,d,hr,min,secs,&t2);
  if(localtime==0) {
    lt= gmttocivil(t,angles(lng,"rad","deg"));
    intot2(lt,&t3,EPOCH);
    }
  else {
    lt= t;
    t= civiltogmt(lt,angles(lng,"rad","deg"));
    t3= t2;
    intot2(t,&t2,EPOCH);
    }
  printdate(&t2); printf(" GMT, ");
  printdate(&t3); printf(" local civil time\n");
  printf("%.4f days since 0 Jan 1980 UT\n",t);

  positions(t);
  moon(t);
  earthcoord(t);
#ifdef STARS
  if(dostars!=0)
    cstars(t,lat,lng);
#endif

  printf("\nPlanets\n");
#ifdef STARS
  printf("Position(Sun at 0)      Dist          Dec              RA    Alt   Azim  Name\n");
#endif
  xe= x1[2]/AUKM; ye= y1[2]/AUKM; ze= z1[2]/AUKM;
  for(i=0;i<9;i++) {
    x1[i]/= AUKM; y1[i]/= AUKM; z1[i]/= AUKM;
    xpe= x1[i]-xe; ype= y1[i]-ye; zpe= z1[i]-ze;
    dist= sqrt(xpe*xpe+ype*ype+zpe*zpe);
    tomins(ra[i]/15.0,&ir,&ram);
    tomins(dec[i],&id,&dem);
    tomins(alt[i],&ial,&alm);
    tomins(az[i],&iaz,&azm);
    printf("(%6.2f,%6.2f,%6.2f) %5.2f %5.1f(%4.1fm) %5.1f(%2dh%4.1fm)  %5.1f  %5.1f  %s\n",
         x1[i],y1[i],z1[i],dist,dec[i],dem,ra[i],ir,ram,
         alt[i],az[i],toplanetname(i));
    }
  printf("(%6.2g,%6.2g,%6.2g) -----      %7.2f         %7.2f  %5.1f  %5.1f  Sun\n",
    0.0,0.0,0.0,decsun,rasun,altsun,azsun);
  printf("                       -----      %7.2f         %7.2f  %5.1f %5.2f  Moon\n\n",
    decmoon,ramoon,altmoon,azmoon);
#ifdef STARS
  if(dostars!=0) {
    printf("\n\nStars\n");
    printf("Dec           RA      Alt    Azim   Magntd  Dist (ly)  Name\n");
    for(i=0;i<NSTARS;i++) 
      printf("%7.2f  %7.2f  %7.2f  %7.2f  %7.2f %7.2f  %s\n",
        sdec[i],sra[i],salt[i],saz[i],stars[i].vmag,
        stars[i].dist,stars[i].name);
    }
#endif

  exit();
}

#endif


/* ------------------------------------------------------------------------- */


