
// filetree.cpp
//	Search tree structure
//	John Manferdelli, 2003

#include <stdlib.h>
#include <stdio.h>
#define _WIN32_WINNT 0x0400
#include "windows.h"
#include <string.h>
#include <malloc.h>
#include <io.h>
#include <fcntl.h>

//#define NODEBUG


#include "sha.h"
#ifndef byte
typedef BYTE byte;
#endif
#ifndef SHA1
#define SHA1 0x0020
#endif

FILE*	g_ioOut= stdout;

//---------------------------------------------------------------------------------------


static DWORD	s_dwFile= 0x01;
static DWORD	s_dwDirectory= 0x02;
static DWORD	s_dwSystem= 0x04;
static DWORD	s_dwHidden= 0x08;


class MergeData {
public:
	char* 		m_szName;
	DWORD		m_dwFileType;
	byte 		m_rgbSha[20];
	int			m_iFileSize;
	FILETIME	m_oModTime;

	MergeData() { m_szName= NULL; m_dwFileType= 0; m_iFileSize= 0; };
	void Init() { m_szName= NULL; m_dwFileType= 0; m_iFileSize= 0; };
};


//---------------------------------------------------------------------------------------


// Read Scan
int			 g_iFiles1= 0;
MergeData*   g_rgData1= NULL;
MergeData**  g_rgpData1= NULL;


// Current Scan
int			 g_iFiles2= 0;
MergeData*   g_rgData2= NULL;
MergeData**  g_rgpData2= NULL;
MergeData**  g_rgScratch2= NULL;


//---------------------------------------------------------------------------------------


/*
	File Format

	<FileAudit>
	  <Directory> Dir-Name </Directory>
	  <Time>
	    <Date> yyyy/mm/dd </Date>
	    <TimeofDay> hh:mm ss.ss </TimeofDay>
	  </Time>

	  <File>
	    <FileName> name </FileName>
	    <FileType> FHSR </FileType>
	    <FileSizeBytes> nnnn </FileSizeBytes>
	    <LastModified>
		<Time> ... </Time>
	    </LastModified>
	    <SHA1Digest> nnnn </SHA1Digest>
	  </File>

	</FileAudit>
 */


//---------------------------------------------------------------------------------------


#define FILEBUFSIZE  4096
int		g_iSizeBuf= 0;
int 	g_iCurPos= 0;
byte 	g_rgbFileBuffer[FILEBUFSIZE];

byte 	g_rgbHashBuffer[FILEBUFSIZE];

#define LINEBUFSIZE  512
char	g_rgbLineBuffer[LINEBUFSIZE];

class List {
public: 
	List*	m_pNext;
	char*	m_pszHead;
	int	m_iSizeBuf;
	List() {m_pNext= NULL; m_pszHead= NULL; m_iSizeBuf= 0;};
};


// For character tape
const int	g_iDefaultTapeBlock= 50000;
int			g_iCurrentTapeLeft= 0;
char*		g_pszCurrPointer= NULL;
List*		g_poTapeList= NULL;


bool whitespace(char c)
{
	if(c==' ' || c=='\n' || c=='\t')
		return true;
	return false;
}


char*  getTape(int iSize)
{
	char*	pBuf= NULL;

	if(g_iCurrentTapeLeft>=iSize) {
		g_iCurrentTapeLeft-= iSize;
		pBuf= g_pszCurrPointer;
		g_pszCurrPointer+= iSize;
		return pBuf;
	}

	// get another Buffer
	List* pNew= new List;
	if(pNew==NULL)
		return NULL;

	pNew->m_pNext= g_poTapeList;
	g_poTapeList= pNew;
	pBuf= (char*)malloc((size_t)g_iDefaultTapeBlock);
	if(pBuf==NULL)
		return NULL;
	pNew->m_pszHead= pBuf;
	pNew->m_iSizeBuf= g_iDefaultTapeBlock;
	g_poTapeList= pNew;

	g_pszCurrPointer= pBuf+iSize;
	g_iCurrentTapeLeft= g_iDefaultTapeBlock-iSize;
	return pBuf;
}


int getac(int iInFile)
{
	if(g_iSizeBuf==0 || g_iCurPos>=g_iSizeBuf) {
		g_iSizeBuf= read(iInFile, &g_rgbFileBuffer, FILEBUFSIZE);
		if(g_iSizeBuf<0)
			return -1;
		g_iCurPos= 0;
		}
	return g_rgbFileBuffer[g_iCurPos++];
}


void ungetac(int iInFile)
{
	g_iCurPos--;
}


int getNextRealChar(int iInFile)
{
	int		ic;
	char 	c;

	for(;;) {
		ic= getac(iInFile);
		if(ic<0)
			return ic;
		c= (char) ic;
		if(!whitespace(c))
			break;
	}
	
	return ic;
}


const int s_EndOfFile=		-1;
const int s_Unknown=		99;
const int s_FileAudit=		 0;
const int s_endFileAudit=	 1;
const int s_Directory=		 2;
const int s_endDirectory=	 3;
const int s_Time=		 4;
const int s_endTime=		 5;
const int s_File=		 6;
const int s_endFile=		 7;
const int s_FileName=		 8;
const int s_endFileName=	 9;
const int s_FileType=		10;
const int s_endFileType=	11;
const int s_FileSizeBytes=	12;
const int s_endFileSizeBytes=	13;
const int s_LastModified=	14;
const int s_endLastModified=	15;
const int s_Date=		16;
const int s_endDate=		17;
const int s_TimeofDay=		18;
const int s_endTimeofDay=	19;
const int s_Sha1Digest=		20;
const int s_endSha1Digest=	21;


