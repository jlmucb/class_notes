
//
//      Hidden Markov data
//      File: hmm.h - multi-dimensional matrix helpers
//      (c) 2007, John L. Manferdelli
//


// --------------------------------------------------------------------------------

#ifndef HMM_H
#define HMM_H


inline int dIndex(int i, int j, int maxi, int maxj)
{
    return maxj*i+j;
}


inline int tIndex(int i, int j, int k, int maxi, int maxj, int maxk)
{
    return (maxk*((maxj*i)+j)+k);
}


#endif


// --------------------------------------------------------------------------------



