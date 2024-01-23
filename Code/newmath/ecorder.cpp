#include <stdio.h> 
#include <string.h>
#include <io.h>
#include <stdlib.h> 


//
//  ecorder.cpp, elliptic curve computing
//  (C) 2007-2008, John L. Manferdelli
//


// ---------------------------------------------------------------------------------------



class Point {
public:
    bool        m_isZero;
    int         m_x, m_y;

    Point() {m_isZero= false; m_x=0; m_y=0;};
    Point(bool isZero, int x, int y) {m_isZero= isZero; m_x= x; m_y= y;};
    void    copyPoint(Point& P);
    void    printPoint();
};


class infoPoint {
public:
    int     m_iOrder;
    Point   m_oPoint;
};


class ECurve {
    //  E(p,a,b): y**2= x**3 + ax + b  (mod p)
public:
    int         m_p;
    int         m_a, m_b;
    int         m_disc;
    int         m_iPrimitive;
    bool        m_fPrimitiveValid;
    int*        m_piPow;
    int*        m_piIndex;
    bool        m_fPointsInfoValid;
    int         m_iNumPoints;
    infoPoint*  m_rginfoPoints;

    ECurve(int p, int a, int b) {m_p= p; m_a= a; m_b= b; 
                                 m_fPrimitiveValid= false;
                                 m_fPointsInfoValid= false;
                                 m_iNumPoints= 0;
                                 m_disc= -((16*(4*a*a*a+27*b*b))%p);};
    ~ECurve() {};
    void        printCurve();
    bool        calcPrimitive();
    bool        calcLogTables();
    int         getPrimitive();
    int         getIndex(int x);
    int         getPow(int x);
    int         getDisc() {return m_disc;};
    int         aSquareRoot(int x);
    int         calcYsquared(int x) {return (x*x*x+m_a*x+m_b)%m_p;}
    bool        calcPoints();
    bool        sortPoints();
    int         findPoint(Point& P);
    bool        calcOrders();
};


//  Elliptic Curve Order Calculator
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
//


void ECurve::printCurve()
{
    printf("E(%d, %d, %d) y^2= x^3 + (%d)x + %d (mod %d), discriminant: %d", 
                    m_p, m_a, m_b, m_a, m_b, m_p, m_disc);
    if(m_fPrimitiveValid) {
        printf(", %d is primitive (mod %d)", m_iPrimitive, m_p);
    }
    printf("\n");
}


bool  ECurve::calcLogTables()
{
    int y;
    int i;

    if(!m_fPrimitiveValid) {
        return false;
    }
    m_piPow= (int*) malloc(m_p*sizeof(int));
    m_piIndex= (int*) malloc(m_p*sizeof(int));
    for(i=0; i<m_p; i++) {
        m_piIndex[i]= -1;
        m_piPow[i]= -1;
    }

    y= 1;
    m_piPow[0]= 1;
    m_piPow[1]= m_iPrimitive;
    m_piIndex[1]= 0;
    m_piIndex[m_iPrimitive]= 1;
    for(i=1; i<m_p; i++) {
        y= (y*m_iPrimitive)%m_p;
        m_piIndex[y]= i;
        m_piPow[i]= y;
    }
    return true;
}


bool  ECurve::calcPrimitive()
{
    int     pointOrder(ECurve&, Point&);
    bool    isPrimitive(int, int);
    int     i;

    m_fPrimitiveValid= false;
    for(i=2;i<m_p;i++) {
        if(isPrimitive(i,m_p)) {
            m_fPrimitiveValid= true;
            m_iPrimitive= i;
            break;
        }
    }
    if(m_fPrimitiveValid) {
        if(!calcLogTables()) {
            m_fPrimitiveValid= false;
        }
    }
    return m_fPrimitiveValid;
}


int ECurve::aSquareRoot(int x)
{
    int SquareRoot(int x, int p);
    int l;

    if(!m_fPrimitiveValid)
        return SquareRoot(x, m_p);
    if(x==0)
        return 0;
    l= getIndex(x);
    if((l%2)==1)
        return -1;
    return getPow(l/2);
}


int ECurve::getPrimitive()
{
    if(!m_fPrimitiveValid)
        return -1;
    return m_iPrimitive;
}


int ECurve::getIndex(int x)
{
    x= x%m_p;
    if(!m_fPrimitiveValid)
        return -1;
    return m_piIndex[x];
}


int ECurve::getPow(int x)
{
    x= x%m_p;
    if(!m_fPrimitiveValid)
        return -1;
    return m_piPow[x];
}


