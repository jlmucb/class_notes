#include <stdio.h> 
#include <string.h>
#include <io.h>
// #include <fcntl.h>
// #include <sys/types.h>
// #include <sys/stat.h>
#include <stdlib.h> 


// ---------------------------------------------------------------------------------------


void copyPolytoPoly(int l1, int* pP1, int* pl2, int* pP2)
{
	int i;

	*pl2= l1;
	for(i=0; i<=l1; i++)
		pP2[i]= pP1[i];
}


void RecalculateConnection(int L, int* pCp, int* pbL, int* pBp, int N, int m)
{
	int i, k;

	if(L<=(N/2))
		k= N+1-L;
	else
		k= L;

	for(i=0; i<=k; i++) {
		if(i>L)
			pCp[i]= 0;
	}
	for(i=0; i<=(*pbL); i++) 
		pCp[i+N-m]^= pBp[i];
}

void printPoly(int L, int* pP)
{
	int i;
	for(i=0;i<=L;i++)
		printf("%d",pP[i]);
}

		
void printRound(int l, int* pS, int L, int* pCp, int m, int bL, int* pBp, int d,
		int tL, int* pTCp)
{
	printf("Bit #: %d, Value: %d, L: %d, m: %d, d: %d\n", l, pS[l], L, m, d);
	printf("Cp: "); printPoly(L, pCp); printf("\n");
	printf("Bp: "); printPoly(bL, pBp); printf("\n");
	printf("pTCp: "); printPoly(tL, pTCp); printf("\n");
}


// Berlekamp-Massey
int main(int an, char** av) 
{ 
	int 	n;  			// length of sequence
	int*	pS= NULL;		// sequence
	int*	pCp= NULL;		// connection polynomial
	int*	pTCp= NULL;		// temporary connection polynomial
	int*	pBp= NULL;		// previous connection polynomial
	int	    l;  			// position of current bit
	int	    m;  			// largest integer < l : L(s^m)<L(s^l)
	int	    L;  			// length of current min LFSR
	int 	bL; 			// length of previous poly
	int 	tL; 			// length of temporary poly
	int	    d;	    		// current discrepency
	int	    iPrintRound= 1;	// Print Round
    bool    bFirst= true;

	int 	i;
	char 	*p;

	if(an<=1) {
		printf("BerlMass seq\n");
		return(1);
	}

	n= 0;
	p= av[1];
	while(*p!=0) {
		n++;
		p++;
	}

	pS= (int*) malloc(sizeof(int)*n);
	if(pS==NULL) {
		printf("Cannot allocate S\n");
		goto Done;
	}
	pCp= (int*) malloc(sizeof(int)*n);
	if(pCp==NULL) {
		printf("Cannot allocate Cp\n");
		goto Done;
	}
	pTCp= (int*) malloc(sizeof(int)*n);
	if(pTCp==NULL) {
		printf("Cannot allocate TCp\n");
		goto Done;
	}
	pBp= (int*) malloc(sizeof(int)*n);
	if(pBp==NULL) {
		printf("Cannot allocate Bp\n");
		goto Done;
	}

	p= av[1];
	i= 0;
	while(*p!=0) {
		if(*p=='1')
			pS[i++]= 1;
		else
			pS[i++]= 0;
		p++;
	}

	if(an>2 && strcmp(av[2],"-p")==0)
		iPrintRound= 0;		// Print Round

	printf("Berlekamp-Massey, sequence length: %d.  Sequence: ", n);
	for(i=0; i<n; i++)
		printf("%1d", pS[i]);
	printf("\n");

	m= -1;
	L= 0;
	bL= 0;
	d= 0;
	tL= 0;
	pTCp[0]= 0;
	// C(D)= 1
	pCp[0]= 1;
	// B(D)= 1
	pBp[0]= 1;
	for(l=0;l<n;l++) {
		if(iPrintRound>0) {
			printf("\n\nRound before bit %d is processed.\n",l);
			printRound(l, pS, L, pCp, m, bL, pBp, d, tL, pTCp);
		}
		// calculate discrepency
		d= pS[l];
		for(i=1;i<=L;i++)
			d^= pCp[i]&pS[l-i];
		// no change
		if(d==0)
			continue;
		// recalculate connection polynomial
		copyPolytoPoly(L, pCp, &tL, pTCp);
		// C(D)= C(D)+B(D)D^(l-m)
		RecalculateConnection(L, pCp, &bL, pBp, l, m);
		if(L<=(l/2)) {	
			L= l+1-L;
			m= l;
			copyPolytoPoly(tL, pTCp, &bL, pBp);
		}
		if(iPrintRound>0) {
			printf("Connection modified after bit %d is processed.\n",l);
			printRound(l, pS, L, pCp, m, bL, pBp, d, tL, pTCp);
		}
	}

	printf("\nLength: %d, Value: ", L);
	for(i=0; i<=L; i++)
		printf("%1d", pCp[i]);
	printf("\n");

	printf("s[%d]= ", L);
	for(i=0; i<L; i++) {
        if(pCp[i]==0)
            continue;
        if(bFirst==true) {
	        printf("s[%d] ", L-1-i);
            bFirst= false;
        }
        else
	        printf("+ s[%d]", L-1-i);
    }
	printf("\n");

Done:
	if(pS!=NULL) {
		free(pS);
		pS= NULL;
	}
	if(pCp!=NULL) {
		free(pCp);
		pCp= NULL;
	}
	if(pTCp!=NULL) {
		free(pTCp);
		pTCp= NULL;
	}
	if(pBp!=NULL) {
		free(pBp);
		pBp= NULL;
	}
	return(0);
}


// ---------------------------------------------------------------------------------------




