#include "windt.h"
#define DEBUG

#ifdef DEBUG
#include "stdio.h"
#endif

//
//    Simplex and Gaussian Elimination.
//      16 Oct 1994. (c) 1994, John Manferdelli.
//


/* ---------------------------------------------------------------------------------- */


//
//  Linear Programming, Simplex method.



//  Minmize
//    SUM from j=1 to n  {c[j] x[j]}
//  Subject to
//    SUM from j=1 to n {a[i,j] x[j]} = b[i],  i= 1,2,3,...m
//  and
//    x[j]>=0
//
//    Note: m constraint equations, r slack variables, must solve C(m+r,r)
//      sets of equations


//  The following is used to select matrix entries.
#define index(ii,jj) ((ii*(NP1))+jj)

// Infinity for comparisons
#define ZM 9.9e100


#define MAXITER 150



/* --------------------------------------------------------------------------------- */



#ifdef DEBUG


void printall(Int m,Int n,Double *c,Double *a,char *bv, int *cn) {
  Int i,j,N,NP1;

  N= m+n;
  NP1= N+1;

  printf("Number of basic variables (n): %ld, Number of constraints (m): %ld \n",n,m);

  // print objective function
  printf("\nObjective Function:\n");
  for(i=0;i<NP1;i++)
    printf("%7.2lf, ",c[i]);
  printf("\n");

  // print basic variable list
  printf("\nBasic variables:\n");
  for(i=0;i<N;i++)
    if(bv[i]!=0)
      printf("%2ld, ",i);
  printf("\n");

     // print Equation names
  printf("\nEquation Names:\n");
  for(i=0;i<m;i++)
    printf("%2d, ",cn[i]);
  printf("\n");

  // print tableau
  printf("\nTableau:\n");
  for(j=0;j<m;j++) {
    for(i=0;i<NP1;i++)
        printf("%7.2lf, ",a[NP1*j+i]);
    printf("\n");
    }

  return;
}
#endif


//    Tableau examines old objective coefficient (oc below).
//    If pivot is found, tableau does "pivot and reduce" step
//    and returns 1.
//    If no pivot can be found, tableau returns 0.
//
//    Tableau matricies (oa,na) have dimension (m) x (n+m+1)
//    Row coefficients are for the n basic and m non basics
//    variables plus the coefficient.  Additional m variables
//    came by introducing slack variables.  Similarly, oc, nc
//    have dimension n+m+1.



//     rp, cp row, column pivot, if successful
//    Find entering and leaving basic variable.
//    Return 1 if successful, 0  otherwise.
Int findpivot(Int n,Int m,Double c[],Double a[],char *bv,int *rp,int *cp) {
  Int N,NP1;
  int i,crp,ccp;
  Double pv,x,y,z,t;

  N= m+n;
  NP1= N+1;
  pv= 0.0;
  for(i=0;i<N;i++) {
    if((bv[i]>=0)&&(c[i]<pv)) {
      ccp= i;
      pv= c[i];
      }
    }

  // if pv >=0, no leaving basic was found
  if(pv>=0)
    return(0);

  // find smallest ratio
  z= ZM;
  for(i=0;i<m;i++) {
    y= a[index(i,N)];
    x= a[index(i,ccp)];
    if((x!=0.0)&&((t=(y/x))>0)&&(t<z)) {
      z= t;
      crp= i;
      }
    }

  if(z>0) {

    *rp= crp;
    *cp= ccp;
    return(1);
    }
  return(0);
}




/*
 *  n- Number of basic (!=0) variables
 *  m- Number of constraints
 *  oa- old tableau coefficients
 *  oc- old objective function coefficients
 *  obv- old basic variable flags
 *  ocn- old column names
 */
Int tableau(Int n,Int m,Double *oa,Double *oc,char *obv,int *ocn, Double *na,Double *nc,char *nbv, int *ncn) {
  Int i,j,N,NP1;
  int rp, cp, ev, lv;
  Double pv,t;

  if((i=findpivot(n,m,oc,oa,obv,&rp,&cp))<=0)
    return(i);

    // for matrix computations
    N= m+n;
    NP1= N+1;

    // change basic variables
    lv= cp;        // leaving
    ev= ocn[rp];      // entering

  // new basic variable map, and column name
  for(i=0;i<N;i++)
      nbv[i]= obv[i];
    for(i=0;i<m;i++)
      ncn[i]= ocn[i];
    ncn[rp]= lv;
    nbv[ev]= 1;
    nbv[lv]= 0;

  // do pivot row (pivot row is rp)
  pv= oa[index(rp,cp)];
  for(i=0;i<NP1;i++) {
    // pivot row is divided by pivot
    t= oa[index(rp,i)]/pv;
    na[index(rp,i)]= t;
    nc[i]= oc[i]-(oc[cp]*t);
    }

  // do remaining coefficients
  // new row= old row-(pivot col coeff)*new pivot row
  for(i=0;i<m;i++) {
    if(i==rp)
      continue;
    for(j=0;j<NP1;j++) {
      if(j==cp)
        continue;
      na[index(i,j)]= oa[index(i,j)]-(oa[index(i,cp)]*na[index(rp,j)]);
      }
    }

  return(1);
}

