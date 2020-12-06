#include "bodydefs.h"



/*
 *  (c) Copyright, 1989-1995, John L. Manferdelli.  All Rights Reserved
 */



// ---------------------------------------------------------------------- 


int      iNumBodies;      //  Number of bodies
Body*    Bodies;        //  Base of Bodies
double    xTimeStep;      //  One step time
F3Point    P;          //  Point in proj plane perp to plane
F3Point    Xa;          //  X axis in projective Plane
F3Point    Ya;          //  Y axis in projective Plane
FrPoint    E;          //  Position of viewer Perspective



// -----------------------------------------------------------------------


//    Geometric Transformations


void Body::ReScale(double xXFactor,xYFactor)

{
  int i;
    
}


void  Body::Perspective_transform(F3Point& const E, F3Point& const P,
    F3Point& const Xa, F3Point& const Ya)

// E is Observer's Eye.
// P is point at base of normal defining plane (perp to defining plane).
// Xa is the X axis in the Plane
// Ya is the Y axis in the Plane 
         
{
  double d, t;
  F3Point I;    // I is intersection Point at Plane.

  d= (E.x-P.x)*(E.x-XPosition.x)+(E.y-P.y)*(E.y-XPosition.y)+(E.z-P.z)*(E.z-XPosition.z);
  if(d==0.0)
    return;
  t= (-1.0)*((E.x-P.x)*(XPosition.x-P.x)+(E.y-P.y)*(XPosition.y-P.y)+(E.z-P.z)*(XPosition.z-P.z))/d;
  I.x= XPosition.x+t*(E.x-XPosition.x);
  I.y= XPosition.y+t*(E.y-XPosition.y);
  I.z= XPosition.z+t*(E.z-XPosition.z);
  dsDisplay.x= (I.x-P.x)*Xa.x+(I.y-P.y)*Xa.y+(I.z-P.z)*Xa.z;
  dsDisplay.y= (I.x-P.x)*Ya.x+(I.y-P.y)*Ya.y+(I.z-P.z)*Ya.z;
  return;
} 


void Body::Display()

{
  }


BOOL  Body::Serialize(int fInOrOut,int iFile)

{
  }


// ---------------------------------------------------------------------------------------


//      Data Persistance



SerializeAll(int iInOrOut,char* pstrFile)


{
  
}


// ---------------------------------------------------------------------------------------


void f(double k,F3Point& const xP1,F3Point& const xP2,
     double* ax,double* ay,double* az)

{
  double x,y,z,d,d3;
  double sqrt();

  x= xP1.x-xP2.x;
  y= xP1.y-xP2.y;
  z= xP1.z-Xp2.z;
  d= x*x+y*y+z*z;
  d= sqrt(d);
  x/= d;
  y/= d;
  z/= d;
  d3= d*d;
  *ax= k*x/d3;
  *ay= k*y/d3;
  *az= k*z/d3;
}


fullstep(int n, double dt, double k,    // number of bodies, timestep, constant
     Body *Bodies[],
     F3Point NewPosition[],
     F3Point NewVelocity[])

{
  int i;

  double bx,by,bz;

  for(i=0;i<n;i++) {
    NewVelocity[i].x= Bodies[i]->xVelocity.x;
    NewVelocity[i].y= Bodies[i]->xVelocity.y;
    NewVelocity[i].z= Bodies[i]->xVelocity.z;
    for(j=0;j<n;j++) {
      if(i==j)
        continue;
      f(k[i],Bodies[i]->XPosition,Bodies[j]->xPosition,&bx,&by,&bz);
      NewVelocity[i].x+= dt*bx;
      NewVelocity[i].y+= dt*by;
      NewVelocity[i].z+= dt*bz;
      }
    NewPosition[i].x= Bodies[i]->xPosition.x+dt*(Bodies[i]->xVelocity.x+NewVelocity[i].x)/2;
    NewPosition[i].y= Bodies[i]->xPosition.y+dt*(Bodies[i]->xVelocity.y+NewVelocity[i].y)/2;
    NewPosition[i].z= Bodies[i]->xPosition.z+dt*(Bodies[i]->xVelocity.z+NewVelocity[i].z)/2;
    }

  for(i=0;i<n;i++) {
    Bodies[i]->xTime+= dt;
    Bodies[i]->xPosition.x= NewPosition[i].x;
    Bodies[i]->xPosition.y= NewPosition[i].y;
    Bodies[i]->xPosition.z= NewPosition[i].z;
    Bodies[i]->xVelocity.x= NewVelocity[i].x;
    Bodies[i]->xVelocity.y= NewVelocity[i].y;
    Bodies[i]->xVelocity.z= NewVelocity[i].z;
    }
  
  return;
}


/* ---------------------------------------------------------------------- */


void simulate(int n, Body* Bodies[], int iNumSteps, double xDeltaT, double k)

//    multibody problem


{

  int i,j;
  double t,dt;

  for(i=0;i<iNumSteps;i++) {
    }

  
  return;
}


/* ---------------------------------------------------------------------- */