const int s_iMaxRun=		50;


const int s_NumTags= 22;
struct XMLTags {
	char*	m_szTag;
	int	m_iValue;
} rgTags[s_NumTags] =
{
	"FileAudit", s_FileAudit,
	"/FileAudit", s_endFileAudit,
	"Directory", s_Directory,
	"/Directory", s_endDirectory,
	"Time", s_Time,
	"/Time", s_endTime,
	"File", s_File,
	"/File", s_endFile,
	"FileName", s_FileName,
	"/FileName", s_endFileName,
	"FileType", s_FileType,
	"/FileType", s_endFileType,
	"FileSizeBytes", s_FileSizeBytes,
	"/FileSizeBytes", s_endFileSizeBytes,
	"LastModified", s_LastModified,
	"/LastModified", s_endLastModified,
	"Date", s_Date,
	"/Date", s_endDate,
	"TimeofDay", s_TimeofDay,
	"/TimeofDay", s_endTimeofDay,
	"Sha1Digest", s_Sha1Digest,
	"/Sha1Digest", s_endSha1Digest
};


int  TagId(char* sz)
{
	int i;

	for(i=0; i<s_NumTags; i++) {
		if(strcmp(sz,rgTags[i].m_szTag)==0)
			return rgTags[i].m_iValue;
	}

	return s_Unknown;
}


#define MAXTAGSTACK 5
int g_iUngottenTag= 0;
int g_rgiUnGottenTags[MAXTAGSTACK];


void UngetTag(int iTag)
{
	g_rgiUnGottenTags[g_iUngottenTag++]= iTag;
}


int NextTag(int iInFile)
{
	int n= 0;
	int ic= 0;

	if(g_iUngottenTag>0)
		return g_rgiUnGottenTags[--g_iUngottenTag];

	for(;;) {
		ic= getac(iInFile);
		if(ic<0)
			return -1;
		if(ic==(int)'<')
			break;
	}

	for(;;) {
		ic= getac(iInFile);
		if(ic<0)
			return -1;
		if(ic==(int)'>')
			break;
		if(n>(LINEBUFSIZE-1))
			return -1;
		g_rgbLineBuffer[n++]= (char) ic; 
	}
	g_rgbLineBuffer[n++]= 0;
	return TagId(g_rgbLineBuffer);
}


int getData(int iInFile)
{
	int n= 0;
	int ic= 0;
	char c;

	for(;;) {
		ic= getac(iInFile);
		if(ic<0)
			return -1;
		c= (int) ic;
		if(!whitespace(c)) {
			ungetac(iInFile);
			break;
		}
	}
	for(;;) {
		ic= getac(iInFile);
		if(ic<0)
			return -1;
		c= (int) ic;
		if(c=='<') {
			ungetac(iInFile);
			break;
		}
		if(n>(LINEBUFSIZE-1))
			return -1;
		g_rgbLineBuffer[n++]= (char) ic; 
	}
	g_rgbLineBuffer[n++]= 0;
	return n;
}


//---------------------------------------------------------------------------------------


const int s_NumFreeList= 100;


class SLList {
public:
	void*		m_pValue;
	SLList*		m_pNext;
	SLList() { m_pValue= NULL; m_pNext= NULL;};
};


class XMLTag {
public:
	int		m_iTag;
	char*		m_szValue;
	SLList*		m_pKids;
	XMLTag() { m_iTag= -1; m_szValue= NULL; m_pKids= NULL;};
};


SLList*		g_pFreeSLList= NULL;
XMLTag*		g_pFreeXMLTag= NULL;


SLList*	 getSLList()
{
	SLList*	 pL= g_pFreeSLList;

	if(pL==NULL)
		return NULL;
	g_pFreeSLList= pL->m_pNext;
	pL->m_pValue= NULL;
	pL->m_pNext= NULL;
	return pL;	
}


void FreeSLList(SLList* pM)
{
	SLList*	 pL= pM;
	
	if(pL==NULL)
		return;
	while(pL->m_pNext!=NULL)
		pL= pL->m_pNext;
	pL->m_pNext= g_pFreeSLList;
	g_pFreeSLList= pM;
}


XMLTag*	getXMLTag()
{
	XMLTag*	 pL= g_pFreeXMLTag;

	if(pL==NULL)
		return NULL;
	g_pFreeXMLTag= (XMLTag*) pL->m_pKids;
	pL->m_pKids= NULL;
	return pL;	
}


void FreeXMLTag(XMLTag* pM)
{
	if(pM==NULL)
		return;
	pM->m_pKids= (SLList*) g_pFreeXMLTag;
	g_pFreeXMLTag= pM;
	return;
}
			

void FreeXMLNode(XMLTag* pTag)
{
	SLList*		pList= NULL;
	XMLTag* 	pChildTag= NULL;

	if(pTag==NULL)
		return;
			
	pTag->m_iTag= 0;
	pList= pTag->m_pKids;
	while(pList!=NULL) {
		pChildTag= (XMLTag*)pList->m_pValue;
		FreeXMLNode(pChildTag);
		pList= pList->m_pNext;
		}
	FreeSLList(pTag->m_pKids);
	pTag->m_pKids= NULL;
	FreeXMLTag(pTag);

	return;
}


