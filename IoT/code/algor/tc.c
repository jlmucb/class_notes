
#define NULL 0

struct gen {
  unsigned  *g;
  unsigned  *ginv;
  };
struct schreier {
  unsigned  point;
  int *orbit;
  unsigned orbitsize;
  struct gen **svector;
  int *backptr;
  };
struct gendata {
  struct gen *pgen;
  unsigned  ck;
  struct gendata *next;
  };




bsgs(npoints,ngen,nbigbase,base,bigbase,basedata,top,s)

unsigned ngen,*base,*bigbase,*s,nbigbase,npoints;
struct schreier **basedata;
struct gendata **top;

{
  unsigned j,jj,k,x,basesize,first,nfixpts,*perm;
  unsigned orbsize,*ibase,*ibigbase;
  unsigned  *alloc();
  struct gen **pgalloc(),*galloc(),*pg,**stor,**y;
  struct gen *member(),*ginsert();
  struct schreier *salloc(),*initorb();
  struct gendata *gdalloc(),**bottom,*gd,**pgdalloc();
  int *ialloc(),i,*orb,*w,b;

  /* get a first approximation to a base */
  basesize= makebase(ngen,npoints,nbigbase,base,bigbase,s);
  /* initialize the arrays top and bottom */
  /* which point to the stacks.          */
  bottom= pgdalloc(nbigbase);
  for (i=0;i<nbigbase;++i) {
    top[i]= gdalloc();
    bottom[i]= gdalloc();
    top[i]->pgen= bottom[i]->pgen= NULL;
    top[i]->ck= bottom[i]->ck= 0;
    top[i]->next= bottom[i];
    bottom[i]->next= NULL;
    }

  for (i=0;i<basesize;++i)  {
    /* initialize orbit structures.     */
    basedata[i]= initorb(base[i],npoints);
    }
  for (i= 0;i<ngen;++i)  {
    /* initialize the list of generators */
    perm= s+(npoints*i);
    /* compute the first k such that perm */
    /* moves b[k].                        */
    for (j= 0;perm[j] == base[j];++j);
    nfixpts= j;
    first= 0;
    /* Allocate space for the permutation */
    /* and return a pointer. Recompute the */
    /* orbit structures for the relevant */
    /* basepoints. Note that we must pass */
    /* perm in order to extend the orbit */
    /* structures.                        */
    pg= ginsert(npoints,perm);
    linsert(first,nfixpts,pg,top);
    for (k= first;k <= nfixpts;++k)
      orbdata(basedata[k],top[k],pg,npoints);
    }
  ibase= alloc(nbigbase);    /* these arrays are used in member()  */
  ibigbase= alloc(nbigbase);
  stor= pgalloc(npoints);
  /* begin the main loop               */
  /* at this point we know that base is */
  /* a "base" and that top[i] points to */
  /* a strong generating set for the sub- */
  /* group fixing base[0],..,base[i-1]   */
  for (i= basesize-1;i >= 0;--i)  {
    pg= NULL;
    orb= (basedata[i])->orbit;
    orbsize= basedata[i]->orbitsize;
    /* loop through the orbit checking the */
    /* schreier generators for each element */
    /* on the list pointed to by top[i].   */
    for (j= 0;j<orbsize;++j)   {
      /* If we are not at the bottom of the */
      /* list, then process the next generator */
      /* if the current point and generator */
      /* has not yet been processed.         */
      for (gd= top[i];gd->pgen != NULL;gd= gd->next)   {
        /* check if the schreier generator is in */
        /* i th level subgroup. If yes, return */
        /* the NULL pointer. Otherwise, return */
        /* a pointer to its structure and the */
        /* number nfixpts of fixed points.  */
        if (gd->ck <= j) {
          x= orb[j];
          pg= member(i,x,&nfixpts,gd->pgen,npoints,basedata,base,basesize,bigbase,nbigbase,
              ibase,ibigbase,stor);
          ++(gd->ck);
          if (pg !=  NULL) {
            /* a new generator has been found */
            first= i+1;
            /* if necessary, add a new base point */
            if (nfixpts >= basesize) {
              ++basesize;
              perm= pg->g;
              for (jj=0;perm[jj] == bigbase[jj];++jj);
              base[basesize-1]= bigbase[jj];
              basedata[basesize-1]= initorb(base[basesize-1],npoints);
              }
            /* insert in the appropriate stacks  */
            linsert(first,nfixpts,pg,top);
            /* recompute the orbit structures.*/
            for (k =first;k <= nfixpts;++k)
              orbdata(basedata[k],top[k],pg,npoints);
            i= nfixpts;
            goto endilp;
            }
          }
        }
      }
endilp: ;
    }

  return(basesize);
}

makebase(ngen,npoints,nbigbase,base,bigbase,s)

unsigned ngen,npoints,nbigbase,*base,*bigbase,*s;

