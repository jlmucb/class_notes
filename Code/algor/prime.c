
int pp[]={2,3,5,7,11,13,17,19,23,29,31,37,41,47};


isprime(i)

int i;

{
  int j,k,l,m;
  double x,y;
  extern double sqrt();

  for(j=0;j<10;j++) {
    if(i==pp[j])
      return(1);
    if((i%pp[j])==0)
      return(0);
    }
  x= i;
  y= sqrt(x);
  k= y;
  for(j=2;j<k;j++)
    if((i%j)==0)
      return(0);
  return(1);
}



main (an,av)

int an;
char *av[];

{
  int i,j,k;

  i= atoi(av[1]);
  j= atoi(av[2]);
  printf("Primes between %d and %d\n",i,j);
  for(k=i;k<j;k++)
    if(isprime(k)>0)
      printf("%d is prime\n",k);
  exit();
}
