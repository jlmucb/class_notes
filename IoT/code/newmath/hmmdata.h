
//
//      Hidden Markov data 
//      File: hmmdata.h
//      (c) 2007, John L. Manferdelli
//
//      n states [0,..., n-1]
//      m values [0, 1, ..., m-1]
//      T observations [O[0], ..., O[T-1]
//
//      Pi[0,..,n-1] - initial state distribution
//      P[n,n] - state transition  P[i,j]= Pr[j|i]
//      Q[n,m] - output from state distribution Q[i,j]= Pr[j|i]


// --------------------------------------------------------------------------------

#ifndef HMM_DATA_H
#define HMM_DATA_H

#define NSTATES 2
#define NVALUES 3
#define NOBSERVATIONS 4

int g_nStates= NSTATES;
int g_mValues= NVALUES;
int g_TObservations= NOBSERVATIONS;
double g_rgPi[NSTATES]= {0.6, 0.4};
double g_rgP[NSTATES*NSTATES]= {0.7, 0.3, 0.4, 0.6};
double g_rgQ[NSTATES*NVALUES]= {0.1, 0.4, 0.5, 0.7, 0.2, 0.1};
int g_rgO[NOBSERVATIONS]= {0,1,0,2};
#endif


// --------------------------------------------------------------------------------



