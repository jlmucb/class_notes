#include <stdio.h> 
#include <string.h>
#include <io.h>
// #include <fcntl.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <stdlib.h> 


// ---------------------------------------------------------------------------------------


class GenerateSymmetric {
public:
    int         m_n;
    int*        m_piPerms;
    int         m_iSizeofPerm;

    GenerateSymmetric(int in);
    ~GenerateSymmetric();
    int GenSn(int iLastStart, int iThisStart, int i, int m);
    int GenPerms();
	void WeaveAllPermsAtPosition(int n, int* piStartOfOldPerms, int* piStartOfNewPerms, 
                                 int iNumOldPerms, int iPos);
    void SingleWeave(int n, int* piStartOfOldPerm, int* piStartOfNewPerm, int iPos);

};


GenerateSymmetric::GenerateSymmetric(int in) 
{
    m_n= in;
    m_piPerms= NULL;
    m_iSizeofPerm= 0;
}


GenerateSymmetric::~GenerateSymmetric () 
{
    if(m_piPerms!=NULL) {
        delete m_piPerms;
        m_piPerms= NULL;
    }
}


void GenerateSymmetric::SingleWeave(int n, int* piStartofSingleOldPerm, int* piStartofSingleNewPerm, int iPos)
{
    int i;

    for(i=0; i<iPos;i++) {
        piStartofSingleNewPerm[i]= piStartofSingleOldPerm[i];
    }
    piStartofSingleNewPerm[i++]= n-1;
    for(;i<n;i++) {
        piStartofSingleNewPerm[i]= piStartofSingleOldPerm[i-1];
    }
	return;
}



void GenerateSymmetric::WeaveAllPermsAtPosition(int n, int* piStartOfOldPerms, int* piStartOfNewPerms, int iNumOldPerms, int iPos)
{
        int j;

		for(j=0; j<iNumOldPerms; j++) {
             SingleWeave(n, piStartOfOldPerms, piStartOfNewPerms, iPos);
		     piStartOfOldPerms+= n-1;
			 piStartOfNewPerms+= n;
        }
        return;
}


int GenerateSymmetric::GenSn(int iLastStart, int iThisStart, int n, int nFactorial)
{
        int iLastFactorial= nFactorial/n;
        int j;
        int* piLastPerms= &m_piPerms[iLastStart];
        int* piNextPerms= &m_piPerms[iThisStart];

		for(j=(n-1); j>=0; j--) {
			WeaveAllPermsAtPosition(n, piLastPerms, piNextPerms, iLastFactorial, j);
			piNextPerms+= iLastFactorial*n;
		}
        return(1);
}


int GenerateSymmetric::GenPerms()
{ 
    int i, m;
    
    // nothing to do?
    if(m_n<1)
        return(1);

    // How much room?
    m= 1;
    for(i=1; i<=m_n; i++) {
        m*= i;
        m_iSizeofPerm+= m*i;
    }
    
    // get room
    m_piPerms= new int [m_iSizeofPerm];
    if(m_piPerms==NULL) {
        printf("Can't alloc room to store permutations (%d integers)\n", m_iSizeofPerm);
        return(1);
    }

    // S[1]
    m_piPerms[0]= 0;
     
    // Generate S[i] from S[i-1]
    int iLastStart= 0;
    int iThisStart= 1;

    m= 1;
    for(i=2; i<=m_n; i++) {
        m*= i;
        GenSn(iLastStart, iThisStart, i, m);
        iLastStart= iThisStart;
        iThisStart+= m*i;
    }

    return(0);
}



// ---------------------------------------------------------------------------------------


int iDumpSn= 5;


void DumpSn(int n, int nFactorial, int* pBase, char* strDumpFile)

{
	int iFile;

    printf("DumpSn %d %d %s\n", n, nFactorial, strDumpFile);
	if (strDumpFile==NULL)
		return;

	iFile= creat(strDumpFile, 0x0080);
	if(iFile<0) {
		printf("Output Error\n");
		// printf("Output Error %d\n", errno);
		return;
	}
	write(iFile, &n, sizeof(int));
	write(iFile, &nFactorial, sizeof(int));
	write(iFile, pBase, n*nFactorial*sizeof(int));
	close(iFile);
}



int main(int an, char** av) 
{ 
    int n= 1;
    int i, j, k, l, m;
    int* pBase;
    int* pPermBase;

    if(an>1) {
            sscanf(av[1], "%d", &n);
    }

    // Generate permutations
    printf("Generate S[%d]\n", n);
    GenerateSymmetric* poPerm= new GenerateSymmetric(n);
    poPerm->GenPerms();

    // Print them out
    pBase= poPerm->m_piPerms;
    m= 1;
    for(i=1;i<=n;i++) {
        m*= i;
        printf("\n\nS(%d), order %d\n", i, m);
		pPermBase= pBase;
		if(iDumpSn==i && an>2) {
			DumpSn(i, m, pBase, av[2]);
		}
        for(j=0; j<m; j++) {       
            printf("\t");
            for(k=0; k<i; k++) {
                l= pPermBase[k];
                printf("%02d ", l);
            }
			printf("\n");
			pPermBase+= i;
        }
        pBase+= m*i;
    }

    return(0);
}


// ---------------------------------------------------------------------------------------