{
  unsigned basesize,flag,i,j,k,index;

  basesize= 0;
  for (i= 0;i<ngen;++i) {
    flag= 0;               /* check if the generator moves a basepoint */
    for (j= 0;j<basesize && flag == 0;++j){
      index= j+(npoints*i);
      if (s[index] != base[j])
        flag= 1;
      }
    if (flag == 0) {                    /* add a point to the base */
      ++basesize;
      for (k= 0;k<nbigbase && flag == 0;++k){
        index= k+(npoints*i);
        if (s[index] != bigbase[k]) {
          flag= 1;
          base[basesize-1]= bigbase[k];
          }
        }
      }
    }

  return(basesize);
}


orbdata(ps,gd,pg,npoints)

struct schreier *ps;
struct gen *pg;
struct gendata *gd;
unsigned npoints;

{
  int *orb,*w;
  unsigned i,j,k,left,m,right;
  int *ialloc();
  struct gen **pgalloc(),**y;
  struct gendata *pl;

  right =(ps->orbitsize)-1;
  left= right;
  orb= ps->orbit;
  y= ps->svector;
  w= ps->backptr;
  for (i= 0;i <= left;++i) {  /* first apply pg->g to each orbit point. */
    j= orb[i];
    k= (pg->g)[j];
    if (w[k] == -2) {
      ++right;
      orb[right]= k;
      y[k]= pg;
      w[k]= j;
      }
    }
  /* if perm preserves the orbit then the */
  /* next loop is not performed.          */
  while (left<right)  {
    pl= gd;
    ++left;
    j= orb[left];
    pg= pl->pgen;
    while(pg != NULL) {
      k= (pg->g)[j];
      if (w[k] == -2) {
        ++right;
        orb[right]= k;
        y[k]= pg;
        w[k]= j;
        }
      pl= pl->next;
      pg= pl->pgen;
      }
    }
  ps->orbitsize= right+1;
  ps->orbit= orb;
  ps->svector= y;
  ps->backptr= w;
}


struct schreier *initorb(x,npoints)

unsigned x,npoints;

{
  unsigned j;
  struct schreier *ps,*salloc();
  struct gen **pgalloc();
  int *ialloc();

  ps= salloc();
  ps->point= x;
  ps->orbitsize= 1;
  ps->orbit= ialloc(npoints);
  (ps->orbit)[0]= x;
  for (j= 1;j<npoints;++j)
    (ps->orbit)[j]= -1;
  ps->svector= pgalloc(npoints);
  for (j= 0;j<npoints;++j)
    (ps->svector)[j]= NULL;
  ps->backptr= ialloc(npoints);
  for (j= 0;j<npoints;++j)
    (ps->backptr)[j]= -2;
  (ps->backptr)[ps->point]= -1;
  return(ps);
}


struct gen *ginsert(npoints,perm)

unsigned npoints,*perm;

{
  struct gen *galloc(),*pg;
  unsigned *alloc(),*perminv,i;

  pg= galloc();
  pg->g= perm;
  perminv= alloc(npoints); /* create the inverse of perm  */
  for (i= 0;i<npoints;++i)
    perminv[perm[i]]= i;
  pg->ginv= perminv;
  return(pg);
}


linsert(first,last,pg,top)

unsigned first,last;
struct gen *pg;
struct gendata **top;

{
  struct gendata *pgd;
  unsigned i,k;

  for (i= first;i <= last;++i)
    if ((top[i])->pgen ==NULL)
      (top[i])->pgen= pg;
    else {
      pgd= gdalloc();
      pgd->pgen= pg;
      pgd->ck= 0;
      pgd->next= top[i];
      top[i]= pgd;
      }
}


struct gen *member(level,a,pnfix,pg,npoints,basedata,base,basesize,bigbase,nbigbase,ibase,ibigbase,stor)

/* ibase and ibigbase hold the images   */
/* of base and bigbase as the pemutation */
/* is processed. stor is used to hold the */
/* schreier generator as a word in the */
/* generators in top[i].               */

unsigned a,*pnfix,npoints,*base,basesize,*bigbase,nbigbase,*ibase,*ibigbase;
int level;
struct gen *pg,**stor;
struct schreier **basedata;

