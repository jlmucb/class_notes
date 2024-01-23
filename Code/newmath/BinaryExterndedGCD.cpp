#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Binary extended GCD (HAC)


#ifndef byte
typedef unsigned char byte;
typedef long unsigned u64;
typedef long int i64;
#endif


// --------------------------------------------------------------------- 



bool BinaryExtendedGCD(i64 x, i64 y, i64* pa, i64* pb, i64* pg) {
    i64     g= 1L;
    i64     A= 1L;
    i64     B= 0L;
    i64     C= 0L;
    i64     D= 1L;
    i64     u= 0L;
    i64     v= 0L;

    printf("\nStep 1, x: %ld, y: %ld, g: %ld, u: %ld, v: %ld\n", x,y,g,u,v);
    printf("\tA: %ld, B: %ld, C: %ld, D: %ld\n\n", A,B,C,D);

    while(x!=0 && y!=0 && (x&0x1L)==0 && (y&0x1)==0) {
        g*= 2L; 
        x/= 2L;
        y/= 2L;
    }
    u= x;
    v= y;
    printf("Step 2, x: %ld, y: %ld, g: %ld, u: %ld, v: %ld\n", x,y,g,u,v);
    printf("\tA: %ld, B: %ld, C: %ld, D: %ld\n\n", A,B,C,D);

    for(;;) {
        printf("\n--------------------------\n");
        while(u!=0 && (u&0x1L)==0) {
            u/= 2L;
            if(A!=0 && B!=0 && (A&0x1L)==0 && (B&0x1)==0) {
                A/= 2L;
                B/= 2L;
            }
            else {
                A= (A+y)/2L;
                B= (B-x)/2L;
            }
            printf("Step 4, x: %ld, y: %ld, g: %ld, u: %ld, v: %ld\n", x,y,g,u,v);
            printf("\tA: %ld, B: %ld, C: %ld, D: %ld\n\n", A,B,C,D);
        }
        while(v!=0 && (v&0x1L)==0) {
            v/= 2L;
            if(C!=0 && D!=0 && (C&0x1L)==0 && (D&0x1)==0) {
                C/= 2L;
                D/= 2L;
            }
            else {
                C= (C+y)/2L;
                D= (D-x)/2L;
            }
            printf("Step 5, x: %ld, y: %ld, g: %ld, u: %ld, v: %ld\n", x,y,g,u,v);
            printf("\tA: %ld, B: %ld, C: %ld, D: %ld\n\n", A,B,C,D);
        }

        if(u>=v) {
            u= u-v;
            A= A-C;
            B= B-D;
        }
        else {
            v= v-u;
            C= C-A;
            D= D-B;
        }
        printf("Step 6, x: %ld, y: %ld, g: %ld, u: %ld, v: %ld\n", x,y,g,u,v);
        printf("\tA: %ld, B: %ld, C: %ld, D: %ld\n\n", A,B,C,D);

        if(u==0L) {
            *pa= C;
            *pb= D;
            *pg= v*g;
            printf("Step 7, a: %ld, b: %ld\n", C, D);
            break;
        }
    }

    return true;
}


// -------------------------------------------------------------------------




int main(int an, char** av) 

{
    i64 x, y;
    i64 a= 0L;
    i64 b= 0L;
    i64 g= 0L;

    if(an<3) {
        printf("BinaryExtendedGCD wrong number of args, call BEGCD a b\n");
        return 1;
    }
    sscanf(av[1], "%ld", &x);
    sscanf(av[2], "%ld", &y);
    printf("BinaryExtendedGCD (%ld, %ld)\n", x, y);

    if(!BinaryExtendedGCD(x, y, &a, &b, &g)) {
        printf("BinaryExtendedGCD failed\n");
        return 1;
    }
    printf(" %ld (%ld) + %ld (%ld) = %ld\n", a,x,b,y,g);
    return 0;
}


// ---------------------------------------------------------------------------


