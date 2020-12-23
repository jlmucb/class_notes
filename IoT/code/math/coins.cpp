
// coins.cpp
//  Expected number of coins for change
//  John Manferdelli, 2003

#include <stdlib.h>
#include <stdio.h>

#define NODEBUG


class Coinslot {
public:
  int    m_iNumberOfCoins;
  int*    m_rgiCoinValues;
  double    m_xAvCoins;

  Coinslot(int iNumCoins);
  ~Coinslot();
  void    CopySelf(Coinslot* pNew);
  int    CalcNumCoins(int iValue);
};


Coinslot::Coinslot(int iNumCoins) {
  m_iNumberOfCoins= iNumCoins;
  m_rgiCoinValues= new (int [m_iNumberOfCoins]);
}


Coinslot::~Coinslot() {
  if(m_rgiCoinValues)
    delete (m_rgiCoinValues);
  m_rgiCoinValues= NULL;
}


void Coinslot::CopySelf(Coinslot* pNew) {
  int i;

  pNew->m_iNumberOfCoins= m_iNumberOfCoins;
  for(i=0;i<m_iNumberOfCoins;i++)
    pNew->m_rgiCoinValues[i]= m_rgiCoinValues[i];
  pNew->m_xAvCoins= m_xAvCoins;
}


int Coinslot::CalcNumCoins(int iValue) {
  int iCurCoinVal;
  int iNumUsed= 0;
  int k;

  for(k=(m_iNumberOfCoins-1); k>=0; k--) {
    iCurCoinVal= m_rgiCoinValues[k];
    while(iValue>=iCurCoinVal) {
      iValue-= iCurCoinVal;
      iNumUsed++;
    }
  }
  return(iNumUsed);
}


int main(int argc, char* argv[]) {
  int     iNumCoins;
  int     i;
  double    xAvCoins= 0.0;
  Coinslot*  pMaster= NULL;
  int     iC1, iC2;

  if (argc<2) {
    printf("COINS: number-of-coins value1 ,...\n");
    return(1);
  }

  iNumCoins= atoi(argv[1]);
  if (argc<(iNumCoins+2)) {
    printf("Not enough coin values\n");
    return(1);
  }

  // 3 coins
  pMaster= new Coinslot(iNumCoins);
  if(pMaster== NULL){
    printf("Bad allocation of master object %d coins\n", iNumCoins);
    return(1);
  }

  for(i=0;i<iNumCoins; i++){
    pMaster->m_rgiCoinValues[i]= atoi(argv[i+2]);
  }

  for(i=1; i<100; i++) {
    xAvCoins+= (double)pMaster->CalcNumCoins(i);
  }
  xAvCoins/= 99.0;
  printf("%8.4f average coins\n", xAvCoins);


  Coinslot  oMin(3);

  oMin.m_iNumberOfCoins= 3;
  oMin.m_xAvCoins= 100.00;
  pMaster->m_rgiCoinValues[0]= 1;

  for(iC1= 2; iC1<100; iC1++) {
    pMaster->m_rgiCoinValues[1]= iC1;
    for (iC2=(iC1+1); iC2<100; iC2++) {
      pMaster->m_rgiCoinValues[2]= iC2;
      xAvCoins= 0.0;
      for(i=1; i<100; i++) {
        xAvCoins+= (double)pMaster->CalcNumCoins(i);
      }
      pMaster->m_xAvCoins= xAvCoins/99.0;
#ifndef NODEBUG
      printf("Running.  %8.4f Coins (%03d, %03d, %03d)\n", 
                   pMaster->m_xAvCoins, 
                   pMaster->m_rgiCoinValues[0],
                   pMaster->m_rgiCoinValues[1],
                   pMaster->m_rgiCoinValues[2]);
#endif
      
      // New Min?
      if(pMaster->m_xAvCoins<oMin.m_xAvCoins) {
        pMaster->CopySelf(&oMin);
      }
    }
  }

  printf("Minimum.  %8.4f Coins (%03d, %03d, %03d)\n", oMin.m_xAvCoins, 
                   oMin.m_rgiCoinValues[0],
                   oMin.m_rgiCoinValues[1],
                   oMin.m_rgiCoinValues[2]);

  return 0;
}


// ---------------------------------------------------------------------------------------------------

