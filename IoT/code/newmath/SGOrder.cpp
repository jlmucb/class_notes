#include <stdio.h>
#include <string.h>

typedef int                 i32;
typedef unsigned            u32;
#ifdef  GCC
typedef long long unsigned  u64;
typedef long long int       i64;
#else
typedef unsigned __int64    u64;
typedef __int64             i64;
#endif

/*
 * Name     Parameters      Order
 * Zp       p, prime        n
 * An       n>4             n!/2
 * PSL      n,q             {\frac 1 {(n,q-1)}} (q^{n}-1)(q^{n}-q)(q^{n}-q^{2})...
 *                              (q^{n}-q^{n-1})$, simple if n>2 or q>4
 * PSp      n=l/2, q        {\frac 1 {(2,q-1)}} * q^{l^2}(q^{2} -1) (q^{4} -1) ...
 *                              (q^{2l} -1), simple unless (n,q)= (2,2), (2,3), (4,2)
 * PSU      n,q^2           {\frac 1 {(n,q+1)}} (q^{\frac {n(n-1)} 2}-1) (q^{2}-1)(q^{3} +1) 
 *                            q^4-1)...(q^{n}-(-1)^n), simple unless (n,q^2)= (2,4),(2,9),(3,4)
 * PSO      n=2l+1,q        {\frac 1 {(n,q-1)}} q^{l^2}(q^{2}-1)(q^{4}-1)...(q^{2l}-1),
 *                            simple if l>1.
 * PSO+     n=2l,q          see below, s=1
 * PSO-     n=2l,q          see below, s=-1
 *                          1/(4,q^l-s)} q^{l(l-1)}(q^{2}-1)(q^{4}-1)...
 *                              (q^{2l-2}-1)(q^{l}-s) q=2^k, simple if l>2 otherwise if l>1.
 * E6       q               1/(3,q-1) q^{36}(q^{12}-1)(q^{9}-1)(q^{8}-1)(q^{6}-1)
 *                              (q^{5}-1)(q^{2}-1)
 * E7       q               ${\frac 1 {(3,q-1)}} q^{63} (q^{18}-1) (q^{14}-1) (q^{12}-1) 
 *                          (q^{10}-1) (q^{8}-1) (q^{6}-1) (q^{2}-1)$, 
 * E8       q               $q^{120} (q^{30}-1) (q^{24}-1) (q^{20}-1) (q^{18}-1) (q^{14}-1) 
 *                           (q^{12}-1) (q^{8}-1) (q^{2}-1)$, 
 * F4       q               $q^{24} (q^{12}-1) (q^{8}-1) (q^{6}-1) (q^{2}-1)$, 
 * G2       q               $q^{6} (q^{6}-1)(q^{2}-1)$, 
 * tD4      q               $q^{12} (q^{8}+q^4+1) (q^{6}-1) (q^{2}-1)$, 
 * tE6      q               $q^{36} (q^{12}-1) (q^{9}+1) (q^{8}-1) (q^{6}-1) (q^{2}-1)$, 
 * Sz       n               $q^{2} (q^{2}+1) (q-1)$, 
 * tF4      n               $q^{12} (q^{6}+1) (q^{4}-1) (q^{3}+1) (q-1)$, 
 * tG2      n               $q^{3} (q^{3}+1) (q-1)$, 
 *
 * Usage:  SimpleGroup [-all] -name -p nn nn
 */


const int g_iNumGroups= 18;

const int g_Zp= 1;
const int g_An= 2;
const int g_PSL= 3;
const int g_PSp= 4;
const int g_PSU= 5;
const int g_PSO= 6;
const int g_PSOP= 7;
const int g_PSOM= 8;
const int g_E6= 9;
const int g_E7= 10;
const int g_E8= 11;
const int g_F4= 12;
const int g_G2= 13;
const int g_tD4= 14;
const int g_tE6= 15;
const int g_Sz= 16;
const int g_tF4= 17;
const int g_tG2= 18;

