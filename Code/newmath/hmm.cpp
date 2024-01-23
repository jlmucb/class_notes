#include "math.h"
#include "stdio.h"
#include "hmm.h"
#include "hmmdata.h"

//
//      Hidden Markov Re-estimation
//      File: hmm.cpp
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


double g_rgAlpha[NOBSERVATIONS*NSTATES];
double g_rgBeta[NOBSERVATIONS*NSTATES];
double g_rgGamma1[NOBSERVATIONS*NSTATES];
double g_rgGamma2[NOBSERVATIONS*NSTATES*NSTATES];
double g_rgC[NOBSERVATIONS];

#define STEPDEBUG
#define SCALE


// --------------------------------------------------------------------------------


void printMatrix(int n, int rgMatrix[])
{
    int j;

    for(j=0;j<n;j++)
        printf("%04d ", rgMatrix[j]);
    printf("\n");
}


void printDataMatrix(int nIndex, double rgMatrix[], int rgiDimensions[])
{
    int i, j, k, m, n, r;

    if(nIndex<1 || nIndex>3) {
        printf("Can only print 1,2 or 3 dimensional matricies\n");
        return;
        }
    if(nIndex==1) {
        n= rgiDimensions[0];
        for(j=0;j<n;j++)
            printf("%8.4f ", rgMatrix[j]);
        printf("\n");
    }
    if(nIndex==2) {
        m= rgiDimensions[0];
        n= rgiDimensions[1];
        for(i=0;i<m;i++) {
            for(j=0;j<n;j++)
                printf("%8.4f ", rgMatrix[dIndex(i,j,m,n)]);
            printf("\n");
        }
    }
    if(nIndex==3) {
        r= rgiDimensions[0];
        m= rgiDimensions[1];
        n= rgiDimensions[2];
        for(k=0; k<r; k++) {
            printf("Plane %d\n", k);
            for(i=0;i<m;i++) {
                for(j=0;j<n;j++)
                    printf("%8.4f ", rgMatrix[dIndex(i,j,m,n)]);
                printf("\n");
            }
        }
    }
}


// --------------------------------------------------------------------------------


void AlphaPass(int n, int m, int T,
               int O[], double c[], double P[], double Q[], double pi[],
               double alpha[], double beta[], double gamma1[], double gamma2[])
{
    int i, j, t;
        
    // compute alpha_0(i)
    c[0]= 0.0;
    for(i= 0; i<n; i++) {
        alpha[dIndex(0,i,T,n)]= pi[i]*Q[dIndex(O[0],i,n,m)];
        c[0]+= alpha[dIndex(0,i,T,n)];
    }

#ifdef SCALE
    // scale the alpha_0(i)
    c[0]= 1.0/c[0];
#endif

    for(i= 0; i<n; i++)
        alpha[dIndex(0,i,T,n)]*= c[0];

    // compute alpha_t(i)
    for(t=1; t<T; t++) {
        c[t]= 0;
        for(i= 0; i<n; i++) {
            alpha[dIndex(t,i,T,n)]= 0;
            for(j= 0; j<n; j++) 
                alpha[dIndex(t,i,T,n)]+= alpha[dIndex(t-1,j,T,n)]*P[dIndex(j,i,n,n)];
            alpha[dIndex(t,i,T,n)]*= Q[dIndex(i,O[t],n,m)];
            c[t]+= alpha[dIndex(t,i,T,n)];
        }

#ifdef SCALE
        // scale alpha_t(i)
        c[t]= 1.0/c[t];
        for(i= 0; i<n; i++) 
            alpha[dIndex(t,i,T,n)]*= c[t];
#endif
    }
}