bool ECurve::calcPoints()
{
    int     p= m_p;
    int     ys, y;
    int     numPoints= 1;  // the identity
    int     nOrder= 0;
    Point   P(true, 0, 0);

    if(m_fPointsInfoValid)
            return true;
    m_rginfoPoints= new infoPoint[2*m_p];
    m_fPointsInfoValid= true;
    m_rginfoPoints[0].m_oPoint.copyPoint(P);
    m_rginfoPoints[0].m_iOrder= 1;

    for(int i=0; i<p; i++) {
        ys= calcYsquared(i);
        if((y=aSquareRoot(ys))>=0) {
            P.m_isZero= false;
            P.m_x= i;
            P.m_y= y;
            m_rginfoPoints[numPoints].m_oPoint.copyPoint(P);
            m_rginfoPoints[numPoints].m_iOrder= -1;
            numPoints++;
            if(y==0)
                continue;
            P.m_y= m_p-y;
            m_rginfoPoints[numPoints].m_oPoint.copyPoint(P);
            m_rginfoPoints[numPoints].m_iOrder= -1;
            numPoints++;
        }
    }
    m_iNumPoints= numPoints;
    return true;
}


bool ECurve::sortPoints()
{
}


int  ECurve::findPoint(Point& P)
{
    return -1;
}


bool ECurve::calcOrders()
{
    int i;
    int pointOrder(ECurve, Point);

    if(!m_fPointsInfoValid)
        return false;
    for(i=0; i<m_iNumPoints; i++)
        m_rginfoPoints[i].m_iOrder= pointOrder(*this, m_rginfoPoints[i].m_oPoint);

    return true;
}


void Point::printPoint()
{
    if(m_isZero) {
        printf("           O");
        return;
    }
    printf("(%4d, %4d)", m_x, m_y);
    return;
}


void Point::copyPoint(Point& P)
{
    m_isZero= P.m_isZero;
    m_x= P.m_x;
    m_y= P.m_y;
}


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


inline int modpower(int base, int exp, int m)
{
    int     x= 1;
    int     i;
    int     s= base;

    if(exp==0)
        return 1;
    for(i=0; i<32; i++) {
        if(exp&1)
            x= (x*s)%m;
        if(exp==0)
            break;
        s= (s*s)%m;
        exp>>= 1;
    }
    return x;
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
    y= modpower(n, t, p); 
    r= e;  
    x= modpower(a, (q-1)/2, p);
    b= (a*x*x)%p;  
    x= (a*x)%p;
    while(b!=1) {
        m= 1;
        for(;;) {
            k= 1<<m;
            j= modpower(b,k,p);
            if(j==1)
                break;
            m++;
        }
	if(m==r)
	    return -1;
        k= 1<<(r-m-1);
        t= modpower(y,k,p);
        y= (t*t)%p;  
        r= m;
        x= (x*t)%p; 
        b= (b*y)%p;
        }

    return(x);
}


bool isPrimitive(int x, int p)
{
    int     i, k;
    int     pMinus1= p-1;

    if(x==1)
        return false;
    if(pMinus1==2)
        return true;

    if(modpower(x, pMinus1/2, p)==1)
        return false;
    for(i=3; i<pMinus1; i+=2) {
        if((pMinus1%i)!=0)
            continue;
        if(modpower(x, pMinus1/i, p)==1)
            return false;
    }
    return true;
}


int printPowers(ECurve oC, Point P)
{
    int     i;
    int     iCurOrder= 1;
    Point   Q(true, 0, 0);
    Point   R(true, 0, 0);
    extern  bool AddPoints(ECurve, Point, Point, Point&);

    printf("\n");
    oC.printCurve();
    printf("\nPowers\n");
    for(i=0; i<oC.m_iNumPoints+5; i++) {
        AddPoints(oC, P, Q, R);
        Q.copyPoint(R);
        printf("(%4d)  ", iCurOrder);
        R.printPoint();
        printf("\n");
        if(R.m_isZero)
            return iCurOrder;
        iCurOrder++;
    }
    return -1;
}


int pointOrder(ECurve oC, Point P)
{
    int     i, j;
    int     iCurOrder= 1;
    Point   Q(true, 0, 0);
    Point   R(true, 0, 0);
    extern  bool AddPoints(ECurve, Point, Point, Point&);

    Q.copyPoint(P);
    R.copyPoint(P);
    for(i=0; i<2*oC.m_iNumPoints; i++) {
        if(R.m_isZero)
            return iCurOrder;
        AddPoints(oC, P, Q, R);
        iCurOrder++;
        Q.copyPoint(R);
    }
    return -1;
}