char* g_szGroupNames[g_iNumGroups] = {
    "Zp",
    "An",
    "PSL",
    "PSp",
    "PSU",
    "PSO",
    "PSO+",
    "PSO-",
    "E6",
    "E7",
    "E8",
    "F4",
    "G2",
    "tD4",
    "tE6",
    "Sz",
    "tF4",
    "tG2"
};

const i32  s_iSizeofFirstPrimes= 512;
u32 rgFirstPrimes[s_iSizeofFirstPrimes]= {
2,    3,    5,    7,   11,   13,   17,   19,
23,   29,   31,   37,   41,   43,   47,   53,
59,   61,   67,   71,   73,   79,   83,   89,
97,  101,  103,  107,  109,  113,  127,  131,
137,  139,  149,  151,  157,  163,  167,  173,
179,  181,  191,  193,  197,  199,  211,  223, 
227,  229,  233,  239,  241,  251,  257,  263, 
269,  271,  277,  281,  283,  293,  307,  311, 
313,  317,  331,  337,  347,  349,  353,  359, 
367,  373,  379,  383,  389,  397,  401,  409, 
419,  421,  431,  433,  439,  443,  449,  457, 
461,  463,  467,  479,  487,  491,  499,  503, 
509,  521,  523,  541,  547,  557,  563,  569, 
571,  577,  587,  593,  599,  601,  607,  613, 
617,  619,  631,  641,  643,  647,  653,  659, 
661,  673,  677,  683,  691,  701,  709,  719, 
727,  733,  739,  743,  751,  757,  761,  769, 
773,  787,  797,  809,  811,  821,  823,  827, 
829,  839,  853,  857,  859,  863,  877,  881, 
883,  887,  907,  911,  919,  929,  937,  941, 
947,  953,  967,  971,  977,  983,  991,  997, 
1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049,
1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097,
1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163,
1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223,
1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283,
1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321,
1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423,
1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459,
1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511,
1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571,
1579, 1583, 1597, 1601, 1607, 1609, 1613, 1619,
1621, 1627, 1637, 1657, 1663, 1667, 1669, 1693,
1697, 1699, 1709, 1721, 1723, 1733, 1741, 1747,
1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811,
1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877,
1879, 1889, 1901, 1907, 1913, 1931, 1933, 1949,
1951, 1973, 1979, 1987, 1993, 1997, 1999, 2003,
2011, 2017, 2027, 2029, 2039, 2053, 2063, 2069,
2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129,
2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203,
2207, 2213, 2221, 2237, 2239, 2243, 2251, 2267,
2269, 2273, 2281, 2287, 2293, 2297, 2309, 2311,
2333, 2339, 2341, 2347, 2351, 2357, 2371, 2377,
2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423,
2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503,
2521, 2531, 2539, 2543, 2549, 2551, 2557, 2579,
2591, 2593, 2609, 2617, 2621, 2633, 2647, 2657,
2659, 2663, 2671, 2677, 2683, 2687, 2689, 2693,
2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741,
2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801,
2803, 2819, 2833, 2837, 2843, 2851, 2857, 2861,
2879, 2887, 2897, 2903, 2909, 2917, 2927, 2939,
2953, 2957, 2963, 2969, 2971, 2999, 3001, 3011,
3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079,
3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167,
3169, 3181, 3187, 3191, 3203, 3209, 3217, 3221,
3229, 3251, 3253, 3257, 3259, 3271, 3299, 3301,
3307, 3313, 3319, 3323, 3329, 3331, 3343, 3347,
3359, 3361, 3371, 3373, 3389, 3391, 3407, 3413,
3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491,
3499, 3511, 3517, 3527, 3529, 3533, 3539, 3541,
3547, 3557, 3559, 3571, 3581, 3583, 3593, 3607,
3613, 3617, 3623, 3631, 3637, 3643, 3659, 3671
};



bool isPrime(u64 iP) 
{
    if(iP==1)
        return(true);
    for(int i=0; i< s_iSizeofFirstPrimes;i++)
        if(rgFirstPrimes[i]==iP)
            return(true);
    if(iP<3672)
        return(false);
    return(true);
}


void Euclid(i64 iA, i64 iB, i64* piQ, i64* piR)
{
    *piQ= iA/iB;
    *piR= iA-(*piQ*iB);
}


