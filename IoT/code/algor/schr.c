#define NS 4
#define NR 100

int nsb={NS},nrb={NR},trace={0};
int m={12},e[12];
int pn[12]={-1,2,3,5,7,11,13,17,19,23,29,31};

sch(n,k)

int n,k;

/*
 *  Schroeppel factorization using continued fractions
 */

{
  int i,j,round,ns;
  int d,r,rx,u,ux,v,vx,p,px,a,s,t;

#ifdef DEBUG
  printf("sch(%d,%d)\n",n,k);
#endif

e1:
  ns= 0;
  round= 0;
  d= k*n;
  r= sqr(d);
#ifdef DEBUG
  printf("%d=sqt(%d)\n",r,d);
#endif
  rx= 2*r;
  u= (ux=rx);
  v= 1;
  vx= d-r*r;
  px= 1;
  p= r;
  a= 0;
  s= 0;
  if(trace!=0) {
    printf("\td: %d, k: %d, r: %d,rx: %d,",d,k,r,rx);
    printf("u: %d, ux: %d, v: %d,vx: %d",u,ux,v,vx);
    printf("p: %d, px: %d, a: %d,s: %d,t: %d\n",p,px,a,s,t);
    }

e2:
  round++;
  t= v;
  v= a*(ux-u)+vx;
  vx= t;
  a= u/v;
  ux= u;
  u= rx-(u%v);
  s= 1-s;

  /* assert: (p*p) - kn(q*q)= v (-1)**s, some q */

e3:
  e[0]= s;
  for(i=0;i<m;i++)
    e[i]= 0;
  t= v;
  for(i=0;i<m;i++) {
    for(;;)
      if((t%pn[i])!=0)
        break;
      else {
        t/= pn[i];
        e[i]++;
        }
    }


e4:
  if(t==1) {
    ns++;
    printf("Solution %d: %d sup 2 =",ns,p);
    for(j=0;j<m;j++)
      if(e[j]!=0)
        i= j;
    for(j=0;j<m;j++)
      printf("%d^%d",pn[j],e[j]);
    printf("mod %d\n",n);
    }
  if(trace!=0) {
    printf("\td: %d, k: %d, r: %d,rx: %d,",d,k,r,rx);
    printf("u: %d, ux: %d, v: %d,vx: %d",u,ux,v,vx);
    printf("p: %d, px: %d, a: %d,s: %d,t: %d\n",p,px,a,s,t);
    }

e5:
  if((v==1)||(ns>nsb)||(round>nrb)) {
    printf("Finished ns: %d, rounds: %d, v: %d\n",ns,round,v);
    return;
    }
  t= p;
  p= (a*p+px)%n;
  px= t;
  goto e2;
}


}


main(argn,argv)

int argn;
char *argv[];

{
  int i,j,k,n;

  printf("Schroepell factorization\n");
  if(argn<2) {
    printf("SCH n k nsols rndbnd trace\n");
    exit();
    }
  k=1;
  n= atoi(argv[1]);
  if(argn>2)
    k= atoi(argv[2]);
  if(argn>3)
    nsb= atoi(argv[3]);
  if(argn>4)
    nrb= atoi(argv[4]);
  if(argn>5)
    trace= 1;
  sch(n,k);
  exit();
}


