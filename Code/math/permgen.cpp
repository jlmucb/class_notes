#include  <stdio.h>


class CMove {
  public:
  int iFirst, iSecond;
  void Push(int i, int j){iFirst= i; iSecond= j;}
  void Pop(int *pi, int *pj) {*pi= iFirst; *pj= iSecond;}
  };


void transpose(int* piA, int* piB) {
  int i;

  i= *piA;
  *piA= *piB;
  *piB= i;
  return;
  }


 int Factorial(int iNumElements) {
   register int iNum;

   iNum= 1;

   while(iNumElements>0) {
    iNum*= iNumElements;
    iNumElements--;
    }
   return (iNum);
   }



void generate(int iNumElements, CMove* pNewMoves) {
  CMove *pOldMoves, *pOSave; 
  int  iNumlessOneFact, iMove1, iMove2;
  int  i, j, iFwd;
  
  if(iNumElements==1) {
    pNewMoves->Push(0,0);
    return;
    }

  iNumlessOneFact= Factorial(iNumElements-1);

  pOSave= new CMove [iNumlessOneFact];
  pOldMoves= pOSave;

  generate(iNumElements-1,pOldMoves);

  iFwd= 0;

  for(i=0;i<iNumlessOneFact;i++) {
    (pOldMoves++)->Pop(&iMove1,&iMove2);
    if(iFwd) {
      (pNewMoves++)->Push(iMove1+1,iMove2+1);
      for(j=0;j<(iNumElements-1);j++) {
        (pNewMoves++)->Push(j,j+1);
        }
      }
    else {
      (pNewMoves++)->Push(iMove1,iMove2);
      for(j=(iNumElements-1);j>0;j--) {
        (pNewMoves++)->Push(j,j-1);
        }
      }    
    iFwd^= 1;
    }

  delete  [iNumlessOneFact] pOSave;

  return;
  }


int main(int an,char *av[]) {
  int iNumPlace, iNumFact;
  int i, iMove1, iMove2;
  CMove* pMoves;

  sscanf(av[1],"%d",&iNumPlace);
  iNumFact= Factorial(iNumPlace);
  pMoves= new CMove [iNumFact];
  generate(iNumPlace,pMoves);

  for(i=0;i<iNumFact;i++) {
    (pMoves++)->Pop(&iMove1,&iMove2);
    printf("(%d, %d)\n", iMove1, iMove2);
    }

  return(0);
  }