void BetaPass(int n, int m, int T,
              int O[], double c[], double P[], double Q[], double pi[],
              double alpha[], double beta[], double gamma1[], double gamma2[])
{
    int i,j,t;

#ifdef SCALE
    // Let beta_{T-1}(i)= 1 scaled by c[T-1]
    for(i= 0; i<n; i++) 
        beta[dIndex(T-1,i,T,n)]= c[T-1];
#else
    // Let beta_{T-1}(i)= 1
    for(i= 0; i<n; i++) 
        beta[dIndex(T-1,i,T,n)]= 1.0;
#endif

    for(t= T-2; t>=0; t--) {
        for(i= 0; i<n; i++) {
            beta[dIndex(t,i,T,n)]= 0.0;
            for(j= 0; j<n; j++)
                beta[dIndex(t,i,T,n)]+= P[dIndex(i,j,n,n)]*
                                        Q[dIndex(j,O[t+1],n,m)]*
                                        beta[dIndex(t+1,j,T,n)];
#ifdef SCALE
            // scale beta_t(i) with same scale factor as alpha_t(i)
            beta[dIndex(t,i,T,n)]*= c[t];
#endif
            }
        }
}


void ComputeGammas(int n, int m, int T,
                   int O[], double c[], double P[], double Q[], double pi[],
                   double alpha[], double beta[], double gamma1[], double gamma2[])
{
    int i,j,t;
    double denom;

    for(t= 0; t<(T-1); t++) {
        denom = 0.0;
        for(i= 0; i<n; i++) {
            for(j= 0; j<n; j++)
                denom+= alpha[dIndex(t,i,T,m)]*P[dIndex(i,j,n,n)]*
                        Q[dIndex(j,O[t+1],n,m)]*beta[dIndex(t+1,j,T,n)];
            }
        for(i= 0; i<n; i++) {
            gamma1[dIndex(t,i,T,n)]= 0.0;
            for(j= 0; j<n; j++) {
                gamma2[tIndex(t,i,j,T,n,n)]= alpha[dIndex(t,i,T,n)]*P[dIndex(i,j,n,n)]*
                                Q[dIndex(j,O[t+1],n,m)]*beta[dIndex(t+1,j,T,n)]/denom;
		gamma1[dIndex(t,i,T,n)]+= gamma2[tIndex(t,i,j,T,n,n)];
            }
        }
    }
}


void ReEstimate(int n, int m, int T,
                int O[], double c[], double P[], double Q[], double pi[],
                double alpha[], double beta[], double gamma1[], double gamma2[])
{
    int i, j, t;
    double denom, numer;

    // re-estimate pi
    for(i= 0; i<n; i++)
        pi[i]= gamma1[dIndex(0,i,T,n)];

    // re-estimate P
    for(i=0; i<n; i++) {
        for(j=0; j<n; j++) {
            numer= 0.0;
            denom= 0.0;
            for(t= 0; t<T; t++) {
                numer+= gamma2[tIndex(t,i,j,T,n,n)];
                denom+= gamma1[dIndex(t,i,T,n)];
                }
            P[dIndex(i,j,n,n)]= numer/denom;
            }
        }

    // re-estimate Q
    for(i=0; i<n; i++) {
        for(j=0; j<m; j++) {
            numer= 0.0;
            denom= 0.0;
            for(t= 0; t<(T-1); t++) {
                if(O[t]==j)
                    numer+= gamma1[dIndex(t,i,T,n)];
                denom+= gamma1[dIndex(t,i,T,n)];
                }
            Q[dIndex(i,j,n,m)]= numer/denom;
            }
        }
}


// Link in libcmt.lib to get log10
#define Infinity 1.0e50


