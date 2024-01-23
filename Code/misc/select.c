

select (a,n,k)

int a[],n,k;

{
  extern int lt[], eq[],gt[];
  int i,n5,m,i1,i2,i3;
  int sb[500];

  if(n<50) {
    bubble(a,n);
    return(a[k-1]);
    }
  n5= n/5;
  for(i=0;i<n5;i++)
    bubble(&a[i*5],5);
  for(i=0;i<n5;i++)
    sb[i]= a[i*5+2];
  m= select(sb,(n5+1)/2);

  for(i=0;i<n;i++)
    switch(cmpr(m,a[i])) {
      case 1:
      lt[i1++]= a[i];
      break;
      case 0:
      eq[i2++]= a[i];
      break;
      case -1:
      gt[i3++]= a[i];
      break;
      }
  if(i1>=k) {
    for(i=0;i<i1;i++)
      a[i]= lt[i];
    return(select(a,i1,k));
    }
  if((i1+i2)>=k)
    return(m);
  for(i=0;i<i3;i++)
    a[i]= gt[i];
  return(select(a,i3,k-i1-i2));
}
  