bool InitLists()
{
	int i;
	SLList* pL1= NULL;
	SLList* pL2= NULL;
	XMLTag* pL3= NULL;
	XMLTag* pL4= NULL;
	
	pL1= new SLList();
	if(pL1==NULL)
		return false;
	g_pFreeSLList= pL1;
	pL2= pL1;
	for(i=0; i<s_NumFreeList; i++) {
		pL1= new SLList();
		if(pL1==NULL)
			return false;
		pL2->m_pNext= pL1;
		pL2= pL1;
	}
	
	pL3= new XMLTag();
	if(pL3==NULL)
		return false;
	g_pFreeXMLTag= pL3;
	pL4= pL3;
	for(i=0; i<s_NumFreeList; i++) {
		pL3= new XMLTag();
		if(pL3==NULL)
			return false;
		pL3->m_pKids= NULL;
		pL4->m_pKids= (SLList*) pL3;
		pL4= pL3;
	}
	
	return true;
}


int getXMLNode(int iInFile, XMLTag** ppTag, int iMax)
{
	int j,n;
	int iTag;

	int ic= 0;
	int iLevel= 0;
	XMLTag* pCurrTag= NULL;
	XMLTag* pChildTag= NULL;
	SLList* pLastChild= NULL;
	SLList* pNextChild= NULL;

	if(--iMax<0)
		return(-1);
	*ppTag= NULL;

	iTag= NextTag(iInFile);
	if(iTag==s_EndOfFile || iTag==s_Unknown)
		return -1;
		
	if(*rgTags[iTag].m_szTag=='/') {
		UngetTag(iTag);
		return -1;
	}
	
	iLevel++;
	*ppTag= getXMLTag();    // new XMLTag();	// Replace
	if(*ppTag==NULL)
		return -1;

	(*ppTag)->m_iTag= iTag;
	(*ppTag)->m_szValue= NULL;
	(*ppTag)->m_pKids= NULL;

	// data value or tag
	for(;;) {
		ic= getNextRealChar(iInFile);
		ungetac(iInFile);
		if(ic==(int)'<') {
			// Another tag
			j= NextTag(iInFile);
			if(*rgTags[j].m_szTag=='/') {
				if(strcmp(rgTags[j].m_szTag+1,rgTags[iTag].m_szTag)==0) {
					iLevel--;
					if(iLevel==0)
						return 0;
				}
				else
					return -1;
			}
			
			UngetTag(j);

			if(getXMLNode(iInFile, &pChildTag, s_iMaxRun)<0)
				return -1;
				
			pNextChild= new SLList();
			if(pNextChild==NULL)
				return -1;
			pNextChild->m_pNext= NULL;
			if(pNextChild==NULL)
				return -1;
			if(pLastChild==NULL)
				(*ppTag)->m_pKids= pNextChild;
			else
				pLastChild->m_pNext= pNextChild;
			pNextChild->m_pValue= pChildTag;
			pLastChild= pNextChild;
		}	
		else {
			// collect Data
			n= getData(iInFile);
			if(n>0)
				(*ppTag)->m_szValue= strdup(g_rgbLineBuffer);
		}

	}
}


//---------------------------------------------------------------------------------------


int GetFileNameFromTag(XMLTag* pTag, char** ppszFileName)
{
	if(pTag==NULL)
		return(0);

	if(pTag->m_iTag==s_FileName) {
		*(ppszFileName)= pTag->m_szValue;
		return 1;
		}
	return 0;
}


int GetDirectoryFromTag(XMLTag* pTag, char** ppszDirName)
{
	if(pTag==NULL)
		return(0);

	if(pTag->m_iTag==s_Directory) {
		*(ppszDirName)= pTag->m_szValue;
		return 1;
		}
	return 0;
}


int GetShaFromTag(XMLTag* pTag, byte* rgbHash)
{
	int 	i= 0;
	char* 	p;

	if(pTag==NULL)
		return(0);

	p= pTag->m_szValue;
	while(whitespace(*p))
		p++;
	for(i=0; i<20; i++) {
		sscanf(p,"%02x", &rgbHash[i]);
		p+= 2;
		}
	return 1;
}



int GetFileTypeFromTag(XMLTag* pTag, DWORD* pdwType)
{
	int 	i= 0;
	char* 	p;

	if(pTag==NULL)
		return(0);

	*pdwType= 0;
	p= pTag->m_szValue;

	while(*p!=0) {
		if(*p=='F')
			*pdwType|= s_dwFile;
		if(*p=='D')
			*pdwType|= s_dwDirectory;
		if(*p=='H')
			*pdwType|= s_dwHidden;
		if(*p=='S')
			*pdwType|= s_dwSystem;
		p++;
		}
	return 1;
}


int GetFileSizeBytesFromTag(XMLTag* pTag, int* piSize)
{
	int 	i= 0;
	char* 	p;

	if(pTag==NULL)
		return(0);

	p= pTag->m_szValue;
	while(whitespace(*p))
		p++;
	sscanf(p,"%d", piSize);
	return 1;
}