bool ExtendedGCD(i64 iA, i64 iB, i64* piA, i64* piB, i64* piGCD)
{
    i64 ia, ib, iaold, ibold, ianew, ibnew, ir, irold, irnew, iq;

    if(iA<iB)
        return(false);
    irold=  iA;
    iaold=  1;
    ibold=  0;
    ia= 0;
    ib= 1;
    ir= iB;
    irnew=  0;
    for(;;) {
        Euclid(irold, ir, &iq, &irnew);
        if(irnew==0)
            break;
        ianew= iaold-iq*ia;
        ibnew= ibold-iq*ib;
        iaold= ia; ibold= ib; irold= ir;
        ia= ianew; ib= ibnew; ir= irnew;
        }
    
    *piA= ia;
    *piB= ib;
    *piGCD= ir;
    return(true);
}


u64 factorial(u64 i1) 
{
    u64 iProd= 1;
    u64 iIndex= i1;

    while(iIndex>1) {
        iProd*= iIndex;
        iIndex--;
    }
    return(iProd);
}

int GroupId(char* szGroupName)
{
    if(strcmp("Zp", szGroupName)==0) {
        return(g_Zp);
    }
    if(strcmp("An", szGroupName)==0) {
        return(g_An);
    }
    if(strcmp("PSL", szGroupName)==0) {
        return(g_PSL);
    }
    if(strcmp("PSp", szGroupName)==0) {
        return(g_PSp);
    }
    if(strcmp("PSU", szGroupName)==0) {
        return(g_PSU);
    }
    if(strcmp("PSO", szGroupName)==0) {
        return(g_PSO);
    }
    if(strcmp("PSO+", szGroupName)==0) {
        return(g_PSOP);
    }
    if(strcmp("PSO-", szGroupName)==0) {
        return(g_PSOM);
    }
    if(strcmp("E6", szGroupName)==0) {
        return(g_E6);
    }
    if(strcmp("E7", szGroupName)==0) {
        return(g_E7);
    }
    if(strcmp("E8", szGroupName)==0) {
        return(g_E8);
    }
    if(strcmp("F4", szGroupName)==0) {
        return(g_F4);
    }
    if(strcmp("G2", szGroupName)==0) {
        return(g_G2);
    }
    if(strcmp("tD4", szGroupName)==0) {
        return(g_tD4);
    }
    if(strcmp("tE6", szGroupName)==0) {
        return(g_tE6);
    }
    if(strcmp("Sz", szGroupName)==0) {
        return(g_Sz);
    }
    if(strcmp("tF4", szGroupName)==0) {
        return(g_tF4);
    }
    if(strcmp("tG2", szGroupName)==0) {
        return(g_tG2);
    }
    return(0);
}


u64 CalculateOrderZp (u64 i1, u64 i2) 
{
    if(!isPrime(i1)) {
           printf("Cyclic Group of order %ld which is not prime so group is not simple\n", i1);
           return(0);
    }
    printf("Simple cyclic group of order %ld\n", i1);
    return(1);
}


u64 CalculateOrderAn (u64 i1, u64 i2) 
{
    u64 iOrder= factorial(i1)/2;
    if(i1<5) {
           printf("Alternating group of order %ld which is not simple\n", iOrder);
           return(0);
    }
    printf("Simple alternating group of order %ld\n", iOrder);
    return(1);
}


u64 CalculateOrderPSL (u64 i1, u64 i2) 
//
//      |PSL(n,q)|=  1/(n,q-1) 1/(q-1) (q^{n}-1)(q^{n}-q)(q^{n}-q^{2})...(q^{n}-q^{n-1})
//      simple if n>2 or q>3
//
{
    i32 n= (i32) i1;
    i32 i;
    i64 iN= (i64) i1;
    i64 iQ= (i64) i2;
    i64 iQM1= iQ-1;
    i64 iQP= 1;
    i64 iQLastP= 1;
    i64 iAccum= 1;
    i64 iNC, iQM1C, iGCD;
    u64 iOrder;

    if(iN<2 || iQ<2)
        return(0);

    for(i=0;i<n;i++)
        iQP*= iQ;

    if(!ExtendedGCD(iN, iQM1, &iNC, &iQM1C, &iGCD) && !ExtendedGCD(iQM1,iN, &iQM1C, &iNC, &iGCD))
        return(0);

    for(i=0; i<n; i++) {
        iAccum*= (iQP-iQLastP);
        iQLastP*= iQ;
    }

    iOrder= (u64) ((iAccum/iQM1)/iGCD);
    if(n>2 || iQ>3)
        printf("PSL(%d, %ld) is simple and has order: ", n, iQ);
    else
        printf("PSL(%d, %ld) is not simple and has order: ", n, iQ);
    printf("%ld\n", iOrder);

    return(iOrder);
}