int Maximize(int maxIters)
{
    int     i= 0;
    int     t;
    double  oldLogProb= -Infinity;
    double  logProb= -1.0;
#ifdef STEPDEBUG
    int     rgiDim[3];
#endif

    while((i++)<maxIters) {
        if(logProb<=oldLogProb) {
            return(i);
            }
        oldLogProb = logProb;
        AlphaPass(g_nStates, g_mValues, g_TObservations,
                g_rgO, g_rgC, g_rgP, g_rgQ, g_rgPi,
                g_rgAlpha, g_rgBeta, g_rgGamma1, g_rgGamma2);
#ifdef STEPDEBUG
        printf("Alpha after AlphaPass:\n");
        rgiDim[0]= g_TObservations;
        rgiDim[1]= g_nStates;
        printDataMatrix(2, g_rgAlpha, rgiDim);
        printf("c after AlphaPass:\n");
        rgiDim[0]= g_TObservations;
        printDataMatrix(1, g_rgC, rgiDim);
#endif
        BetaPass(g_nStates, g_mValues, g_TObservations,
                g_rgO, g_rgC, g_rgP, g_rgQ, g_rgPi,
                g_rgAlpha, g_rgBeta, g_rgGamma1, g_rgGamma2);
#ifdef STEPDEBUG
        printf("Beta after BetaPass:\n");
        rgiDim[0]= g_TObservations;
        rgiDim[1]= g_nStates;
        printDataMatrix(2, g_rgBeta, rgiDim);
#endif
        ComputeGammas(g_nStates, g_mValues, g_TObservations,
                g_rgO, g_rgC, g_rgP, g_rgQ, g_rgPi,
                g_rgAlpha, g_rgBeta, g_rgGamma1, g_rgGamma2);
#ifdef STEPDEBUG
        printf("Gamma1 after ComputeGammas:\n");
        rgiDim[0]= g_TObservations;
        rgiDim[1]= g_nStates;
        printDataMatrix(2, g_rgGamma1, rgiDim);
        printf("Gamma2 after ComputeGammas:\n");
        rgiDim[0]= g_TObservations;
        rgiDim[1]= g_nStates;
        rgiDim[2]= g_nStates;
        printDataMatrix(3, g_rgGamma2, rgiDim);
#endif
        ReEstimate(g_nStates, g_mValues, g_TObservations,
                g_rgO, g_rgC, g_rgP, g_rgQ, g_rgPi,
                g_rgAlpha, g_rgBeta, g_rgGamma1, g_rgGamma2);

        logProb= 0.0;
        for(t=0; t<g_TObservations; t++) 
            logProb+= log10(g_rgC[t]);
        logProb= -logProb;
        }

    return(i);
}



int main(int an, char** av) 
{
    int i;
    int rgiDim[2];

    printf("HMM Model\n");
    printf("\tNumber of states: %d\n", g_nStates);
    printf("\tNumber of values: %d\n", g_mValues);
    printf("\tNumber of observations: %d\n", g_TObservations);
    printf("\n");
    
    printf("Initial State Distribution:\n");
    rgiDim[0]= g_nStates;
    printDataMatrix(1, g_rgPi, rgiDim);
    printf("\n");
    printf("Initial State Transitions:\n");
    rgiDim[0]= g_nStates;
    rgiDim[1]= g_nStates;
    printDataMatrix(2, g_rgP, rgiDim);
    printf("\n");
    printf("Initial State-Output Distribution:\n");
    rgiDim[0]= g_nStates;
    rgiDim[1]= g_mValues;
    printDataMatrix(2, g_rgQ, rgiDim);
    printf("\n");
    printf("Observations:\n");
    rgiDim[0]= g_TObservations;
    printMatrix(g_TObservations, g_rgO);
    printf("\n");

    i= Maximize(5);
    if(i<0) {
        printf("Error from Maximize\n");
        return(1);
    }

    printf("%d re-estimation iterations\n", i);
    printf("Re-estimated Initial Distribution:\n");
    rgiDim[0]= g_nStates;
    printDataMatrix(1, g_rgPi, rgiDim);
    printf("\n");
    printf("Re-estimated State Transitions:\n");
    rgiDim[0]= g_nStates;
    rgiDim[1]= g_nStates;
    printDataMatrix(2, g_rgP, rgiDim);
    printf("\n");
    printf("Re-estimated State-Output Distribution:\n");
    rgiDim[0]= g_nStates;
    rgiDim[1]= g_mValues;
    printDataMatrix(2, g_rgQ, rgiDim);
    printf("\n");
    return(0);
}


// --------------------------------------------------------------------------------


