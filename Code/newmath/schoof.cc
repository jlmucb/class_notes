//
//  File: schoof.cc
//  Description: Shoof's algorithm
//
//  Copyright (c) 2014, John Manferdelli.  All rights reserved.
//
// Use, duplication and disclosure of this file and derived works of
// this file are subject to and licensed under the Apache License dated
// January, 2004, (the "License").  This License is contained in the
// top level directory originally provided with the CloudProxy Project.
// Your right to use or distribute this file, or derived works thereof,
// is subject to your being bound by those terms and your use indicates
// consent to those terms.
//
// If you distribute this file (or portions derived therefrom), you must
// include License in or with the file and, in the event you do not include
// the entire License in the file, the file must contain a reference
// to the location of the License.

#include "common.h"

/*
 *  g=(x^3+ax+b,x^p-x)
 *  if(g==1)
 *    t= 0 (mod 2)
 *  else
 *    t= 1 (mod 2)
 *   Create a set, S={l_i}, 0<i<=L, such that Prod_{i=1}^L l_i < 4 p^(1/2)
 *   Compute the first l_L+2 division polynomials polyinomials phi_k
 *   For each l in S, compute k=p (mod l)
 *       if (p_{16}, f_l)!=1 then there is a P in E[l]: psi_l^2P= pm kP
 *           if k is not a QR mod l 
 *               t= 0 (mod l)
 *           else 
 *               compute w: w^2=k (mod l)
 *               if (p_{17}, f_l)=1 then t=0 (mod l) else
 *               if (p_{18}, f_l)!=1 then t=2w (mod l) else t= -2w (mod l)
 *      else
 *        for each tau<= (l+1)/2
 *          if (p_{19},f_l)!=1 then 
 *            psi_p^2 +k = tau \psi_p (mod l) for some point in E[l], so
 *            if(p_{19},f_l)!= 1 then t= tau (mod l) else t= - tau (mod l)
 *        next tau
 *  next l
 *  Use CRT to compute T= y (mod N), N= Prod_{i=1}^L l_i
 *  if T is within Hasse;s bounds then t=T else t= -T (mod N) and
 *  #E(F_p)=p+1-t
 */

int main(int an, char**av) {
  return 0;
}


