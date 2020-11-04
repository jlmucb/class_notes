
int trace={0};

euclid1(a,b)

int a,b;

{
	int u,v,r;

	printf("Euclid1(%d %d)\n",a,b);
	if(a>0)
		u= a;
	else if(a==0)
		return(b);
	else
		u= (-1)*a;
	if(b>0)
		v= b;
	else if(b==0)
		return(a);
	else
		v= (-1)*b;

	if(trace!=0)
		printf("\tEntering: (%d,%d)\n",u,v);

	for(;;) {
		if(u>v) {
			r= (u%v);
			if(r==0) {
				r= v;
				break;
				}
			if(r==1)
				break;
			u= r;
			}
		else {
			r= (v%u);
			if(r==0) {
				r= u;
				break;
				}
			if(r==1)
				break;
			v= r;
			}
		if(trace!=0)
			printf("\t(%d,%d)\n",u,v);
		}


	return(r);
}


euclid2(a,b)

int a,b;

{
	int u[3], v[3], r[3];
	int i;

	printf("Euclid2(%d %d)\n",a,b);
	if(a<0)
		a*= (-1);
	if(b<0)
		b*= (-1);
	if(a==0)
		return(b);
	if(b==0)
		return(a);
	if(b>a) {
		i= a;
		a= b;
		b= i;
		}

	u[0]= 1;
	v[0]= 0;
	u[1]= 0;
	v[1]= 1;
	r[0]= a;
	r[1]= b;

	if(trace>0) {
		printf("%d(%d) + %d(%d)= %d\n",u[0],a,v[0],b,r[0]);
		printf("%d(%d) + %d(%d)= %d\n",u[1],a,v[1],b,r[1]);
		}

	for(;;) {
		if(r[1]==0)
			break;
		i= r[0]/r[1];

		r[2]= r[0]-i*r[1];
		u[2]= u[0]-i*u[1];
		v[2]= v[0]-i*v[1];

		for(i=0;i<2;i++) {
			u[i]= u[i+1];
			v[i]= v[i+1];
			r[i]= r[i+1];
			}

		if(trace>0)
			printf("%d(%d) + %d(%d)= %d\n",u[2],a,v[2],b,r[2]);
		}

		printf("%d(%d) + %d(%d)= %d\n",u[0],a,v[0],b,r[0]);
		return(r[0]);
}



main(argn,argv)

int argn;
char *argv[];

{
	int i,k,n,a;

	printf("Euclid's algorithm\n");
	if(argn<3) {
		printf("GCD a b [traceflag]\n");
		exit();
		}

	n= atoi(argv[1]);
	k= atoi(argv[2]);
	if(argn>2)
		a= atoi(argv[3]);
	else
		a= 0;
	if(argn>4)
		trace= 1;
	else
		trace= 0;
	
	if(a!=0)
		i= euclid1(n,k);
	else
		i= euclid2(n,k);

	printf("%d= (%d,%d)\n",i,k,n);
	exit();
}


