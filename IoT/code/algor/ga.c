
/*
 *	Heapsort
 *	a[i], sons: a[2i], a[2i+1]
 *	heapify(i,j)
 *		if i is not a leaf and sone of i contains a larger elt than i
 *			begin
 *			let k be the son of i with largest elt
 *			switch a[i] and a[k]
 *			heapify(k,j)
 *			end
 *
 *	buildheap
 *		for (i=n; i--;i>=1)
 *			heapify(i,n);
 *
 *	heapsort
 *		buildheap
 *		for (i=n; i--;i>=2)
 *			switch a[1] and a[i]
 *			heapify(1,i-1)
 *
 *	select(k,S)
 *		if(ord(S)<50) {
 *			sort(S)
 *			return s[k-1]  (kth elt)
 *			}
 *		for (i=0;i++,i<= ord(S)/5)
 *			sort(S[5i],...,S[5i+4])
 *			let T[i]= S[5i+2];
 *		m= select((ord(T)+1)/2,T)
 *		let S1= S[i]<m
 *			S2= S[i]==m
 *			S3= S[i]>m
 *		if(ord(S1)>=k)
 *			return(select(k,S1)
 *		if(ord(S1)+ord(S2)>=k)
 *			return(k)
 *		return(select(k-ord(S1)-ord(S2),S3)
 */

 /*
  *	Union find
  *
  *	1. R[i]= internal name
  *	2. List[A], Size{a}, EXT_NAME[], INT_NAME[]
  *
  *	union(i,j,k)
  * 	a=int_name[i]
  *		b= int_namr[j]
  *		wlog size[a]<= size[b]
  *		e= list[a]
  *		while(e!=0) {
  *			R[e]= b
  *			l= e;
  *			e= next[e];
  *			}
  *		next[l]= list[b];
  *		list[b]= list[a]
  *		size[b]+= size[a]
  *		int_name[k]= b
  *		ext_name[b]= k
  *
  */

  /*
   *	Tarjan's union find
   *
   *	count[vi]= 1, name[vi]= i, f[vi]= 0, root[i]= vi, e[i]= vi
   *
   *	find(i)
   *		list= 0
   *		v= e[i]
   *		while(f[v]!= 0) {
   *			add v to list
   *			v= f[v]
   *			}
   *		print name[v]
   *		for each w on list
   *			f[w]= v
   *
   *	union(i,j,k)
   *		wlog count(root(i))<= count(root(j))
   *		large= root(j)
   *		small= root(i)
   *		f[small]= large
   *		count(large)+= count(small)
   *		name(large)= k
   *		root(k)= large
   *
   *	FFT
   *
   *	for(i=0;i<= 2^k -1;i++)
   *		r[i]= a[i]
   *	for(l=0;l<=k-1;l++) {
   *		s[i]= r[i]
   *	    for(i=0;i<= 2^k -1;i++)
   *			i= Sum(j=0,k-1)d[k-1-j] 2^j
   *            r[d0,d1,...d(k-1)]= s[d0...d(l-1)0,d(l+1)...d(k-1)]
   *								+ u^[dl,...,d0,0...0] S[d0,..d(l-1)1d(l+1)...]
   *		}
   *	for(i=0;i< 2^(k-1);i++)
   *		b[d0,...,d(k-1)]= r[d(k-1)..d0]
   *
   *	fft as evaluation of poly at a root of 1
   *
   *	r[0,k]= Sum(j=0,n-1)a[j] x^j
   *	/* r[l,m]= Sum(j,n-1)a[j] x^j /q[l,m] * /
   *    for (m=k-1;k--;k>=0) {
   *		for(l=0;l+= 2^(m+1), l<=n-1) {
   *			r[l,m+1]= Sum(j=0,2^(m+1)) a[j] x^j
   *			s= rev(l/(2^m))
   *			r[l,m]= Sum(j=0,2^m -1)(a[j]+u^s a[j+2^m]x^j
   *			r[l+2^m,m]= Sum(j=0,2^m-1)(a[j]+u^(s+n/2)) x^j
   *			}
   *		}
   *	for(l=0;l<=(n-1);n++)
   *		b[rev(l)]= r[l,0]
   *
   *	Eval is O(nlg(n))
   *		q[l,m]= Prod(j=l,l+2^m-1) (x-c[j]); q[0,k]= p(x), q[l,0]= (x-c[l])
   *
   *
   *	x[n+1](ax[n]+c)(mod m)
   *	period length (c,m)=1, b=a-1=kp for p|m, 4|b if 4|m
   *
   *	chi test chi(Y)= Sum(j=1,k) ((y[j]-np[j])**2/(np[j])
   *
   *	freq test, serial test (q,r) recurring,gap test
   *	coupon test (length to obtain all), perm test, run up/down
   *	spectral test
   */

