/* 
 *	verticies 1,2,...,n
 *	edges (g[i].l,g[i].r)  i= 1,2,...,m
 */

struct edg {
	short int l,r;
	};
struct edg g[];
int n,m;


ec()

/*
 *	Elementary circuit
 *		Ref: Tiernan, CACM v13, no 12
 */

{
	int i,j,k;
	short int path[N];

	for(i=0;i<n;i++) {
	  path[0]= i;
	  k=0;
	  for(j=0;j<n;j++)
		clearclosed(j);
	  for(;;) {
	    /*
	     *	ec2
	     */
	    for(;;)
		if((j=extend(path[k]))>=0)
			path[k++]= g[j].r;
		else
			break;
		/*
		 *	ec3
		 */
		for(j=0;j<m;j++)
			if((g[j].l==path[k])&&(g[j].r==path[0])) {
				reportpath(path,k);
				break;
				}
		/*
		 *	ec4
		 */
		if(k==0)
			break;
		closevert(path[k-1],path[k]);
		clearclosed(path[k]);
		k--;
		}
	  }
	return;
}