u64 CalculateOrderPSp (u64 i1, u64 i2) 
{
//  |PSp(n=2l, q)|=  1/(2,q-1) q^{l^2}(q^{2}-1)(q^{4}-1) ...
//      (q^{2l}-1), simple unless (n,q)= (2,2), (2,3), (4,2)
    i32 n= (i32) (i1);
    i32 l= n/2;
    i32 i;
    i64 iQ= (i64) i2;
    i64 iQS= iQ*iQ;
    i64 iQP= 1;
    i64 iAccum= 1;
    i64 iGCD;
    u64 iOrder;

    if(n<2 || iQ<2)
        return(0);
    if(n&1)        // n must be even
        return(0);

    if(iQ&1)
        iGCD= 2;
    else
        iGCD= 1;

    for(i=0; i<(l*l); i++) {
        iAccum*= iQ;
    }

    for(i=0; i<l; i++) {
        iQP*= iQS;
        iAccum*= (iQP-(i64)1);
    }

    iOrder= (u64) (iAccum/iGCD);
    if((n==2 && iQ<4) || (n==4 && iQ==2))
        printf("PSp(%d, %ld) is not simple and has order: ", n, iQ);
    else
        printf("PSp(%d, %ld) is simple and has order: ", n, iQ);
    printf("%ld\n", iOrder);
    return(1);
}


u64 CalculateOrderPSU (u64 i1, u64 i2) 
//  |PSU(n,q^2)|= 1/(n,q+1) (q^{(n(n-1))/2}-1)(q^{2}-1)(q^{3}+1)...(q^4-1)...(q^{n}-(-1)^n),
//       simple unless (n,q^2)= (2,4),(2,9),(3,4)
{
    i32 n= (i32) i1;
    i32 m= (n*(n-1))/2;
    i32 i;
    i64 iN= (i64) i1;
    i64 iQ= (i64) i2;
    i64 iQP1= iQ + 1;
    i64 iQP= 1;
    i64 iQLastP= 1;
    i64 iAccum= 1;
    i64 iNC, iQP1C, iGCD;
    u64 iOrder;

    if(iN<2 || iQ<2)
        return(0);

    if(!ExtendedGCD(iN, iQP1, &iNC, &iQP1C, &iGCD) && !ExtendedGCD(iQP1,iN, &iQP1C, &iNC, &iGCD))
        return(0);

    for(i=0; i<m; i++) {
        iAccum*= iQ;
    }
        iAccum-= 1;

    iQP*= iQ;
    for(i=1; i<n; i++) {
        iQP*= iQ;
        if(i&1)
            iAccum*= (iQP-(i64)1);
        else
            iAccum*= (iQP+(i64)1);
    }

    iOrder= (u64) (iAccum/iGCD);

    if((n<4 && iQ==2) || (n==2 && iQ==3))
        printf("PSU(%d, %ld) is not simple and has order: ", n, iQ*iQ);
    else
        printf("PSU(%d, %ld) is simple and has order: ", n, iQ*iQ);
    printf("%ld\n", iOrder);
    return(1);
}


