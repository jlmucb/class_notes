
#define NULL 0

struct link {
  int element;
  struct link *next;
  };


/* -------------------------------------------------------------------- */


#define NUMR 100


sdr(sets,nsets,answer)

struct link *sets[];
int nsets, answer[];

/*
 *  System of Distinct Representatives
 *    Backtrack
 */

{
  int i,j;
  struct link *csdr[NUMR];

#ifdef DEBUG
  printf("sdr %d sets\n",nsets);
#endif
  /* initialize current location of sdr pointer */
  for(j=0;j<nsets;j++)
    csdr[j]= sets[j];
  i=0;          /* current sdr to be selected */

  while(1) {
    if(i<0)        /* no sdr */
      return(0);
    if(i>=nsets)      /* have sdr */
      break;
    if(csdr[i]==NULL) {    /* end of list, backtrack */
      csdr[i]= sets[i];
      i--;
      csdr[i]= csdr[i]->next;
#ifdef DEBUG
      printf("continue\n");
#endif
      continue;
      }
    for(j=0;j<i;j++)    /* legal rep? */
      if(csdr[i]->element==csdr[j]->element)
        break;
#ifdef DEBUG
    printf("debug: %d %d, elt(%d) \n",i,j,csdr[i]->element);
#endif
    if(j>=i)      /* legal rep */
      i++;
    else        /* not legal, try next */
      csdr[i]= csdr[i]->next;
    }

#ifdef DEBUG
    printf("debug finished sdr: %d\n",i);
#endif
  for(i=0;i<nsets;i++)
    answer[i]= csdr[i]->element;
  return(1);
}


/* -------------------------------------------------------------------- */


#define NUM 8
#define NFREE 500

struct link *thesets[NUM];
struct link freeb[NFREE];
int avail={0};
int sdrs[NUM];


/* -------------------------------------------------------------------- */


#define LAST 20
struct {
  int s,m;
  } intr[LAST]= {
  0,1,0,2,0,3,0,4,
  1,3,1,5,
  2,3,2,5,2,7,2,8,
  3,2,3,5,
  4,1,4,8,
  5,6,5,8,
  6,2,6,3,
  7,2,7,6
  };


/* -------------------------------------------------------------------- */


addelt(i,j)

int i,j;

{
  struct link *p;

  if(thesets[i]==NULL) {
    thesets[i]= (p= &freeb[avail++]);
    p->element= j;
    p->next= NULL;
    return;
    }
  p= thesets[i];
  while(1)
    if(p->next==NULL)
      break;
    else
      p= p->next;
  p->next= &freeb[avail++];
  p= p->next;
  p->element= j;
  p->next= NULL;
  return;
}


main()

{
  int i,j,n;
  struct link *p;

  printf("SDR %d sets, %d elements\n",NUM,LAST);
  for(i=0;i<NUM;i++)
    thesets[i]= NULL;
  for(i=0;i<LAST;i++)
    addelt(intr[i].s,intr[i].m);
  printf("The sets:\n");
  for(i=0;i<NUM;i++) {
    printf("set %d: ",i+1);
    p= thesets[i];
    while(p!=NULL) {
      printf("%d ",p->element);
      p=p->next;
      }
    printf("\n");
    }
  if(sdr(&thesets[0],NUM,sdrs)<=0)
    printf("No sdr for this configuration\n");
  else {
    for(i=0;i<NUM;i++)
      printf("%d ",sdrs[i]);
    printf("\n");
    }
  printf("done\n");
  exit();
}


/* -------------------------------------------------------------------- */