int GetTimeFromTag(XMLTag* pTag, SYSTEMTIME* poTime)
{
	SLList*		pList= NULL;
	XMLTag* 	pChildTag= NULL;
	DWORD		dw1, dw2, dw3;
	bool		fSeenTimeOfDay= false;
	bool		fSeenDate= false;

	if(pTag==NULL)
		return(-1);
			
	if(pTag->m_iTag==s_Time) {
		poTime->wMilliseconds =0;
		poTime->wDayOfWeek = 0;
		poTime->wSecond= 0;
		pList= pTag->m_pKids;
		while(pList!=NULL) {
			pChildTag= (XMLTag*)pList->m_pValue;
			if(pChildTag->m_iTag==s_Date) {
				sscanf(pChildTag->m_szValue,"%4d/%d/%d",
				       &dw1,&dw2, &dw3);
				poTime->wYear= (WORD)dw1;
				poTime->wMonth= (WORD)dw2;
				poTime->wDay= (WORD)dw3;
				fSeenDate= true;
				}
			if(pChildTag->m_iTag==s_TimeofDay) {
				sscanf(pChildTag->m_szValue,"%d:%d",
				       &dw1,&dw2);
				poTime->wHour= (WORD)dw1;
				poTime->wMinute= (WORD)dw2;
				fSeenTimeOfDay= true;
				}
			pList= pList->m_pNext;
			}
		}

	if(fSeenTimeOfDay && fSeenDate)
		return 1;

	return 0;
}


int GetFileData(XMLTag* pTag)
{
	int 		n= 0;
	bool		fRet= false;
	SLList*		pList= NULL;
	XMLTag* 	pChildTag= NULL;
	bool		fSeenFileName= false;
	bool		fSeenFileType= false;
	bool		fSeenFileSizeBytes= false;
	bool		fSeenSha= false;
	bool		fSeenModifiedTime= false;
	bool		fSeenDirectoryType= false;
	MergeData*	pMergeData= NULL;

	try {
		// Expecting
		//	<File>
		//	<FileName>\jlm\work\aestables</FileName>
		//	<FileType>D</FileType>
		//	</File>
		// or
		//	<File>
		//	<FileName>\jlm\work\aestables\AesTables.cs</FileName>
		//	<FileType>F</FileType>
		//	<FileSizeBytes>12166</FileSizeBytes>
		//	<LastModified>
		//	<Time>
		//	<Date>2002/08/11</Date>
		//	<TimeofDay>21:49</TimeofDay>
		//	</Time>
		//	</LastModified>
		//	<Sha1Digest>bf031cba37998a2bcacec0c93c3843a969e7028e</Sha1Digest>
		//	</File>
		//
		//	Put in
		//
		//	int	     g_iFiles1= 0;
		//	MergeData*   g_rgData1= NULL;
		//	MergeData**  g_rgpData1= NULL;

		if(pTag==NULL)
			return(-1);
			
		if(pTag->m_iTag!=s_File)
			return -1;

		pMergeData= &g_rgData1[g_iFiles1];

		pList= pTag->m_pKids;
		while(pList!=NULL) {
			pChildTag= (XMLTag*)pList->m_pValue;
			if(pChildTag->m_iTag==s_FileName) {
				if(GetFileNameFromTag(pChildTag, 
						      &(pMergeData->m_szName))<=0)
					return -1;
				fSeenFileName= true;
				}
			else if(pChildTag->m_iTag==s_FileType) {
				 if(GetFileTypeFromTag(pChildTag, 
						      &(pMergeData->m_dwFileType))<=0)
					return -1;
				fSeenFileType= true;
				if(pMergeData->m_dwFileType&s_dwDirectory)
					fSeenDirectoryType= true;
				}
			else if(pChildTag->m_iTag==s_FileSizeBytes) {
				if(GetFileSizeBytesFromTag(pChildTag, 
						      &(pMergeData->m_iFileSize))<=0)
					return -1;
				fSeenFileSizeBytes= true;
				}
			else if(pChildTag->m_iTag==s_LastModified) {
				SLList*		pList2= pChildTag->m_pKids;
				XMLTag* 	pChildTag2= NULL;
				SYSTEMTIME  oSysTime;
				
				if(pList2==NULL)
					return -1;
				pChildTag2= (XMLTag*)pList2->m_pValue;
				if(GetTimeFromTag(pChildTag2, &oSysTime)<=0)
					return -1;
				// Convert
				if(!SystemTimeToFileTime(&oSysTime, &(pMergeData->m_oModTime)))
					return -1;
				fSeenModifiedTime= true;
				}
			else if(pChildTag->m_iTag==s_Sha1Digest) {
				if(GetShaFromTag(pChildTag, &(pMergeData->m_rgbSha[0]))<=0)
					return -1;
				fSeenSha= true;
				}
			pList= pList->m_pNext;
		}
	}
	catch(char* szError) {
		printf("Error: %s\n", szError);
		return(-1);
	}

	if(fSeenFileName && fSeenFileType && fSeenDirectoryType)
		return 1;

	if(fSeenFileName && fSeenFileType && fSeenFileSizeBytes && fSeenSha &&
	   		    fSeenModifiedTime && !fSeenDirectoryType) {
		g_iFiles1++;
		return 1;
	}

	return 0;
}