u64 CalculateOrderPSO (u64 i1, u64 i2) 
//  |PSO(n=2l+1,q)|= 1/(n,q-1) q^{l^2}(q^{2}-1)(q^{4}-1)...(q^{2l}-1),
//   simple if l>1.
{
    i32 n= (i32) (i1);
    i32 l= (n-1)/2;
    i32 i;
    i64 iQ= (i64) i2;
    i64 iQS= iQ*iQ;
    i64 iQP= 1;
    i64 iAccum= 1;
    i64 iGCD;
    u64 iOrder;

    if(n<2 || iQ<2)
        return(0);
    if((n&1)==0)        // n must be odd
        return(0);

    if(iQ&1)
        iGCD= 2;
    else
        iGCD= 1;

    for(i=0; i<(l*l); i++) {
        iAccum*= iQ;
    }

    for(i=0; i<l; i++) {
        iQP*= iQS;
        iAccum*= (iQP-(i64)1);
    }

    iOrder= (u64) (iAccum/iGCD);
    if(l==1)
        printf("PSO(%d, %ld) is not simple and has order: ", n, iQ);
    else
        printf("PSO(%d, %ld) is simple and has order: ", n, iQ);
    printf("%ld\n", iOrder);
    return(1);
}


u64 CalculateOrderPSOP (u64 i1, u64 i2) 
//  PSO+(n=2l,q)|= 1/(4,q^l-sgn) q^{l(l-1)}(q^{2}-1)(q^{4}-1)...(q^{2l-2}-1)(q^{l}-sgn), sgn=1
//   simple if l>2 otherwise if l>1.
{
    i32 n= (i32) (i1);
    i32 l= n/2;
    i32 m= l*(l-1);
    i32 i;
    i64 iFOUR= (i64) 4;
    i64 iQEXPL= 1;
    i64 iQ= (i64) i2;
    i64 iQS= iQ*iQ;
    i64 iQP= 1;
    i64 iAccum= 1;
    i64 iFOURC, iQEXPLC, iGCD;
    u64 iOrder;
    i64 iSign= (i64)1;

    if(n<2 || iQ<2)
        return(0);
    if(n&1)        // n must be even
        return(0);


    for(i=0; i<l; i++)
        iQEXPL*= iQ;
    iQEXPL-= iSign;

    if(!ExtendedGCD(iFOUR, iQEXPL, &iFOURC, &iQEXPLC, &iGCD) && 
       !ExtendedGCD(iQEXPL, iFOUR, &iQEXPLC, &iFOURC, &iGCD))
        return(0);

    for(i=0; i<m; i++) {
        iAccum*= iQ;
    }

    for(i=2; i<l; i++) {
        iQP*= iQS;
        iAccum*= (iQP-(i64)1);
    }
    iAccum*= iQEXPL;

    iOrder= (u64) (iAccum/iGCD);
    if(l<2)
        printf("PSO+(%d, %ld) is not simple and has order: ", n, iQ);
    else
        printf("PSO+(%d, %ld) is simple and has order: ", n, iQ);
    printf("%ld\n", iOrder);
    return(1);
}


u64 CalculateOrderPSOM (u64 i1, u64 i2) 
//  |PSO-(n=2l,q)|= 1/(4,q^l-sgn)q^{l(l-1)}(q^{2}-1)(q^{4}-1)...(q^{2l-2}-1)(q^{l}-sgn), sgn=-1
//   simple if l>2 otherwise if l>1.
{
    i32 n= (i32) (i1);
    i32 l= n/2;
    i32 m= l*(l-1);
    i32 i;
    i64 iFOUR= (i64) 4;
    i64 iQEXPL= 1;
    i64 iQ= (i64) i2;
    i64 iQS= iQ*iQ;
    i64 iQP= 1;
    i64 iAccum= 1;
    i64 iFOURC, iQEXPLC, iGCD;
    u64 iOrder;
    i64 iSign= (i64)(-1);

    if(n<2 || iQ<2)
        return(0);
    if(n&1)        // n must be even
        return(0);


    for(i=0; i<l; i++)
        iQEXPL*= iQ;
    iQEXPL-= iSign;

    if(!ExtendedGCD(iFOUR, iQEXPL, &iFOURC, &iQEXPLC, &iGCD) && 
       !ExtendedGCD(iQEXPL, iFOUR, &iQEXPLC, &iFOURC, &iGCD))
        return(0);

    for(i=0; i<m; i++) {
        iAccum*= iQ;
    }

    for(i=2; i<l; i++) {
        iQP*= iQS;
        iAccum*= (iQP-(i64)1);
    }
    iAccum*= iQEXPL;

    iOrder= (u64) (iAccum/iGCD);
    if(l<2)
        printf("PSO-(%d, %ld) is not simple and has order: ", n, iQ);
    else
        printf("PSO-(%d, %ld) is simple and has order: ", n, iQ);
    printf("%ld\n", iOrder);
    return(1);
}


