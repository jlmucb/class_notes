#include "windt.h"



//
//  projective transformations
//
//    Observer: (x0,y0,z0)
//    Plane: (x,y,z) such that (px-x0,py-y0,pz-z0) perp (px-x,py-y,pz-z)
//


int proj(Double x, Double y, Double z, Double px, Double py, Double pz,
         Double x0, Double y0, Double z0, Double *xx, Double *yy, Double *zz)

{
  Double l,d;

  d= (x0-px)*(x0-x)+(y0-py)*(y0-y)+(z0-pz)*(z0-z);

  if(d!=0.0)
    return(-1);

  l= (px-x0)*(px-x0)+(py-y0)*(py-y0)+(pz-z0)*(pz-z0);
  l/= d;
  *xx= l*(x-x0)+x0;
  *yy= l*(y-y0)+y0;
  *zz= l*(z-z0)+z0;

  return(1);
}