int GetPrologue(int iInFile, char** ppszOldBaseDir, SYSTEMTIME *poTime) 
{
	int 		n= 0;
	bool		fRet= false;
	SLList*		pList= NULL;
	XMLTag* 	pTag= NULL;
	XMLTag* 	pChildTag= NULL;
	bool		fSeenDirectory= false;
	bool		fSeenTime= false;

	try {
	 	// Expecting
		// 	<Directory>\jlm\work</Directory>
		// 	<Time>
		// 	<Date>2003/09/17</Date>
		// 	<TimeofDay>03:18</TimeofDay>
		// 	</Time>

		for(;;) {
			if((n=getXMLNode(iInFile, &pTag, s_iMaxRun))<0)
				throw("Bad Prologue");

			if(pTag->m_iTag==s_Directory) {
				if(GetDirectoryFromTag(pTag, ppszOldBaseDir)<=0)
					throw("Bad Prologue Node");
				fSeenDirectory= true;
			}

			else if(pTag->m_iTag==s_Time) {
				if(GetTimeFromTag(pTag, poTime)<=0)
					throw("Bad time tag in prologue");
				fSeenTime= true;
			}
			else
				throw("Bad tag in prologue");

			// Free node and associated data
			FreeXMLNode(pTag);
			pTag= NULL;
			if(fSeenDirectory && fSeenTime)
				break;
		}
	}
	catch(char* szError) {
		printf("Error: %s\n", szError);
		return(-1);
	}

	return 1;
}


bool GetOldScanData(char* szAuditFile, char** ppszOldBaseDir, 
					SYSTEMTIME *poPreviousSysTime) 
{
	int 		iInFile= 0;
	int 		n= 0;
	bool		fRet= false;
	XMLTag* 	pTag= NULL;

	iInFile= _open(szAuditFile, _O_RDONLY);
	g_iSizeBuf= 0;
	g_iCurPos= 0;

	if(iInFile<0) {
		printf("Cant open %s for writing\n");
		return false;
	}

	try {
		for(;;) {
			n= NextTag(iInFile);
			if(n==s_FileAudit) {
				if(GetPrologue(iInFile, ppszOldBaseDir, 
					       poPreviousSysTime)<0)
					throw("Bad Prologue");
				else
					break;
				}
			}
		for(;;) {
			if(getXMLNode(iInFile, &pTag, s_iMaxRun)<0) {
				if(NextTag(iInFile)==s_endFileAudit)
					break;
				throw("Bad Tag at top Level");
			}
			if(pTag==NULL)
				throw("Bad Tag");
			if(n<0)
				break;
			if(pTag->m_iTag==s_File) {
				GetFileData(pTag);
				}

			// Free node and associated data
			FreeXMLNode(pTag);
			pTag= NULL;
			}
		}
	catch(char* szError) {
		printf("Error: %s\n", szError);
	}

	_close(iInFile);
	return true;
}



//---------------------------------------------------------------------------------------



bool Merge(MergeData* rgSlot1[], int iSize1, MergeData* rgSlot2[], int iSize2, 
			      MergeData* rgScratch[], int(*iCompare)(MergeData*, MergeData*));
bool MergeSort(MergeData* rgSlots[], int iSize, MergeData* rgScratch[],
				  int(*iCompare)(MergeData*, MergeData*));


class Parameters {
public:
	int* 		m_piCount;
	MergeData*	m_poData;
	FILETIME	m_oCurrentTime;
};


int iFileCompare(MergeData* pA1, MergeData* pA2)
{
	return(strcmp(pA1->m_szName, pA2->m_szName));
}


bool Merge(MergeData* rgSlot1[], int iSize1, MergeData* rgSlot2[], int iSize2, 
			      MergeData* rgScratch[], int(*iCompare)(MergeData*, MergeData*))
{
	int		iPos1= 0;
	int		iPos2= 0;
	int		iOut= 0;

	for(;;) {
		if(iPos1>=iSize1 && iPos2>=iSize2)
			break;
		if(iPos1<iSize1 && iPos2>=iSize2) {
			rgScratch[iOut++]= rgSlot1[iPos1++];
			continue;
		}
		if(iPos2<iSize2 && iPos1>=iSize1) {
			rgScratch[iOut++]= rgSlot2[iPos2++];
			continue;
		}
		if(iCompare(rgSlot1[iPos1], rgSlot2[iPos2])==1) {
			rgScratch[iOut++]= rgSlot2[iPos2++];
			continue;
		}
		else {
			rgScratch[iOut++]= rgSlot1[iPos1++];
			continue;
		}
		break;
	}
	return true;
}


bool MergeSort(MergeData* rgSlots[], int iSize, MergeData* rgScratch[],
				  int(*iCompare)(MergeData*, MergeData*))
{
	int		i, j;

	if(iSize<=1)
		return true;

	i= iSize/2;
	j= iSize-i;

	if(!MergeSort(&rgSlots[0], i, &rgScratch[0], iCompare))
		return false;
	if(!MergeSort(&rgSlots[i], j, &rgScratch[i], iCompare))
		return false;
	if(!Merge(&rgSlots[0], i, &rgSlots[i], j, &rgScratch[0], iCompare))
		return false;

	for(i=0;i<iSize;i++)
		rgSlots[i]= rgScratch[i];

	return true;
}


//---------------------------------------------------------------------------------------