u64 CalculateOrderE6 (u64 i1, u64 i2) 
//  |E6(q)|= 1/(3,q-1) q^{36}(q^{12}-1)(q^{9}-1)(q^{8}-1)(q^{6}-1)(q^{5}-1)(q^{2}-1)
{
    i32 m= 36;
    i32 i;
    i64 iQ= (i64) i1;
    i64 iQP= 1;
    i64 iQM1= iQ-(i32)1;
    i64 iAccum= 1;
    i64 iThree= 3;
    i64 iThreeC, iQM1C, iGCD;
    u64 iOrder;
    i64 iE36,iE12,iE9,iE8,iE6,iE5,iE2;

    if(!ExtendedGCD(iThree, iQM1, &iThreeC, &iQM1C, &iGCD) && 
       !ExtendedGCD(iQM1, iThree, &iQM1C, &iThreeC, &iGCD))
        return(0);

    for(i=1; i<=m; i++) {
        iQP*= iQ;
        if(i==36)
            iE36= iQP;
        if(i==12)
            iE12= iQP;
        if(i==9)
            iE9= iQP;
        if(i==8)
            iE8= iQP;
        if(i==6)
            iE6= iQP;
        if(i==5)
            iE5= iQP;
        if(i==2)
            iE2= iQP;
    }
    iAccum= iE36*(iE12-1)*(iE9-1)*(iE8-1)*(iE6-1)*(iE5-1)*(iE2-1);
    iOrder= (u64) (iAccum/iGCD);
    printf("E6(%ld) is simple and has order: ", iQ);
    printf("%ld\n", iOrder);
    return(1);
}


u64 CalculateOrderE7 (u64 i1, u64 i2) 
//  |E7(q)|= 1/(3,q-1)q^{63}(q^{18}-1)(q^{14}-1)(q^{12}-1)(q^{10}-1)(q^{8}-1)(q^{6}-1)(q^{2}-1)
{
    return(1);
}


u64 CalculateOrderE8 (u64 i1, u64 i2) 
//  |E8(q)= q^{120}(q^{30}-1)(q^{24}-1)(q^{20}-1)(q^{18}-1)(q^{14}-1)
//          (q^{12}-1)(q^{8}-1)(q^{2}-1)
{
    return(1);
}


u64 CalculateOrderF4 (u64 i1, u64 i2) 
//  |F4(q)|= q^{24}(q^{12}-1)(q^{8}-1)(q^{6}-1)(q^{2}-1)
{
    i32 m= 24;
    i32 i;
    i64 iQ= (i64) i1;
    i64 iQP= 1;
    i64 iQM1= iQ-(i32)1;
    u64 iOrder;
    i64 iE24,iE12,iE8,iE6,iE2;

    for(i=1; i<=m; i++) {
        iQP*= iQ;
        if(i==24)
            iE24= iQP;
        if(i==12)
            iE12= iQP;
        if(i==8)
            iE8= iQP;
        if(i==6)
            iE6= iQP;
        if(i==2)
            iE2= iQP;
    }
    iOrder= iE24*(iE12-1)*(iE8-1)*(iE6-1)*(iE2-1);
    printf("F4(%ld) is simple and has order: ", iQ);
    printf("%ld\n", iOrder);
    return(1);
}


u64 CalculateOrderG2 (u64 i1, u64 i2) 
//  |G2(q)|= q^{6}(q^{6}-1)(q^{2}-1)
{
    i32 m= 6;
    i32 i;
    i64 iQ= (i64) i1;
    i64 iQP= 1;
    i64 iQM1= iQ-(i32)1;
    u64 iOrder;
    i64 iE6,iE2;

    for(i=1; i<=m; i++) {
        iQP*= iQ;
        if(i==6)
            iE6= iQP;
        if(i==2)
            iE2= iQP;
    }
    iOrder= iE6*(iE6-1)*(iE2-1);
    printf("G2(%ld) is simple and has order: ", iQ);
    printf("%ld\n", iOrder);
    return(1);
}


