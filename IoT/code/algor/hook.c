short int tem[1000];


/*
 *    Young tableaux
 */

insert(short int T[], short int n, short int x) {
  int i,j,k,t;

  for(i=0;i<n;i++) {
    t= x;
    for(j=0;j<n;j++)
      if(T[n*i+j]>t)
        break;
    if(T[i*n+j]>n) {
      T[i*n+j]= t;
      return((i<<16)|j);
      }
    x= T[i*n+j];
    T[i*n+j]= t;
    }

  return(0);
}


delete(short int T[],short int n,short int y,short int x) {
  int i,j,k,t;

  t= T[y*n+x];
  if(t>n)
    return(0);
  T[y*n+x]= n+1;

  for(i=(y-1);i>=0;i--) {
    for(j=0;j<n;j++)
      if(T[i*n+j]>n)
        break;
    for(j--;j>=0;j--) {
      if(t>T[i*n+j]) {
        k= T[i*n+j];
        T[i*n+j]= t;
        t= k;
        break;
        }
      }
    }

  return;
}


prtab(short int T[],short int n) {
  int i,j;

  for(i=0;i<n;i++) {
    if(T[i*n]>n)
      return;
    for(j=0;j<n;j++) {
      if(T[i*n+j]>n)
        break;
      printf(" %3d",T[i*n+j]);
      }
    printf("\n");
    }

  return;
}


init(short int T[], short int n) {
  int i,j;

  for(i=0;i<n;i++)
    for(j=0;j<n;j++)
      T[i*n+j]= n+1;

  return;
}


short int order[100]= {3,5,4,2,1};
int n={5};


main() {
  int i,j,k;
  int revorder[100];

  init(tem,5);
  printf("Young tableaux insertion %d elements\n\torder: ",n);
  for(i=0;i<n;i++)
    printf(" %3d",order[i]);
  printf("\n\n");

  printf("insert\n");
  for(i=0;i<n;i++) {
    revorder[i]= insert(tem,n,order[i]);
    printf("\n");
    prtab(tem,n);
    }
  printf("cascade: ");
  for(i=0;i<n;i++) {
    j= (revorder[i]>>16)&0177777;
    k= revorder[i]&0177777;
    printf(" %3d", tem[j*n+k]);
    }
  printf("\n");
  printf("delete\n");
  for(i=(n-1);i>=0;i--) {
    delete(tem,n,(revorder[i]>>16)&017777,revorder[i]&017777);
    printf("\n");
    prtab(tem,n);
    }

  printf("\ndone\n");
  exit();
}

/* ---------------------------------------------------------------- */