void printTime(FILE* ioOut, SYSTEMTIME oSysTime)
{
	fprintf(ioOut,"<Time>\n");
	fprintf(ioOut,"<Date>");
	fprintf(ioOut, "%04d/%02d/%02d", oSysTime.wYear, oSysTime.wMonth, oSysTime.wDay);
	fprintf(ioOut,"</Date>\n");
	fprintf(ioOut,"<TimeofDay>");
	fprintf(ioOut, "%02d:%02d", oSysTime.wHour, oSysTime.wMinute);
	fprintf(ioOut,"</TimeofDay>\n");
	fprintf(ioOut,"</Time>\n");
}


void printPrologue(FILE* ioOut, char* szDirectory, SYSTEMTIME oTime)
{
	fprintf(ioOut,"<FileAudit>\n");
	fprintf(ioOut,"<Directory>%s</Directory>\n", szDirectory);
	printTime(ioOut, oTime);
	fprintf(ioOut,"\n\n");
}


void printEpilogue(FILE* ioOut)
{
	fprintf(ioOut,"\n</FileAudit>\n");
}


void printFilePrologue(FILE* ioOut, char* szFileName, DWORD dwAtt)
{
	char		rgszAttr[4]={0, 0, 0, 0};
	int			i= 0;

	fprintf(ioOut,"\n<File>\n");
	fprintf(ioOut,"<FileName>%s</FileName>\n", szFileName);

	if(dwAtt&FILE_ATTRIBUTE_DIRECTORY){
		rgszAttr[i++]= 'D';
	}
	else {
		rgszAttr[i++]= 'F';
	}
	if(dwAtt&FILE_ATTRIBUTE_HIDDEN){
		rgszAttr[i++]= 'H';
	}
	if(dwAtt&FILE_ATTRIBUTE_READONLY){
		rgszAttr[i++]= 'R';
	}
	if(dwAtt&FILE_ATTRIBUTE_SYSTEM){
		rgszAttr[i++]= 'S';
	}

	fprintf(ioOut,"<FileType>%s</FileType>\n", rgszAttr);
}


void printFileEpilogue(FILE* ioOut)
{
	fprintf(ioOut,"</File>\n");
}


void printFileData(FILE* ioOut, WIN32_FIND_DATA* poFindFileData)
{
	// print size
	fprintf(ioOut, "<FileSizeBytes>%d</FileSizeBytes>\n", 
		poFindFileData->nFileSizeLow);

	// Modified Time
	SYSTEMTIME oSysTime;
	fprintf(ioOut, "<LastModified>\n");
	if(FileTimeToSystemTime(&(poFindFileData->ftLastWriteTime), &oSysTime)) {
		printTime(g_ioOut, oSysTime);
	}
	fprintf(ioOut, "</LastModified>\n");
}


void printShaDigest(FILE* ioOut, byte* rgbSha)
{
	int i;

	fprintf(ioOut, "<Sha1Digest>");
	for(i=0;i<20;i++)
		fprintf(g_ioOut, "%02x", rgbSha[i]);
	fprintf(ioOut, "</Sha1Digest>\n");
}


//---------------------------------------------------------------------------------------


bool HashFile(char* szFileName, int* piBitSizeHash, byte* pbBuf, unsigned uHash=SHA1)
{
	HANDLE 			hFile= NULL;
	bool			bRet=  false;
	BOOL			bResult;
	DWORD			dwBytesRead;

	if(piBitSizeHash==NULL)
		return false;
	if(*piBitSizeHash==0) {
		*piBitSizeHash= 160;
		bRet=  true;
	}

	try {
		if(*piBitSizeHash<160) {
			throw("Buffer too small");
			}
		if(pbBuf==NULL) {
			throw("No Output Buffer");
			}

		hFile= CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
                		  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
 		if (hFile == INVALID_HANDLE_VALUE)
        		throw("Could not open file");

		// hash
		Sha oHash;
		oHash.Init();

		for(;;) {
			bResult= ReadFile(hFile, g_rgbHashBuffer, FILEBUFSIZE, 
					  &dwBytesRead, NULL);
			if(bResult==FALSE || dwBytesRead == 0)
				break;
			
			oHash.Update(g_rgbHashBuffer, (int) dwBytesRead);
			}
		oHash.Final(pbBuf);
		bRet=  true;
	}
	catch(char* szError) {
		if(szError)
			printf("Error: %s\n");
		else
			printf("Unknown Error\n");
		}
		
	if(hFile)
		CloseHandle(hFile);
	return bRet;
}


//---------------------------------------------------------------------------------------