u64 CalculateOrdertD4 (u64 i1, u64 i2) 
//  |tD4(q)|= q^{12} (q^{8}+q^4+1)(q^{6}-1)(q^{2}-1)
{
    return(1);
}


u64 CalculateOrdertE6 (u64 i1, u64 i2) 
//  |tE6(q)|=q^{36} (q^{12}-1) (q^{9}+1) (q^{8}-1) (q^{6}-1) (q^{2}-1)
{
    return(1);
}


u64 CalculateOrderSz (u64 i1, u64 i2) 
//  |Sz(q)|= q^{2} (q^{2}+1) (q-1)
{
    i32 i;
    i64 iQ= (i64) i1;
    i64 iQS= iQ*iQ;
    i64 iQM1= iQ-(i32)1;
    u64 iOrder;

    iOrder= iQS*(iQS+1)*(iQ-1);
    printf("Sz(%ld) is simple and has order: ", iQ);
    printf("%ld\n", iOrder);
    return(1);
}


u64 CalculateOrdertF4 (u64 i1, u64 i2) 
//  |tF4(q)|= q^{12} (q^{6}+1) (q^{4}-1) (q^{3}+1) (q-1)
{
    return(1);
}


u64 CalculateOrdertG2 (u64 i1, u64 i2) 
//  |tG2(q)|= q^{3} (q^{3}+1) (q-1)
{
    return(1);
}



int main(int an, char** av)
{
    char*   szGroupName="Zp";
    i32     i= 0;
    u64     iP1= 0;
    u64     iP2= 0;

    if(an>1) {
        for(i=1;i<(an-1); i++) {
            if(strcmp(av[i],"-name")==0 && ++i<an) {
                szGroupName= av[i];
            }
            if(strcmp(av[i],"-p")==0) {
                if(++i<an) {
                    sscanf(av[i],"%ld", &iP1);
                }
                if(++i<an) {
                    sscanf(av[i], "%ld", &iP2);
                }
                continue;
            }
        }
    }
    else {
        printf("SimpleGroup GroupName -name -p nn nn\n");
        return(1);
    }

    if((i=GroupId(szGroupName))>0) {
        switch(i) {
            case g_Zp:
                CalculateOrderZp (iP1, iP2);
                return(0);
            case g_An:
                CalculateOrderAn (iP1, iP2);
                return(0);
            case g_PSL:
                CalculateOrderPSL (iP1, iP2);
                return(0);
            case g_PSp:
                CalculateOrderPSp (iP1, iP2);
                return(0);
            case g_PSU:
                CalculateOrderPSU (iP1, iP2);
                return(0);
            case g_PSO:
                CalculateOrderPSO (iP1, iP2);
                return(0);
            case g_PSOP:
                CalculateOrderPSOP (iP1, iP2);
                return(0);
            case g_PSOM:
                CalculateOrderPSOM (iP1, iP2);
                return(0);
            case g_E6:
                CalculateOrderE6 (iP1, iP2);
                return(0);
            case g_E7:
                CalculateOrderE7 (iP1, iP2);
                return(0);
            case g_E8:
                CalculateOrderE8 (iP1, iP2);
                return(0);
            case g_F4:
                CalculateOrderF4 (iP1, iP2);
                return(0);
            case g_G2:
                CalculateOrderG2 (iP1, iP2);
                return(0);
            case g_tD4:
                CalculateOrdertD4 (iP1, iP2);
                return(0);
            case g_tE6:
                CalculateOrdertE6 (iP1, iP2);
                return(0);
            case g_Sz:
                CalculateOrderSz (iP1, iP2);
                return(0);
            case g_tF4:
                CalculateOrdertF4 (iP1, iP2);
                return(0);
            case g_tG2:
                CalculateOrdertG2 (iP1, iP2);
                return(0);
            default:
                break;
        }
    }
    printf("%s - unknown group\n", szGroupName);
    return(1);

}

