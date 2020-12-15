#include <stdio.h>
#include "alib.h"
#include "math.h"


/*
 *  (c) Copyright, 1989, John L. Manferdelli.  All Rights Reserved
 */

main(int argn,Char *argv[]) {
  Int i,j,n;
  Char *lname1, *lname2;
  Double lat1,lat2,lng1,lng2;
  Double rlat1,rlat2,rlng1,rlng2;
  Double x,y;

  if(argn<2) {
    printf("Calling sequence: dist pos-1 pos-2\n");
    exit();
    }
  if(argn<3) {
    printlocs();
    exit();
    }
  i= fetchpos(&lname1,&lat1,&lng1,&argv[1]);
  rlat1= angles(lat1,"deg","rad");
  rlng1= angles(lng1,"deg","rad");
  fetchpos(&lname2,&lat2,&lng2,&argv[1+i]);
  rlat2= angles(lat2,"deg","rad");
  rlng2= angles(lng2,"deg","rad");
  sptriang(rlng1-rlng2,PI/2.0-rlat1,PI/2.0-rlat2,&x);
  y= x*REARTH;
  printf("%s (%.3lf,%.3lf) and %s (%.3lf,%.3lf) are \n",
    lname1,lat1,lng1,lname2,lat2,lng2);
  printf("%.3lf radians, %.3lf degrees, %.3lf km, %.3lf miles apart\n",
    x,(180.0/PI)*x,y,y*KMTOMI);
  exit();
}

