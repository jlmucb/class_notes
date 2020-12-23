
#define NRINGS 20

int nrings;

short int peg1[NRINGS];
short int peg2[NRINGS];
short int peg3[NRINGS];

int tmoves={0};


snaprings() {
  int i;

  printf("after %d moves\n",tmoves);
  printf("peg1|\t");
  for(i=0;i<NRINGS;i++) {
    if(peg1[i]<=0)
      break;
    printf(" %d|",peg1[i]);
    }
  printf("\n");
  printf("peg2|\t");
  for(i=0;i<NRINGS;i++) {
    if(peg2[i]<=0)
      break;
    printf(" %d|",peg2[i]);
    }
  printf("\n");
  printf("peg3|\t");
  for(i=0;i<NRINGS;i++) {
    if(peg3[i]<=0)
      break;
    printf(" %d|",peg3[i]);
    }
  printf("\n");
  return;
}

//  tower of hanoi, don't leave home without it
main(int argn, char** argv) {
  int i;

  /*
   *  put rings on peg 1
   */
  nrings= atoi(argv[1]);
  printf("Tower of hanoi, %d rings\n",nrings);
  for(i=0;i<NRINGS;i++) {
    if(i>nrings)
      peg1[i]= 0;
    else
      peg1[i]= nrings-i;
    peg2[i]= 0;
    peg3[i]= 0;
    }
  snaprings();
  hanoi(peg1,nrings,peg2,peg3);
  snaprings();
  printf("\ndone, %d moves\n",tmoves);
  exit();
}


move(ip,iq)

short int *ip,*iq;

{
  *iq= *ip;
  *ip= 0;
  tmoves++;
  snaprings();
}


hanoi(p1,n1,p2,p3)

int n1;
short int p1[], p2[], p3[];

/*
 *  Move n1 rings from p1 to p2 using p3
 *    1. move n1-1 rings from p1 to p3
 *    2. move 1 ring from p1 to p2
 *    3. move n1-1 rings from p3 to p2
 */

{
#ifdef DEBUG
  printf("hanoi(%d)\n",n1);
#endif

  if(n1<=0)
    return;
  hanoi(&p1[1],n1-1,p3,p2);
  move(p1,p2);
  hanoi(p3,n1-1,&p2[1],p1);
  return;
}



