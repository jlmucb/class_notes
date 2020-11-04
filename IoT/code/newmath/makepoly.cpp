#include <stdio.h> 
#include <string.h> 
#include <io.h>


void printFunction(char* msg, int iSizeF, int* piFun)
{
    int i;

    printf("%s",msg);
    for(i=0;i<iSizeF;i++)
        printf("%02d ", piFun[i]);
    printf("\n");
}


void printPoly(char* msg, int iSizePoly, int* piPoly)
{
    int ii, jj;

    printf("%s: ", msg);
    jj= -1;
    for(ii=(iSizePoly-1);ii>=0;ii--) {
        if(piPoly[ii]!=0)
            jj= ii;
    }
    for(ii=(iSizePoly-1);ii>=0;ii--) {
        if(piPoly[ii]!=0) {
            switch(ii) {
                case 0:
                    printf("(%d)", piPoly[ii]);
                    break;
                case 1:
                    printf("(%d)x", piPoly[ii]);
                    break;
                default:
                    printf("(%d)x^%d", piPoly[ii],ii);
                    break;
            }
            if(ii!=jj)
                printf(" + ");
                }
        }
        if(jj<0)
            printf("0\n");
        else
            printf("\n");
}


// ---------------------------------------------------------------------------------------


class MakePoly {
public:
    int         m_iMod;
    int*        m_piInverse;

    MakePoly(int m);
    ~MakePoly();
    int     Inverse(int k);
    int CalcPoly(int iSizeF, int* piF, int iSizePoly, int* piPoly);
    int MultPoly(int iSizePoly1, int iSizePoly2, int* piSizeOutPoly, int* piPoly1,
                 int* piPoly2, int* piPolyOut);
    int AddPoly(int iSizePoly1, int iSizePoly2, int* piSizeOutPoly, int* piPoly1,
                int* piPoly2, int* piPolyOut);
    int AddToPoly(int iSizePoly1, int iSizePoly2, int* piPoly1, int* piPoly2);
    int MultByConst(int iSizePoly, int* piPoly, int iConst);
    int MultByPoly(int iSizePoly1, int iSizePoly2, int* piPoly1,
                 int* piPoly2);
};


MakePoly::MakePoly(int m)
{
    int i, j;

    m_iMod= m;
    m_piInverse= new int[m];
    m_piInverse[0]= 0;
    m_piInverse[1]= 1;
    for(i=2;i<m_iMod;i++) {
        m_piInverse[i]= 0;
        }

    for(i=2;i<m_iMod;i++) {
        for(j=2;j<m_iMod;j++) {
            if(m_piInverse[j]==i) {
                m_piInverse[i]= j;
                break;
                }
            if(m_piInverse[j]!=0)
                continue;
            if(((i*j)%m_iMod==1)) {
                m_piInverse[i]= j;
                break;
                }
            }
     }
}


int  MakePoly::Inverse(int k)
{
    return(m_piInverse[(k+m_iMod)%m_iMod]);
}


MakePoly::~MakePoly() 
{
    if(m_piInverse!=NULL)
        delete m_piInverse;
    m_piInverse= NULL;
}


int MakePoly::CalcPoly(int iSizeF, int* piF, int iSizePoly, int* piPoly)
{
    int i, j, k;
    int iV, d;

    if(iSizePoly<m_iMod)
        return(-1);
    int* piPoly1= new int [iSizePoly];
    int* piPoly2= new int [iSizePoly];
    for(i=0; i<iSizePoly; i++)   {
        piPoly1[i]= 0;
        piPoly2[i]= 0;
        piPoly[i]= 0;
        }

    for(j=0;j<iSizeF;j++) {
        iV= piF[j];
#ifdef DEBUG
                printf("\n\nf(%d)=%d\n", j,iV);
#endif
        d= 1;
        piPoly2[0]= 1;
                for(k=1;k<iSizePoly;k++)
                        piPoly2[k]= 0;
        for(k=0; k<m_iMod; k++) {
            piPoly1[0]= m_iMod-k;
            piPoly1[1]= 1;
            if(k!=j) {
                MultByPoly(iSizePoly, 2, piPoly2, piPoly1);
                d= (d*(m_iMod+k-j))%m_iMod;
            }
        }
#ifdef DEBUG
                printf("f(%d)= %d, 1/%d= %d\n", j,iV,d,Inverse(d));
#endif
        MultByConst(iSizePoly, piPoly2, Inverse(d)*iV);
#ifdef DEBUG
                printPoly("After Mult", iSizeF, piPoly2);
#endif
        AddToPoly(iSizePoly, iSizePoly, piPoly2, piPoly);
    }

#ifdef DEBUG
        printPoly("After CalcPoly", iSizeF, piPoly);
#endif
    delete piPoly1;
    delete piPoly2;
    return(iSizePoly);
}


int MakePoly::MultPoly(int iSizePoly1, int iSizePoly2, int* piSizeOutPoly, int* piPoly1,
             int* piPoly2, int* piPolyOut)
{
    int i,j,k,m;

    if(*piPolyOut<(iSizePoly1+iSizePoly2))
        return(-1);

    for(i=0; i<*piSizeOutPoly; i++) 
        piPolyOut[i]= 0;

    for(i=0; i<iSizePoly1; i++) {
        for(j=0; j<iSizePoly2; j++) {
            k= i+j;
            m= piPolyOut[k];
            m= (m+piPoly1[i]*piPoly2[j])%m_iMod;
        }
    }

    k= 0;
    for(i=0; i<*piSizeOutPoly; i++)
        if(piPolyOut[i]!=0)
            k= i;

    return(k);
}


