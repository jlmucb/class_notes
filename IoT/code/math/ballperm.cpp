#include "stdio.h"
#include "string.h"


// ---------------------------------------------------------------------------------


void printNameWithSpaces(int iSpacing, char* szStr)
{
	int j= iSpacing;
	if(szStr) {
		printf("%s", szStr);
		j-= strlen(szStr);
		}
	while(j-->0)
		printf(" ");
	}


char* MyDup(char* szStr)
{
	if(szStr==NULL)
		return(NULL);
	int		iStrSize= strlen(szStr);
	char*	szNew=  new (char [iStrSize+1]);
	if(szNew)
		strcpy(szNew, szStr);
	return(szNew);
	}


//
//	John Manferdelli
//
//	Rubik Sphere


class Perm {
public:
	bool	m_fInitialized;
	bool	m_fInverseInitialized;
	char*	m_szName;
	int		m_iSize;
	int*	m_rgPerm;
	int*	m_rgInvPerm;

	bool	m_fNamedElements;
	char**	m_rgszNames;

	Perm(char* szName, int iSize);
	~Perm();

	bool	CommonInit();
	bool	Init(int i, int* rgP, bool fCalcInv= true);
	bool	Init(int i, int* rgPIn, int* rgPOut, bool fCalcInv= true);
	bool	Init(int i, bool(* fp) (int* rgi), bool fCalcInv= true); 
	bool	CalculateInverse();
	bool	Apply(int n, int* rgIn, int* rgOut, bool fInverse= false);

	bool	InitNames(char** rgszNames);

	bool	printOut(int n, int* rgP);
	bool	printCycle(int n, int* rgP);
	};


Perm::Perm(char* szName, int iSize) 
{
	m_fInitialized= false; 
	m_fInverseInitialized= false;
	m_fNamedElements= false;
	m_szName= MyDup(szName); 
	m_iSize= iSize; 
	m_rgPerm= NULL; 
	m_rgInvPerm= NULL;
	m_rgszNames= NULL;
	}


Perm::~Perm() 
{
	m_fInitialized= false; 
	m_fInverseInitialized= false;
	if(m_szName) {
		delete m_szName;
		m_szName= NULL;
		}
	if(m_rgPerm) {
		delete [] m_rgPerm;
		m_rgPerm= NULL;
		}
	if(m_rgInvPerm) {
		delete [] m_rgInvPerm;
		m_rgInvPerm= NULL;
		}
	}


bool Perm::CalculateInverse() 
{
	if(m_fInverseInitialized)
		return(true);
	if(m_rgInvPerm)
		delete [] m_rgInvPerm;
	m_rgInvPerm= new (int [m_iSize]);
	if(m_rgInvPerm==NULL)
		return(false);

	int i;
	for(i=0; i<m_iSize; i++)
		m_rgInvPerm[m_rgPerm[i]]= i;
	
	return(true);
	}


bool Perm::CommonInit() 
{
	int i;
	
	m_rgPerm= new (int [m_iSize]); 
	if(m_rgPerm==NULL)
		return(false);
	for(i=0; i<m_iSize; i++)
		m_rgPerm[i]= i;	
	return(true);
	}


bool Perm::Init(int i, int* rgP, bool fCalcInv) 
{
	if(i!=m_iSize)
		return(false);
	if(!CommonInit())
		return(false);

	for(i=0; i<m_iSize;i++)
		m_rgPerm[i]= rgP[i];
	m_fInitialized= true; 
	if(fCalcInv)
		m_fInverseInitialized= CalculateInverse();
	return(true);
	}


bool Perm::Init(int n, int* rgPIn, int* rgPOut, bool fCalcInv) 
{
	if(!CommonInit())
		return(false);

	int i;
	for(i=0; i<n;i++)
		m_rgPerm[rgPIn[i]]= rgPOut[i];
	m_fInitialized= true; 
	if(fCalcInv)
		m_fInverseInitialized= CalculateInverse();
	return(true);
	}


bool Perm::Init(int i, bool(* fp) (int* rgi), bool fCalcInv) 
{
	if(!CommonInit())
		return(false);
	 
	if(!(*fp)(m_rgPerm))
		return(false);
	m_fInitialized= true;
	if(fCalcInv)
		m_fInverseInitialized= CalculateInverse();
	return(true);
	}


bool Perm::Apply(int n, int* rgIn, int* rgOut, bool fInverse)
{
	int i;

	if(!fInverse) {
		if(!m_fInitialized)
			return(false);
		for(i=0; i<m_iSize; i++)
			rgOut[i]= m_rgPerm[rgIn[i]];
		}
	else {
		if(!m_fInverseInitialized)
			return(false);
		for(i=0; i<m_iSize; i++)
			rgOut[i]= m_rgInvPerm[rgIn[i]];
		}

	return(true);
	}