bool ListPoints(ECurve oC, bool fGetOrder)
{
    printf("\n\n");
    oC.printCurve();
    if(!oC.calcPoints()) {
        printf("Can't calculate points\n");
        return false;
    }

    if(fGetOrder) {
        if(!oC.calcOrders()) {
            printf("Can't calculate points\n");
            return false;
        }
    }

    printf("Points on curve:\n");
    for(int i=0; i<oC.m_iNumPoints; i++) {
        printf("    ");
        if(fGetOrder) {
            printf("[%4d] ", (oC.m_rginfoPoints)[i].m_iOrder);
        }
        oC.m_rginfoPoints[i].m_oPoint.printPoint();
        printf(" ----  %d\n", i+1);
    }
    printf("%d points\n", oC.m_iNumPoints);

    return false;
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
    Q.m_x= (m*m - P1.m_x-P2.m_x)%oC.m_p;
    Q.m_y= (m*(P1.m_x-Q.m_x) - P1.m_y)%oC.m_p;
    if(Q.m_x<0)
        Q.m_x+= oC.m_p;
    if(Q.m_y<0)
        Q.m_y+= oC.m_p;

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
    bool fPowers= false;
    bool fList= false;
    bool fPrimitive= false;
    bool fListwithOrder= false;


    if(an<4) {
        printf("EC p a b [-On (x,y)|O] [-List] [-ListwithOrder] [-Count] [-Add (x,y)|O] (x,y)|O]] [-Multiply (x,y)|O n]\n");
        return(1);
    }

    sscanf(av[1], "%d", &p);
    sscanf(av[2], "%d", &a);
    sscanf(av[3], "%d", &b);
    ECurve oC(p,a,b);
    oC.printCurve();

    Point P, P1, P2, Q;
    for(i=4;i<an;i++) {
        if(strcmp(av[i], "-On")==0) {
            fOn= true;
            i++;
            if(an<i)
                return(1);
            readPoint(av[i], P);
            printf("readpoint %d %d\n", P.m_x, P.m_y);
            P.printPoint();
            printf("\n");
            continue;
        }
        if(strcmp(av[i], "-Count")==0) {
            fCount= true;
            continue;
        }
        if(strcmp(av[i], "-List")==0) {
            fList= true;
            continue;
        }
        if(strcmp(av[i], "-ListwithOrder")==0) {
            fListwithOrder= true;
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
        if(strcmp(av[i], "-Powers")==0) {
            fPowers= true;
            i++;
            if(an<i)
                return(1);
            readPoint(av[i], P);
            i++;
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
        if(strcmp(av[i], "-Primitive")==0) {
            fPrimitive= true;
            continue;
        }
    }

    if(fList) {
        if(!oC.calcPrimitive()) {
            printf("Can't find primitive element of %d\n", oC.m_p);
            return 1;
        }
#ifdef PRINTPRIMITIVE
        int x= 1;
        for(i=1; i<=oC.m_p; i++) {
            x= (x*oC.m_iPrimitive)%oC.m_p;
            printf("%d, %d\n", i, x);
        }
        printf("\n");
#endif
        ListPoints(oC, false);
    }
    if(fListwithOrder) {
        if(!oC.calcPrimitive()) {
            printf("Can't find primitive element of %d\n", oC.m_p);
            return 1;
        }
#ifdef PRINTPRIMITIVE
        int x= 1;
        for(i=1; i<=oC.m_p; i++) {
            x= (x*oC.m_iPrimitive)%oC.m_p;
            printf("%d, %d\n", i, x);
        }
        printf("\n");
#endif
        ListPoints(oC, true);
    }

    if(fCount) {
        printf("There are %d points on the curve\n", LongCountPoints(oC));
    }

    if(fPrimitive) {
        if(!oC.calcPrimitive()) {
            printf("Cant calculate primitive root\n");
            return 1;
        }
        printf("Primitive root mod %d:  %d\n\n", oC.m_p, oC.m_iPrimitive);
        //      1234567890123456789012345678901234567890
        printf("    Number     Index     Power    SqRoot\n");
        for(i=0; i<oC.m_p;i++) {
            n= oC.aSquareRoot(i);
        printf("      %4d      %4d      %4d      %4d\n", i, oC.m_piIndex[i], oC.m_piPow[i], n);
        }
    }

    if(fOn) {
        P.printPoint();
        if(isOnCurve(oC, P))
            printf(" is on the curve\n");
        else
            printf(" is not on the curve\n");
    }

    if(fAdd) {
        P1.printPoint();
        printf(" + ");
        P2.printPoint();
        printf(" = ");
            AddPoints(oC, P1, P2, Q);
        Q.printPoint();
        printf("\n");
    }

    if(fMultiply) {
        printf("%d *  ", n);
        P.printPoint();
        printf(" = ");
            MultiplyPoint(oC, P, n, Q);
        Q.printPoint();
        printf("\n");
    }

    if(fPowers) {
        printPowers(oC,P);
        printf("\n");
    }
    

    return(0);
}


// ---------------------------------------------------------------------------------------