int MakePoly::AddPoly(int iSizePoly1, int iSizePoly2, int* piSizeOutPoly, int* piPoly1,
             int* piPoly2, int* piPolyOut)
{
    int i, j;

    if(iSizePoly1>iSizePoly2) {
        for(i=0;i<iSizePoly2;i++) {
            piPolyOut[i]= (piPoly1[i] + piPoly2[i]) % m_iMod;
                }
        for(; i<iSizePoly1;i++) {
            piPolyOut[i]= piPoly1[i] % m_iMod;
                }
    }
    else {
        for(i=0; i<iSizePoly1; i++) {
            piPolyOut[i]= (piPoly1[i] + piPoly2[i]) % m_iMod;
                }
        for(; i<iSizePoly2; i++) {
            piPolyOut[i]= piPoly2[i] % m_iMod;
                }
    }

    j= 0;
    for(i=0; i<*piSizeOutPoly; i++)
        if(piPolyOut[i]!=0)
            j= i;
    return(j+1);
}


int MakePoly::MultByPoly(int iSizePoly1, int iSizePoly2, int* piPoly1, int* piPoly2)
//
// poly1 <-- poly1*poly2
//
{
    int i, j, k, m;
    int iSizePolyOut= iSizePoly1+iSizePoly2;
    int* piPolyOut= new int [iSizePolyOut];

#ifdef DEBUG1
        printf("\n\nMultByPoly "); printPoly("Poly1", iSizePoly1, piPoly1);  printPoly("Poly2", iSizePoly2, piPoly2);
#endif

    for(i=0; i<iSizePolyOut;i++)
        piPolyOut[i]= 0;

    for(i=0; i<iSizePoly1;i++) {
        if((m=piPoly1[i])==0)
           continue;
        for(j=0; j<iSizePoly2;j++) {
            k= i+j;
            piPolyOut[k]= (piPolyOut[k]+piPoly1[i]*piPoly2[j]) % m_iMod;
        }
    }

    j= 0;
        for(i=0; i<iSizePolyOut;i++) {
        if(piPolyOut[i]!=0)
            j= i;
        }

#ifdef DEBUG1
        printPoly("MultByPoly Result: ", iSizePolyOut, piPolyOut);
#endif
    for(i=0; i<iSizePoly1;i++)
        piPoly1[i]= piPolyOut[i];
    delete piPolyOut;
    return(j+1);
}

int MakePoly::AddToPoly(int iSizePoly1, int iSizePoly2, int* piPoly1, int* piPoly2)
//
// poly2 <-- Poly1+Poly2
{
    int i;

        if(iSizePoly1<iSizePoly2) {
                printf("AddToPoly Error\n");
        return(-1);
        }
    for(i=0; i<iSizePoly1;i++)
        piPoly2[i]= (piPoly1[i] + piPoly2[i]) % m_iMod;
    return(iSizePoly2);
}


int MakePoly::MultByConst(int iSizePoly, int* piPoly, int iConst)
{
    int i;

    for(i=0; i<iSizePoly;i++)
        piPoly[i]= (iConst*piPoly[i]) % m_iMod;
    return(iSizePoly);
}


// ---------------------------------------------------------------------------------------


int main(int an, char** av) 

{ 
    int     p= 5;               // Field size
    int     n= 0;               // Number of functions
    int*    piFunction= NULL;
    int*    piPolyOut= NULL;
    int     i, in;
    bool    fCommandLine= false;
    char*   pFile= NULL;

    if(an<2) {
        printf("MakePoly [-PrimeField num] file\n");
        printf("If optional args are not used, the parameters must be first 8 bytes in file\n");
        return(1);
    }

    for(i=1; i<an; i++) {
        if(strcmp("-PrimeField", av[i])==0) {
            fCommandLine= true;
            sscanf(av[++i], "%d", &p);
            n= 1;
        }
        else
            break;
    }
    pFile= av[i];

    if((in=open(pFile, 0))<0) {
        printf("Can't open: %s\n", pFile);
        return(1);
    }

    if(!fCommandLine) {
        read(in, &p, sizeof(int));
        read(in, &n, sizeof(int));
    }
    piFunction= new int[p];
    MakePoly* poPoly= new MakePoly(p);
    piPolyOut= new int[p];
    printf("MakePoly Field GF(%d)\n", p);

    if(poPoly!=NULL && piPolyOut!=NULL && piFunction!=NULL) {
        for(i=0; i<n; i++) {
            read(in, piFunction, p*sizeof(int));
            poPoly->CalcPoly(p, piFunction, p, piPolyOut);
            printFunction("\nFunction: ", p, piFunction);
            printPoly("Polynomial", p, piPolyOut);
        }
        printf("\nDone\n");
    }
    else {
        printf("Couldn't allocate poly object\n");
        }


    if(piPolyOut)
        delete piPolyOut;
    if(poPoly)
        delete poPoly;
    if(piFunction)
        delete piFunction;

    close(in);
    return(0);
}


// ---------------------------------------------------------------------------------------




