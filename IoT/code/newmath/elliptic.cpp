#include <stdio.h> 
#include <string.h>
#include <io.h>
#include <stdlib.h> 


// ---------------------------------------------------------------------------------------

class ECurve {
    //  E(p,a,b): y**2= x**3 + ax + b  (mod p)
public:
    int     m_p;
    int     m_a, m_b;
    int     m_disc;

    ECurve(int p, int a, int b) {m_p= p; m_a= a; m_b= b; 
                                 m_disc= -((16*(4*a*a*a+27*b*b))%p);};
    ~ECurve() {};
    int getDisc() {return m_disc;};
};

class Point {
public:
    bool    m_isZero;
    int     m_x, m_y;
    Point() {m_isZero= false; m_x=0; m_y=0;};
    Point(bool isZero, int x, int y) {m_isZero= isZero; m_x= x; m_y= y;};
};


//  Elliptic Curve Calculator
//
//  E(p,a,b): y**2= x**3 + ax + b  (mod p)
//      0. What is discrimminant
//      1. Add P1, P2, calculate nP
//      2. Are (x,y) on E?
//      3. Given x, what is y if it exists
//      4. Embed message in curve
//      5. Encrypt
//      6. Decrypt

//
// Add
//
// m= (y2-y1)/(x2-x1) if P1 != P2, m= (3x**2)/(2y1) if P1=P2
// P3=(m**2-x1-x2), m(x1-x3)-y1)


// -------------------------------------------------------------------


inline void Euclid(int iA, int iB, int* piQ, int* piR)
{
        *piQ= iA/iB;
        *piR= iA-(*piQ*iB);
}


//    Function: bool ExtendedGCD
//        (iA, iB)= *piA iA + *piB iB = *piG
//            iA>=iB