void recordFileInfo(char* szDir, WIN32_FIND_DATA* poFindFileData, void* pData)
{
	int		iLongFileNameSize;
	int		iHashSize= 160;
	char*		pszBuf;
	Parameters*	poData;
	MergeData*	pMergeData;

	// Dont do anything for Directories
	if(poFindFileData->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY){
		return;
	}

	if(pData==NULL)
		return;
	poData= (Parameters*) pData;
	
	// Any Room?
	if(*(poData->m_piCount)<=0)
		return;

	pMergeData= poData->m_poData;
	
	iLongFileNameSize= strlen(szDir)+strlen(poFindFileData->cFileName)+1;
	if((pszBuf=getTape(iLongFileNameSize))==NULL)
		return;
	strcpy(pszBuf, szDir);
	strcat(pszBuf, poFindFileData->cFileName);
	pMergeData->m_szName= pszBuf;

	// only check if datetime is earlier than in the parameters
	if(poData->m_oCurrentTime.dwHighDateTime<poFindFileData->ftLastWriteTime.dwHighDateTime)
		return;
	if(poData->m_oCurrentTime.dwHighDateTime==poFindFileData->ftLastWriteTime.dwHighDateTime &&
	   poData->m_oCurrentTime.dwLowDateTime<poFindFileData->ftLastWriteTime.dwLowDateTime)
		return;
	
	// Last Modified
	pMergeData->m_oModTime.dwHighDateTime= poFindFileData->ftLastWriteTime.dwHighDateTime;
	pMergeData->m_oModTime.dwLowDateTime= poFindFileData->ftLastWriteTime.dwLowDateTime;

	// File Size
	pMergeData->m_iFileSize= poFindFileData->nFileSizeLow;

	// hash
	if(!HashFile(pszBuf, &iHashSize, pMergeData->m_rgbSha))
		return;

	(*(poData->m_piCount))--;
	(poData->m_poData)++;
	return;
}


void printFileInfo(char* szDir, WIN32_FIND_DATA* poFindFileData, 
		   void* pData)
{
	int		iLongFileNameSize;
	char*		rgszLongFileName;
	int		iHashSize= 160;
	byte		rgbHash[20];

	iLongFileNameSize= strlen(szDir)+strlen(poFindFileData->cFileName)+1;
   	rgszLongFileName= (char*)_alloca((size_t) iLongFileNameSize);
	if(rgszLongFileName==NULL)
		return;

	strcpy(rgszLongFileName, szDir);
	strcat(rgszLongFileName, poFindFileData->cFileName);

	printFilePrologue(g_ioOut, rgszLongFileName, poFindFileData->dwFileAttributes);

	if(poFindFileData->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY){
		printFileEpilogue(g_ioOut);
		return;
	}

	// Print full information
	printFileData(g_ioOut, poFindFileData);

	// Print hash
	if(HashFile(rgszLongFileName, &iHashSize, rgbHash)) {
		printShaDigest(g_ioOut, rgbHash);
	}
	
	printFileEpilogue(g_ioOut);
}


int enumerate(char* szDirName, void (*Action)(char*, WIN32_FIND_DATA*, void*),
	      void* pData)
{
	int 			iFiles= 0;
  	WIN32_FIND_DATA 	oFindFileData;
  	HANDLE 			hFind= NULL;
	char*			rgszBaseDir= NULL;
	int				iSize= 0;
	int				iStarPos= 0;

	iSize= strlen(szDirName);
	iSize+= MAX_PATH+5;
	rgszBaseDir= (char*)_alloca((size_t)iSize);
	if(rgszBaseDir==NULL) {
		return(0);
		}
	strcpy(rgszBaseDir, szDirName);
	strcat(rgszBaseDir, "\\*");
	iStarPos= strlen(rgszBaseDir)-1;

	hFind= FindFirstFile(rgszBaseDir, &oFindFileData);
	rgszBaseDir[iStarPos]= 0;

	while(hFind != INVALID_HANDLE_VALUE) {

		if(strcmp(oFindFileData.cFileName,".")==0 || 
		   strcmp(oFindFileData.cFileName,"..")==0) {
			if(!FindNextFile(hFind, &oFindFileData))
				break;
			continue;
		}

		Action(rgszBaseDir, &oFindFileData, pData);

		if(oFindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY){
			strcat(rgszBaseDir, oFindFileData.cFileName);
			iFiles+= enumerate(rgszBaseDir, Action, pData);
			rgszBaseDir[iStarPos]= 0;
			}
		else
			iFiles++;

		if(!FindNextFile(hFind, &oFindFileData))
			break;
	}
	
	if(hFind)
		FindClose(hFind);

	return iFiles;
}


//---------------------------------------------------------------------------------------

bool  CmpHash(byte* rgbArray1, byte* rgbArray2, int iSize)
{
	int i;

	for(i=0;i<iSize;i++) {
		if(rgbArray1[i]!=rgbArray2[i])
			return false;
	}

	return true;
}

bool CompareScans()
{
	int			i;
	int			iList1= 0;
	int			iList2= 0;
	MergeData*	pData1;
	MergeData*	pData2;

	for(;;) {
		
		if(iList1>=g_iFiles1 && iList2>=g_iFiles2)
			break;

		if(iList1>=g_iFiles1) {
			// scanned files larger
			pData2= g_rgpData2[iList2];
			printf("Deleted File: %s\n", pData2->m_szName);
			iList2++;
			continue;
		}

		if(iList2>=g_iFiles2) {
			// old files larger
			pData1= &g_rgData1[iList1];
			printf("New File: %s\n", pData1->m_szName);
			iList1++;
			continue;
		}

		pData1= &g_rgData1[iList1];
		pData2= g_rgpData2[iList2];

		i= strcmp(pData1->m_szName, pData2->m_szName);

		if(i>0) {
			// scanned files larger
			pData2= g_rgpData2[iList2];
			printf("Deleted File: %s\n", pData2->m_szName);
			iList2++;
		}

		if(i==0) {
			if(CmpHash(pData1->m_rgbSha, pData2->m_rgbSha, 20))
				printf("%s unchanged\n", pData1->m_szName);
			else
				printf("%s modified\n", pData1->m_szName);
			iList1++;
			iList2++;
		}

		if(i<0) {
			// old files larger
			pData1= &g_rgData1[iList1];
			printf("New File: %s\n", pData1->m_szName);
			iList1++;
		}

	}

	printf("Done\n");

	return true;
}


