#define NULL 0

# define EOF 0
# define USYM 1
# define UNUM 2
# define EQUAL 3
# define MINUS 4
# define LP 5
# define RP 6
# define COMMA 7
# define COLON 8
# define ARROW 9
# define UNK 20

#define TBUF 100
#define IOBUF 3072


int line={1};
int errors={0};
int infile,tokenl;
char tokenc[TBUF];


//  Get next real character strips comments.
getrc() {
  char c;

  if((c=getc())=='#') {
    for(;;) {
      if((c=get())==EOF || (c=='\n'))
        break;
    }
  }
  return getrc();
}


yylex() {
  char c;
  int i;

  for(;;) {
    if((c=getrc())==NULL)
      return(EOF);
    else if(!WSPACE)
      break;
  }
  if(tokes(c)==1)  {
    tokenc[0]= c;
    tokenl= 1;
    for(;;)  {
      if((c=getrc())<=0)
        return(0);
      if(tokes(c)==1)
        tokenc[tokenl++]= c;
      else {
        ungetc();
        if(((tokenc[0]>='0')&&(tokenc[0]<='9'))||(tokenc[0]=='.'))
          return(UNUM);
        return(USYM);
        }
      }
    }
  switch(c) {
    default:
    return(UNK);
    case '=':
    return(EQUAL);
    case '(':
    return(LP);
    case ')':
    return(RP);
    case ',':
    return(COMMA);
    case ':':
    return(COLON);
    case '-':
    return(MINUS);
    case '>':
    return(ARROW);
    }
}


tokes(char c) {
  if((c>='A')&&(c<='Z'))
    return(1);
  if((c>='a')&&(c<='z'))
    return(1);
  if((c>='0')&&(c<='9'))
    return(1);
  switch(c) {
    default:
    return(0);
    case '_':
    case '.':
    case '%':
    case '$':
    return(1);
    }
}

tonum(int radix, char* pt, int dig) {
  int j,k;

  j= 0;
  while(dig-->0) {

      k= *(pt++)-'0';
      if((k<0)||(k>=radix)) {
    printf("\nError %d, bad numeric (%d) line %d",++errors,k,line);
    return(0);
    }
      j= j*radix+k;
      }
  return(j);
}


#define NPERMS 500
#define NBIGBASE 100

int s[NPERMS];
int bigbase[NBIGBASE];

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

extern struct gen *galloc();
extern struct gendata **pgdalloc();
extern struct schreier **psalloc();
extern unsigned *alloc();
extern bsgs();


main(int argn,char** argv) {
  int ngen,nbigbase,npoints;
  unsigned i,j,k,*base,basesize;
  unsigned order,factor;      /* order of the group */
  struct gendata **top,*gd;   /* array of pointers to the top of stacks */
  struct schreier **basedata;

  for (i = 1; (i < argn) && (*(argv[i]) == '-'); ++i);
  if (i == argn)  {
    printf("Cannot open , quitting\n");
    exit();
    }
  if((infile=open(argv[i]))<=0) {
    printf("Cannot open %s, quitting\n",argv[i]);
    exit();
    }
  if(yylex()!=UNUM) {
    printf("bad format, quitting\n");
    exit();
    }
  npoints= tonum(10,tokenc,tokenl);
  if(yylex()!=UNUM) {
    printf("bad format, quitting\n");
    exit();
    }
  ngen= tonum(10,tokenc,tokenl);
  if(yylex()!=UNUM) {
    printf("bad format, quitting\n");
    exit();
    }
  nbigbase= tonum(10,tokenc,tokenl);
  if((npoints*ngen)>NPERMS) {
    printf("Cant hold permutation, quitting\n");
    exit();
    }
  if(nbigbase>NBIGBASE) {
    printf("Cant hold bigbase, quitting\n");
    exit();
    }
  for(i=0;i<(ngen*npoints);i++) {
    if(yylex()!=UNUM) {
      printf("bad format, quitting\n");
      exit();
      }
    s[i]= tonum(10,tokenc,tokenl);
    }
  for(i=0;i<nbigbase;i++) {
    if(yylex()!=UNUM) {
      printf("bad format, quitting\n");
      exit();
      }
    bigbase[i]= tonum(10,tokenc,tokenl);
    }
  close(infile);

  printf("\ndegree: %d, %d generators\n",npoints,ngen);
  for(i=0;i<ngen;i++) {
    printf("\t");
    for(j=0;j<npoints;j++)
      printf("%d ",s[npoints*i+j]);
    printf("\n");
    }

  base= alloc(nbigbase);       /* the base as a subset of bigbase   */
  basedata= psalloc(nbigbase);
  top= pgdalloc(nbigbase);
  basesize= bsgs(npoints,ngen,nbigbase,base,bigbase,basedata,top,s);
  if(basesize>0) { 
    order= 1;
    printf("%s","orbit sizes are ");
    for(i=0;i<basesize;++i) { 
      factor= (basedata[i]->orbitsize);
      printf("%u%s", factor, " ");
      order= order * factor;
      }
    printf("\n%s%u\n", "order= ", order);
    printf("%s", "the basepoints are ");
    for(i= 0;i<basesize;++i) { 
      printf("%u%s" , base[i] , " ");
      }
    printf("\n");
    for (j = 1; (j < argn) && (*(argv[j]) != '-'); ++j);
    if (j == argn)  {
      for(i= 0;i<basesize;++i) { 
        printf("%s%u\n", "generators for basepoint ", base[i]);
        for(gd=top[i];gd->pgen!=NULL;gd=gd->next) { 
          for(k= 0;k<npoints;++k) 
            printf("%u%s", ((gd->pgen)->g)[k], " ");
          printf("\n");
          }
        }
      }
    else  {
      for(i= 0;i<basesize;++i) { 
        printf("%s%u\n", "generators for basepoint ", base[i]);
        for(gd=top[i];gd->pgen!=NULL;gd=gd->next) 
          cyclstruc((gd->pgen)->g,npoints);
          }
      }

    }
  printf("done\n");
  exit();
}

cyclstruc(int* a, int n) {
  int ncycles,i,j,k,count,x,max;
  int **cyclptr,*length,*check,*b,*ialloc(),**palloc();

  b = ialloc(n);         /* stores the cycles */
  cyclptr = palloc(n);   /* points to the beginning of a cycle in b */
  length = ialloc(n);    /* length of the cycle   */
  check = ialloc(n);     /* keeps track of points already cycled */
  ncycles = 0;           /* number of cycles  */
  count = 0;             /* number of points already considered */
  max = 1;               /* maximum length of a cycle */
        
        
  for (i = 0; i < n; ++i)  
    check[i] = 0;

  for (i = 0; i < n; ++i)  {
    if (check[i] == 0)  {
      cyclptr[ncycles] = b + count;
      b[count] = i;
      check[i] = 1;
      length[ncycles] = 1;
      j =  a[i];
      while (check[j] == 0) {
        check[j] = 1;
        b[++count] = j;
        ++length[ncycles];
        j = a[j];
        }
      if (length[ncycles] > max)
        max = length[ncycles];
      ++ncycles;
      ++count;
      }
    }
        if (max ==1)  {
    printf (" the identity permutation\n");
    return;
    }
/* write out the cycles in increasing length ignoring those of length 1 */
  else {                   
    for ( i = 2; i <= max; ++i)  {
      for (j = 0; j < ncycles; ++j)  {
        if (length[j] == i)   {
          printf("(");
          for (k = 0; k < i; ++k)
            printf(" %d",*(cyclptr[j] + k));
          printf(")");
          }
        }
      }
      printf("\n");
      return;
    }
}

          