bool	Perm::InitNames(char** rgszNames)
{
	m_rgszNames= new (char* [m_iSize]);

	int i;
	for(i=0; i<m_iSize;i++)
		m_rgszNames[i]= rgszNames[i];
	m_fNamedElements= true;
	return(true);
	}


bool	Perm::printOut(int n, int* rgP)
{
	int i, j, k;

	if(n!=m_iSize)
		return(false);

	int iSpacing= 0;
	int iMaxRow= 64;

	if(m_fNamedElements) {
		for(i=0; i<m_iSize; i++) {
			j= strlen(m_rgszNames[i]);
			if(j>iSpacing)
				iSpacing= j;
			}
		}
	else {
		iSpacing= 3;
		}
	iSpacing++;
	if(iSpacing>iMaxRow)
		return(false);

	int iNumPerRow= iMaxRow/iSpacing;
	int iNumRows= (m_iSize+iNumPerRow-1)/iNumPerRow;
	int iIndex;

	for(k=0; k<iNumRows; k++) {
		for(j=0; j<iNumPerRow; j++) {
			iIndex= k*iNumPerRow+j;
			if(iIndex>=m_iSize)
				continue;
			if(m_fNamedElements) {
				printNameWithSpaces(iSpacing, m_rgszNames[iIndex]);
				}
			else {
				printf("%03d ", iIndex);
				}
			}
		printf("\n");
		for(j=0; j<iNumPerRow; j++) {
			iIndex= k*iNumPerRow+j;
			if(iIndex>=m_iSize)
				continue;
			if(m_fNamedElements) {
				printNameWithSpaces(iSpacing, m_rgszNames[rgP[iIndex]]);
				}
			else {
				printf("%03d ", rgP[iIndex]);
				}
			}
		printf("\n\n");
		}
	

	return(true);
	}


bool	Perm::printCycle(int n, int* rgP)
{
	return(true);
	}


// ----------------------------------------------------------------------------------


char* g_rgszInitT[32]= {
	"00", "33",
	"10", "23",
	"20", "13",
	"30", "03",
	"33", "00",
	"23", "10",
	"13", "20",
	"03", "30",
	"01", "32",
	"11", "22",
	"21", "12",
	"31", "02",
	"32", "01",
	"22", "11",
	"12", "21",
	"02", "31"
	};

char* g_rgszInitR[64]= {
	"00", "01",
	"10", "11",
	"20", "21",
	"30", "31",
	"01", "02",
	"11", "12",
	"21", "22",
	"31", "32",
	"02", "03",
	"12", "13",
	"22", "23",
	"32", "33",
	"03", "04",
	"13", "14",
	"23", "24",
	"33", "34",
	"04", "05",
	"14", "15",
	"24", "25",
	"34", "35",
	"05", "06",
	"15", "16",
	"25", "26",
	"35", "36",
	"06", "07",
	"16", "17",
	"26", "27",
	"36", "37",
	"07", "00",
	"17", "10",
	"27", "20",
	"37", "30"
	};


char* g_rgszName[32] = {
	"00", "10", "20", "30", "01", "11", "21", "31", "02", "12", "22", "32", "03", "13", "23", "33", 
	"04", "14", "24", "34", "05", "15", "25", "35", "06", "16", "26", "36", "07", "17", "27", "37" 
	}; 


int posFromName(char* szName, char** rgszNames, int n)
{
	for(int i=0; i<n; i++) {
		if(strcmp(szName, rgszNames[i])==0)
			return(i);
		}
	return(-1);
	}


// ---------------------------------------------------------------------------------



int main(int an, char** av)
{
	int i;

	int rgiTmp[32];
	int rgiResult[32];
	int rgiResult1[32];
	
	for(i=0; i<32; i++)
		rgiTmp[i]= i;

	int rgIn[32];
	int rgOut[32];

	for(i=0;i<16; i++) {
		rgIn[i]= posFromName(g_rgszInitT[2*i], g_rgszName, 32);
		rgOut[i]= posFromName(g_rgszInitT[2*i+1], g_rgszName, 32);
		}
	Perm oPermT("T", 32);
	oPermT.Init(16, rgIn, rgOut, true); 
	oPermT.InitNames(g_rgszName);


	for(i=0;i<32; i++) {
		rgIn[i]= posFromName(g_rgszInitR[2*i], g_rgszName, 32);
		rgOut[i]= posFromName(g_rgszInitR[2*i+1], g_rgszName, 32);
		}
	Perm oPermR("R", 32);
	oPermR.Init(32, rgIn, rgOut, true); 
	oPermR.InitNames(g_rgszName);

	printf("apply T\n");
	oPermT.Apply(32, rgiTmp, rgiResult);
	oPermT.printOut(32, rgiResult);

	printf("apply R\n");
	oPermR.Apply(32, rgiTmp, rgiResult1);
	oPermR.printOut(32, rgiResult1);

	return(0);
	}


// ---------------------------------------------------------------------------------