/*
 *   a: Constraint coefficients
 *   c: Objective function coefficients
 */
Int simplex(Int n,Int r,Double *a,Double *c) {
  Int i,j,k,ws,N,NP1;
  Unsigned mm;
  Double *c1, *c2, *a1, *a2, *v, *m;
  char *bv1, *bv2, *bv;
  int *cn1, *cn2, *cn;
  extern Unsigned malloc();

  // for matrix computations
    N=(r+n);
    NP1= N+1;

  // allocate tableaus and vectors
  if((mm=malloc(NP1*sizeof(Double)))==((Unsigned) NULL))
    return(-1);
  c1= (Double *) mm;
  if((mm=malloc(NP1*sizeof(Double)))==((Unsigned) NULL))
    return(-1);
  c2= (Double *) mm;
  if((mm=malloc(NP1*r*sizeof(Double)))==((Unsigned) NULL))
    return(-1);
  a1= (Double *) mm;
  if((mm=malloc(NP1*r*sizeof(Double)))==((Unsigned) NULL))
    return(-1);
  a2= (Double *) mm;
  if((mm=malloc(r*sizeof(int)))==((Unsigned) NULL))
    return(-1);
  cn1= (int *) mm;
  if((mm=malloc(r*sizeof(int)))==((Unsigned) NULL))
    return(-1);
  cn2= (int *) mm;
  if((mm=malloc(N*sizeof(char)))==((Unsigned) NULL))
    return(-1);
  bv1= (char *) mm;
  if((mm=malloc(N*sizeof(char)))==((Unsigned) NULL))
    return(-1);
  bv2= (char *) mm;

    ws= 1;
    bv= bv1;
    cn= cn1;
    m= a1;
    v= c1;
    for(i=0;i<NP1;i++) {
      bv[i]= (char) (i<n);
      if(i<n)
        v[i]= c[i];
      else
        v[i]= 0.0;
      }
    for(i=0;i<r;i++)
      cn[i]= (int)(n+i);
    v[N]= 0.0;
    for(i=0;i<r;i++) {
      for(j=0;j<N;j++) {
        if(j<n)
          m[index(i,j)]= a[(n+1)*i+j];
        else
            m[index(i,j)]= (i==(j-n));
        }
      m[index(i,N)]= a[(n+1)*i+n];
      }

#ifdef DEBUG
    printf("\nAfter initialization\n");
    printall(r,n,c1,a1,bv1,cn1);
#endif

  // Call tableau until stopping rule
  for(k=0;k<MAXITER;k++) {
    if(ws==1) {
      if(tableau(n,r,a1,c1,bv1,cn1,a2,c2,bv2,cn2)<=0)
        break;
#ifdef DEBUG
         printf("\nAfter step %d\n",k+1);
        printall(r,n,c2,a2,bv2,cn2);
#endif
      ws= 2;
      }
    else if(ws==2) {
      if(tableau(n,r,a2,c2,bv2,cn2,a1,c1,bv1,cn1)<=0)
        break;
#ifdef DEBUG
         printf("\nAfter step %d\n",k+1);
        printall(r,n,c1,a1,bv1,cn1);
#endif
      ws= 1;
      }
    }
  return(ws);
}


#define gindex(ii,jj) (((n+1)*(ii)+jj))


/*
 *  Gausian Elimination
 *   n: number of variables
 *   m: number of equations
 *   a: Constraint coefficients
 *   vn: variable names
 *  Return:
 *    n>0, number of rows reduced
 *    -1 if error
 */

Int gelim(Int n,Int m,Double *a,int *vn) {
  int i,j,k,pv, l;
  Double x,p1,p2;

  for(i=0;i<m;i++) {

    // any more variables?
    if(i>=n)
      break;

    // find pivot in col i
    for(k=0;k<(n-i);k++) {
      for(pv=i;pv<m;pv++)
        if(a[gindex(pv,i+k)]!=0)
          break;
      if(pv<m)
        break;
      }

    // no more pivots in any column?
    if(k>=n)
      return(i);

    // switch columns if necessary
    if(k>0) {
      // switch variable names
      l= vn[i];
      vn[i]= vn[i+k];
      vn[i+k]= l;
      // switch columns
      for(j=0;j<m;j++) {
          x= a[gindex(j,i)];
          a[gindex(j,i)]= a[gindex(j,i+k)];
          a[gindex(j,i+k)]= x;
          }
      }

    // switch pivot row, if necessary
      if(pv!=i) {
        for(j=i;j<=n;j++) {
          x= a[gindex(i,j)];
          a[gindex(i,j)]= a[gindex(pv,j)];
          a[gindex(pv,j)]= x;
          }
        }

      p1= a[gindex(i,i)];
      // reduce pivot row
      for(j=i;j<=n;j++)
         a[gindex(i,j)]/= p1;
      // reduce remaining rows
      for(k=(i+1);k<m;k++) {
        p2= a[gindex(k,j)];
        for(j=i;j<=n;j++)
           a[gindex(k,j)]-= p2*a[gindex(i,j)];
        }
    }

   return(m);
}