{
  unsigned *alloc(),*h,*hinv,k,b,last,*multperm();
  int i,j,*w,length;
  struct gen *p,**y,*galloc();
  /* the schreier generator as a word in */
  /* the generators in top[i].           */
  movers(base,ibase,basesize);
  movers(bigbase,ibigbase,nbigbase);
  /* find the effect of the schreier */
  /* generator on base and bigbase and  */
  /* store in ibase and ibigbase.        */
  w= basedata[level]->backptr;
  y= basedata[level]->svector;
  length= -1;
  for (b= a;w[b] >= 0;) {    /* we know that w[a] != -2, so that a is */
    ++length;          /* is in the orbit. Put the reverse word  */
    stor[length]= y[b]; /* of the coset rept ,u(a),for a in stor. */
    b= w[b];
    }
  if (length >= 0)           /* compute u(a).  */
    for (i= length;i >= 0;--i)  {
      image(ibase,(stor[i])->g,basesize);
      image(ibigbase,(stor[i])->g,nbigbase);
      }
  image(ibase,pg->g,basesize);   /* multiply by pg->g     */
  image(ibigbase,pg->g,nbigbase);
  /* multiply by inverse of u(pg->g[a])    */
  for (b= (pg->g)[a];w[b] >= 0;b= w[b]) {
    image(ibase,(y[b])->ginv,basesize);
    image(ibigbase,(y[b])->ginv,nbigbase);
    }
  /* factor the schreier generator and check*/
  /* if a new generator has been found.     */
  last= level+1;                /* last keeps track of *pnfix.       */
  for (k= level+1;k<basesize;++k) {
    y= basedata[k]->svector;
    w= basedata[k]->backptr;
    if (w[ibase[k]] == -2)     /* a new generator has been found */
      goto new;
    ++last;
    /* strip off the inverse of u(b).         */
    for (b= ibase[k];w[b] >=0;) {
      image(ibase,(y[b])->ginv,basesize);
      image(ibigbase,(y[b])->ginv,nbigbase);
      b= w[b];
      }
    }
  /* check for the identity element   */
  if (equal(ibigbase,bigbase,nbigbase))
    return(NULL);
  /* constuct the explicit permutation */
new:
  h= alloc(npoints);
  for(i= 0;i<npoints;++i)   /* initialize h with the identity    */
    h[i]= i;
  w= basedata[level]->backptr;
  y= basedata[level]->svector;
  for (i= length;i >= 0;--i) {  /* compute u(a).                     */
    h= multperm(h,(stor[i])->g,npoints);
    }
  h= multperm(h,pg->g,npoints); /* multiply by pg->g               */
  /* multiply by inverse of u(pg->g[a])    */
  for (b= (pg->g)[a];w[b] >= 0;b= w[b]) {
    h= multperm(h,y[b]->ginv,npoints);
    }
  /* factor the schreier generator and check*/
  for (k= level+1;k <= last && k<basesize;++k) {
    y= basedata[k]->svector;
    w= basedata[k]->backptr;
    /* strip off the inverse of u(b).         */
    for (b= h[base[k]];w[b] >=0;) {
      h= multperm(h,(y[b])->ginv,npoints);
      b= w[b];
      }
    }
  *pnfix= last;
  hinv= alloc(npoints);
  for (i= 0;i<npoints;++i)
    hinv[h[i]]= i;
  p= galloc();
  p->g= h;
  p->ginv= hinv;
  return(p);
}


movers(arr1,arr2,n)

unsigned *arr1,*arr2,n;

{
  unsigned i;

  for (i= 0;i<n;++i)
    arr2[i]= arr1[i];
}


image(arr,perm,n)

unsigned *arr,*perm,n;

{
  unsigned i;

  for (i= 0;i<n;++i)
    arr[i]= perm[arr[i]];
}


equal(arr1,arr2,n)

unsigned *arr1,*arr2,n;

{
  unsigned i;

  for (i= 0;i<n && arr1[i] == arr2[i];++i);
  if (i == n)
    return(1);
  else
    return(0);
}


unsigned *multperm(perm1,perm2,n)

unsigned *perm1,*perm2,n;

{
  unsigned i;

  for (i= 0;i<n;++i)
    perm1[i]= perm2[perm1[i]];
  return(perm1);
}


unsigned  *alloc(n)

unsigned n;

{
  char *malloc();

  return((unsigned *) malloc(n*sizeof(n)));
}


struct gen *galloc()

{
  char *malloc();

  return((struct gen *) malloc(sizeof(struct  gen)));
}


int *ialloc(n)

unsigned n;

{
  char *malloc();

  return((int *) malloc(n*sizeof(int)));
}


int **palloc(n)

unsigned n;

{
  char *malloc();

  return((int **) malloc(n*sizeof(int *)));
}


struct gen  **pgalloc(n)

unsigned n;

{
  char *malloc();

  return((struct gen **) malloc(n*sizeof(struct gen*)));
}


struct schreier  **psalloc(n)

unsigned n;

{
  char *malloc();

  return((struct schreier **) malloc(n*sizeof(struct schreier*)));
}


struct schreier  *salloc()

{
  char *malloc();

  return((struct schreier *) malloc(sizeof(struct schreier)));
}


struct gendata *gdalloc()

{
  char *malloc();

  return((struct gendata *) malloc(sizeof(struct  gendata)));
}


struct gendata  **pgdalloc(n)

unsigned n;

{
  char *malloc();

  return((struct gendata **) malloc(n*sizeof(struct gendata*)));
}