bool GetCurrentScanData(char* szBaseDir, int iFiles) 
{
	int			nSlots= iFiles;
	int			iFile2= 0;
	Parameters	oParam;
	int			i;

	//	Current Time
	GetSystemTimeAsFileTime(&oParam.m_oCurrentTime);

	// Get New Data into Data Structure
	g_rgData2= (MergeData*) malloc(sizeof(MergeData)*((size_t)iFiles));
	g_rgpData2= (MergeData**)malloc((sizeof(MergeData*)*((size_t)iFiles)));
	g_rgScratch2= (MergeData**)malloc((sizeof(MergeData*)*((size_t)iFiles)));

	oParam.m_piCount= &nSlots;
	oParam.m_poData= g_rgData2;
	if((iFile2=enumerate(szBaseDir, recordFileInfo, &oParam))>iFiles)
		return false;

	// why?
	g_iFiles2= iFile2- *(oParam.m_piCount);

	for(i=0;i<iFile2;i++) {
		g_rgpData2[i]= g_rgData2++;
	}

#ifndef SORTON
	// Turns out it is already in sorted order

	// Sort it
	if(!MergeSort(g_rgpData2, iFile2, g_rgScratch2, iFileCompare))
		return false;
#endif
	return true;
}


//---------------------------------------------------------------------------------------


void CleanScanData()
{
	// free things up
	if(g_rgData1)
		free(g_rgData1);
	if(g_rgpData1)
		free(g_rgpData1);
	if(g_rgpData1)
		free(g_rgpData1);
	if(g_rgData2)
		free(g_rgData2);
	if(g_rgpData2)
		free(g_rgpData2);
	if(g_rgpData2)
		free(g_rgpData2);
}


int main(int argc, char* argv[])
{
	int 		iFiles= 0;
	char*		szBaseDir="c:";
	int			i;
	char*		szAuditFile= NULL;
	char*		szOutputFile= NULL;
	bool		fAudit= false;
	bool		fOutput= false;
	SYSTEMTIME	oCurrentSysTime;
	FILETIME	oCurrentFileTime;
	SYSTEMTIME	oPreviousSysTime;
	char*		szOldBaseDir= NULL;

	// Get Current Time
	GetSystemTimeAsFileTime(&oCurrentFileTime);
	if(!FileTimeToSystemTime(&oCurrentFileTime, &oCurrentSysTime))
		return 1;

	// Command Line Parameters
	if(strcmp(argv[1],"-help")==0) {
		printf("filetree Directory-to-audit [-o output-file] [-a audit-file]\n");
		return 0;
	}

	// Directory to Explore
	if (argc>1) {
		szBaseDir= argv[1];
	}

	// Output and audited file
	if (argc>2) {
		for(i=2; i<argc; i++) {
			if(strcmp(argv[i], "-o")==0) {
				fOutput= true;
				szOutputFile= argv[++i];
			}
			if(strcmp(argv[i], "-a")==0) {
				fAudit= true;
				szAuditFile= argv[++i];
			}
		}
	}

	// Record Output?
	if(fOutput) {
		g_ioOut= fopen(szOutputFile, "w");
		if(g_ioOut==NULL) {
			printf("Cant open %s for writing\n");
			return 1;
		}
	}

	// Prologue
	printPrologue(g_ioOut, szBaseDir, oCurrentSysTime);
  
	// Explore Files and inventory them
	iFiles= enumerate(szBaseDir, printFileInfo, NULL);
	printf("\n\nDirectory %s contains %d files\n", szBaseDir, iFiles);

	// Epilogue
	printEpilogue(g_ioOut);
	if(g_ioOut!=stdout) {
		fclose(g_ioOut);
		}

	// Do audit if called for
	if(fAudit) {

		try {

			InitLists();
			// Get New Data into Data Structure
			g_rgData1= (MergeData*) malloc(sizeof(MergeData)*((size_t)iFiles));

			// Get Current Data
			if(!GetCurrentScanData(szBaseDir, iFiles)) {
				throw("Can't record scanned data");
			}

			// Read old Scan
			if(!GetOldScanData(szAuditFile, &szOldBaseDir, &oPreviousSysTime)) {
				throw("Can't read scanned data");
			}

			// Compare them
			if(CompareScans()) {
				throw("Can't compare");
			}
		}
		catch(char* szError) {
			printf("Error: %s\n", szError);
		}

		// Clean Data if necessary
		// CleanScanData();	
	}

	return 0;
}


//---------------------------------------------------------------------------------------

#ifdef SPARE
int getline(int iInFile, char* szBuf, int iSizeBuf)
{
	int i= 0;
	int c= 0;
	
	for(i=0; i<iSizeBuf; i++) {
		c= getac(iInFile);
		if(c<0)
			return -1;
		
		szBuf[i]= (char) c;
		if(c==(int)'\n')
			break;
		}

	if(i>=(iSizeBuf-1))
		return -1;
	szBuf[++i]= 0;

	return i;
}
#endif


//---------------------------------------------------------------------------------------