bool ExtendedGCD(int iA, int iB, int* piA, int* piB, int* piGCD)
{
      int ia, ib, iaold, ibold, ianew, ibnew, ir, irold, irnew, iq;

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


int getInverse(int x, int p)
{
    int a, b, g;

    if(x>p)
        ExtendedGCD(x, p, &a, &b, &g);
    else
        ExtendedGCD(p, x, &b, &a, &g);
    return(a*g);
}


int Legendre(int a, int p)
{
    int x, y, L;
    int m;

    if((a%p)==0)
        return(0);
    x= a;
    y= p;
    L= 1;
    for(;;) {
        x= x%y;
        if(x>(y/2)) {
            x= y-x;
            if((y%4)==3)
                L= -L;
        }
        while((x%4)==0)
                x/= 4;
        if((x%2)==0) {
            x/= 2;
            m= y%8;
            if(m==3 || m==5)
                L= -L;
        }
        if(x==1)
            return(L);
        if(x%4==3 && y%4==3)
            L= -L;
        m= x; x= y; y=m;
    }
}


int modpow(int x, int y, int p)
{
    int s= x;
    int iHigh= 0;
    int t= y;
    int i;
    int Acc= 1;

    for(i=1; i<=32; i++) {
        if(t&1)
            iHigh= i;
        t>>= 1;
    }

    t= y;
    for(i=0; i<iHigh;i++) {
        if(t&1)
            Acc= (Acc*s)%p;
        s= (s*s)%p;
    }
    return(Acc);
}


int SquareRoot(int a, int p)
{
    int x, y, n;
    int e;
    int q= p-1;
    int t, r, m, b, k, j;

    if(a==0)
        return(0);

    // choose random n
    for(n=1; n<a; n++) {
        if(Legendre(n,p)==(-1))
            break;
    }

    e= 0;
    t= q;
    while(t&1==0) {
        e++;
        t>>= 1;
    }
    y= modpow(n, t, p); 
    r= e;  
    x= modpow(a, (q-1)/2, p);
    b= (a*x*x)%p;  
    x= (a*x)%p;
    while(b!=1) {
        m= 1;
        for(;;) {
            k= 1<<m;
            j= modpow(b,k,p);
            if(j==1)
                break;
            m++;
        }
        if(m==r)
            return -1;
        k= 1<<(r-m-1);
        t= modpow(y,k,p);
        y= (t*t)%p;  
        r= m;
        x= (x*t)%p; 
        b= (b*y)%p;
        }

    return(x);
}


int LongCountPoints(ECurve oC)
{
    int p= oC.m_p;
    int i, t, l, n;

    n= p+1;
    for(i=0;i<p;i++) {
        l= (i*i*i + oC.m_a*i + oC.m_b)%p;
        t= Legendre(l,p);
        n+= t;
    }

    return(n);
}


bool isOnCurve(ECurve oC, Point P)
{
    int t= (P.m_x*P.m_x*P.m_x+oC.m_a*P.m_x+oC.m_b)%oC.m_p;

    return(((P.m_y*P.m_y-t)%oC.m_p) == 0);
}


bool DoublePoint(ECurve oC, Point P, Point& Q)
{
    int m, k;

    if(P.m_isZero || P.m_y==0) {
        Q.m_isZero= true;
        return true;
    }

    k= getInverse(2*P.m_y, oC.m_p);
    m= ((3*P.m_x*P.m_x+oC.m_a)*k)%oC.m_p;
    Q.m_isZero= false;
    Q.m_x= (m*m - 2*P.m_x)%oC.m_p;
    Q.m_y= (m*(P.m_x-Q.m_x) - P.m_y)%oC.m_p;

    return true;
}


bool AddPoints(ECurve oC, Point P1, Point P2, Point& Q)
{   
    int m, k;

    if(P1.m_isZero) {
        Q.m_isZero= P2.m_isZero;
        Q.m_x= P2.m_x;
        Q.m_y= P2.m_y;
        return true;
    }

    if(P2.m_isZero) {
        Q.m_isZero= P1.m_isZero;
        Q.m_x= P1.m_x;
        Q.m_y= P1.m_y;
        return true;
    }

    if(P1.m_x==P2.m_x) {
        if(P1.m_y==P2.m_y)
            return(DoublePoint(oC, P1, Q));
        Q.m_isZero= true;
        return(true);
    }

    k= getInverse(P2.m_x-P1.m_x, oC.m_p);
    m= ((P2.m_y-P1.m_y)*k)%oC.m_p;
    Q.m_isZero= false;
    Q.m_x= (m*m - P1.m_x -P2.m_x)%oC.m_p;
    Q.m_y= (m*(P1.m_x-Q.m_x) - P1.m_y)%oC.m_p;

    return true;
}


bool MultiplyPoint(ECurve oC, Point P, int n, Point& Q)
{
    int i, iHighBit, tn;

    if(n==0) {
        Q.m_isZero= true;
        Q.m_x= 0;
        Q.m_y= 0;
        return true;
    }

    Point Acc(true,0,0);
    Point ProdAcc(P.m_isZero, P.m_x, P.m_y);
    Point tempAcc(true,0,0);
    Point tempProdAcc(P.m_isZero, P.m_x, P.m_y);

    // get high bit to avoid useless doubling
    iHighBit= 0;
    tn= n;
    for(i=1;i<=32; i++) {
        if(tn&1)
            iHighBit= i;
        tn>>= 1;
    }

    // 32bit warning
    for(i=0;i<(iHighBit-1); i++) {
        if(n&1) {
            tempAcc.m_isZero= Acc.m_isZero;
            tempAcc.m_x= Acc.m_x;
            tempAcc.m_y= Acc.m_y;
            AddPoints(oC, ProdAcc, tempAcc, Acc);
        }
        n>>= 1;
        DoublePoint(oC, ProdAcc, tempProdAcc);
        ProdAcc.m_isZero= tempProdAcc.m_isZero;
        ProdAcc.m_x= tempProdAcc.m_x;
        ProdAcc.m_y= tempProdAcc.m_y;
    }
    AddPoints(oC, ProdAcc, Acc, Q);

    return true;
}


// ---------------------------------------------------------------------



void printPoint(Point P)
{
    if(P.m_isZero) {
        printf("O");
        return;
    }
    printf("(%d, %d)", P.m_x, P.m_y);
    return;
}


void skipWhite(char** q)
{
    char *p =*q;
    while(*p==' ' || *p=='\t' || *p=='\"')
        p++;
    *q= p;
}


void getComma(char** q)
{
    char *p =*q;
    while(*p!='\0') {
        if(*p==',')
            break;
        p++;
    }
    *q= p;
}


void readPoint(char* p, Point& P)
{
    char* r;
    char* s;
    int  a, b;

    // should be O or (n,m)
    if(strcmp(p,"O")==0) {
        P.m_isZero= true;
        return;
    }
    P.m_isZero= false;
    skipWhite(&p);
    if(*p!='(')
        return;
    p++;
    skipWhite(&p);
    r= p;
    getComma(&p);
    *p= '\0';
    p++;
    skipWhite(&p);
    s= p;
    while(*p!=')' && *p!='\0')
        p++;
    *p= '\0';
    sscanf(r, "%d", &a);
    sscanf(s, "%d", &b);
    P.m_x= a;
    P.m_y= b;
    return;
}


int main(int an, char** av) 
{ 
    int p, a, b;
    int i, n;
    bool fOn= false;
    bool fCount= false;
    bool fAdd= false;
    bool fMultiply= false;


    if(an<4) {
        printf("EC p a b [-On (x,y)|O] [-Count] [-Add (x,y)|O] (x,y)|O]] [-Multiply (x,y)|O n]\n");
        return(1);
    }

    sscanf(av[1], "%d", &p);
    sscanf(av[2], "%d", &a);
    sscanf(av[3], "%d", &b);
    ECurve oC(p,a,b);
    printf("E(%d, %d, %d), discriminant: %d\n", oC.m_p, oC.m_a, oC.m_b, oC.m_disc);

    Point P, P1, P2, Q;
    for(i=4;i<an;i++) {
        if(strcmp(av[i], "-On")==0) {
            fOn= true;
            i++;
            if(an<i)
                return(1);
            readPoint(av[i], P);
            printf("readpoint %d %d\n", P.m_x, P.m_y);
            printPoint(P);
            printf("\n");
            continue;
        }
        if(strcmp(av[i], "-Count")==0) {
            fCount= true;
            continue;
        }
        if(strcmp(av[i], "-Add")==0) {
            fAdd= true;
            i++;
            if(an<i)
                return(1);
            readPoint(av[i], P1);
            i++;
            if(an<i)
                return(1);
            readPoint(av[i], P2);
            continue;
        }
        if(strcmp(av[i], "-Multiply")==0) {
            fMultiply= true;
            i++;
            if(an<i)
                return(1);
            readPoint(av[i], P);
            i++;
            if(an<i)
                return(1);
            sscanf(av[i], "%d", &n);
            continue;
        }
    }

    if(fCount) {
        printf("There are %d points on the curve\n", LongCountPoints(oC));
    }

    if(fOn) {
        printPoint(P);
        if(isOnCurve(oC, P))
            printf(" is on the curve\n");
        else
            printf(" is not on the curve\n");
    }

    if(fAdd) {
        printPoint(P1);
        printf(" + ");
        printPoint(P2);
        printf(" = ");
            AddPoints(oC, P1, P2, Q);
        printPoint(Q);
        printf("\n");
    }

    if(fMultiply) {
        printf("%d *  ", n);
        printPoint(P);
        printf(" = ");
            MultiplyPoint(oC, P, n, Q);
        printPoint(Q);
        printf("\n");
    }
    

    return(0);
}


// ---------------------------------------------------------------------------------------




