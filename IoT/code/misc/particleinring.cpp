#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>


// particleinring.cpp


#ifndef byte
typedef unsigned char byte;
#endif

#define G 1.0
#define M 1000.0
#define R 5.0
#define N 2000
#define pi 3.141592653589793


class PlanePoint {
public:
    double  x_, y_;
};



/* --------------------------------------------------------------------- */


double Distance(PlanePoint& p1, PlanePoint& p2)
{
    double x= p1.x_-p2.x_;
    double y= p1.y_-p2.y_;

    return sqrt(x*x+y*y);
}


void CirclePoint(double r, double theta, PlanePoint* p)
{
    p->x_= r*cos(theta);
    p->y_= r*sin(theta);
}


double GravForce(double d, double m1, double m2)
{
    return G*m1*m2/(d*d);
}


int main(int an, char** av) 

{
    int             i;
    int             n= N;
    PlanePoint      total_force;
    PlanePoint      unit_vector;
    PlanePoint      object_position;
    PlanePoint      position;
    double          r= R;
    double          t= .75*r;
    double          theta= 0.0;
    double          delta_theta= 2.0*pi/((double)n);
    double          m= 1.0/((double)n);
    double          d;
    double          f;
    int             k= 0;

    if(an>1) {
        k= atoi(av[1]);
        t= ((double)k)/100.0;
        t*= r;
    } 
    
    total_force.x_= 0.0; 
    total_force.y_= 0.0; 
    object_position.x_= 0.0;
    object_position.y_= t;

    for(i=0;i<n;i++) {
        CirclePoint(1.0, theta, &unit_vector);
        position.x_= r*unit_vector.x_;
        position.y_= r*unit_vector.y_;
        d= Distance(object_position, position);
        f= GravForce(d, m, M);
        total_force.x_+= f*unit_vector.x_;
        total_force.y_+= f*unit_vector.y_;
        theta+= delta_theta;
    }

    printf("M: %14.3f, m: %14.3f, G: %14.3f, r: %14.3f\n", M, m, G, r);
    printf("particle at: (%14.3f,  %14.3f)\n", 
           object_position.x_, object_position.y_);
    printf("%d points, total force: (%14.3f,  %14.3f)\n", n, 
           total_force.x_, total_force.y_);
    return(0);
}


/* --------------------------------------------------------------------- */


