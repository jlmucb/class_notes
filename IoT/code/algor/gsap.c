/*
 *	Gale Shapeley for stable marraige
 */

int n;
int rboys[625];
int rgirls[625];
int accept[25];

main(argn,argv)

int argn; 
char *argv[];

{
	int i,j,k;
	int input;
	int oprop[25],prop[25];
	int lprop;

	if((input=open(argv[1],0))<0) {
		printf("\nCannot open %s, quitting\n",argv[1]);
		exit();
		}

	read(input,&n,4);
	for(i=0;i<n;i++)
		accept[i]= 0;
	read(input,&rboys[0],4*n*n);
	read(input,&rgirls[0],4*n*n);
	printf("\n\n%d couples\n",n);

	for(i=0;i<n;i++) {
		printf("\nBoy %d\t",i+1);
		for(j=0;j<n;j++)
			printf("%2d ",rboys[i*n+j]);
		}

	lprop=0;
	for(i=0;i<n;i++)
		oprop[i]=(prop[i]=0);

	/*
	 *	Male optimal solution
	 */
	for(i=0;i<n;i++)  {
		lprop++;
		/*
		 *	figure proposals
		 */
		for(j=0;j<n;j++) {
			}
		}

	printf("\n\n Done\n");
	exit();
}
