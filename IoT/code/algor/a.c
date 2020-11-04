
main()
{
	int i,j,k;
	float a,b,c;

	a= 0;
	for(i=1;i<1000000;i++) {
		/* j= (i*i-1)*a+(i+1); a= j/(i*i+1); */
		b= (i*i-1);
		c= (i+1);
		a= (b*a+c)/(b+2);
		if((i%100)==0)
			printf("a[%d]= %f\n",i,a);
		}

	exit();
}

