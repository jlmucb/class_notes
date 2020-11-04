
#include "TCHAR.h"
#include "windows.h"
#include "string.h"
#include "MyTypes.h"
#include <wchar.h>
#include "stdio.h"

#define BUFSIZE 4096
#define NAMESIZE 256



void MakeName(
	Int iFnum, 					// Integer incrementor
	const String pProto, 		// Prototype Name
	String pNewName				// New Name
	)			

{
	Char *p, *q;
	Char NewBuffer[NAMESIZE];
	
	q= pProto;

	p= &NewBuffer[0];
	while((*q!=_T('.'))&&(*q!=_T('\0')))
		*(p++)= *(q++);
	*(p++)= _T('\0');
	_stprintf(pNewName,__TEXT("%s%d\0"),&NewBuffer[0],iFnum);
	p= pNewName;
	while(*p!=_T('\0'))
		p++;
	*(p++)= _T('.'); *(p++)= _T('a'); 
	*(p++)= _T('r'); *(p++)= _T('c');
	*(p++)= _T('\0');
	}



Int FileSplit(
	String InputName, 		// input file name
	String OutputName, 		// output file name (or null)
	Int iMaxSector			// maximum disk file size
	)

{
	HANDLE hIn, hOut;
	Int iFileSize, iFileLeft, iSectorSize, iSectorLeft, iFileNumber;
	BOOL fSuccess;

	Char Buffer[BUFSIZE];
	Char NewOutName[NAMESIZE];
	DWORD lpNumberOfBytesRead, lpNumberOfBytesWritten;			


	hIn= CreateFile(InputName, GENERIC_READ,FILE_SHARE_READ,NULL,
					OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hIn== INVALID_HANDLE_VALUE)
		return(FALSE);
	iFileSize= GetFileSize(hIn,NULL);
	iFileLeft= iFileSize;

	iFileNumber= 0;
	while(iFileLeft>0) 
		{
		
		MakeName(iFileNumber++, 
				(OutputName==NULL)?InputName:OutputName, 
				NewOutName);

		hOut= CreateFile(NewOutName, GENERIC_WRITE,0,NULL,
					CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hOut==INVALID_HANDLE_VALUE)
			return(FALSE);

		iSectorSize= ((iFileLeft>iMaxSector)?iMaxSector:iFileLeft);
		iSectorLeft= iSectorSize;
		while(iSectorLeft>0)
			{
			fSuccess=ReadFile(hIn,Buffer,
							  (iSectorLeft>BUFSIZE)?BUFSIZE:iSectorLeft,
    						  &lpNumberOfBytesRead,NULL); 
    		// Assert(fSuccess==TRUE);
			fSuccess= WriteFile(hOut,Buffer,lpNumberOfBytesRead,
    							&lpNumberOfBytesWritten,NULL);
    		// Assert(fSuccess==TRUE); 
			iSectorLeft-= lpNumberOfBytesRead;
			// Assert(lpNumberOfBytesWritten==lpNumberOfBytesRead);
			}
		
		CloseHandle(hOut);
		iFileLeft-= iSectorSize;
		}

	CloseHandle(hIn);
	return(TRUE);
	}



Int FileMerge(
	String InputName, 		// input file name
	String OutputName 		// output file name (or null)
	)

{
	HANDLE hIn, hOut;
	Int iFileSize, iFileLeft, iFileTotal, iFileNumber;
	BOOL fSuccess;

	Char Buffer[BUFSIZE];
	Char NewInName[NAMESIZE];
	DWORD lpNumberOfBytesRead, lpNumberOfBytesWritten;			


	hOut= CreateFile(OutputName, GENERIC_WRITE,0,NULL,
					CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hOut== INVALID_HANDLE_VALUE)
		return(FALSE);

	iFileNumber= 0;
	iFileTotal= 0;
	for(;;) 
		{
		MakeName(iFileNumber++, 
				(InputName==NULL)?OutputName:InputName, 
				NewInName);
		hIn= CreateFile(NewInName, GENERIC_READ,FILE_SHARE_READ,NULL,
					OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hIn== INVALID_HANDLE_VALUE)
			break;		

		iFileSize= GetFileSize(hIn,NULL);
		iFileLeft= iFileSize;
		while(iFileLeft>0)
			{
			fSuccess=ReadFile(hIn,Buffer,
							  (iFileLeft>BUFSIZE)?BUFSIZE:iFileLeft,
    						  &lpNumberOfBytesRead,NULL); 
    		// Assert(fSuccess==TRUE);
			fSuccess= WriteFile(hOut,Buffer,lpNumberOfBytesRead,
    							&lpNumberOfBytesWritten,NULL);
    		// Assert(fSuccess==TRUE); 
			iFileLeft-= lpNumberOfBytesRead;
			// Assert(lpNumberOfBytesWritten==lpNumberOfBytesRead);
			}
		
		CloseHandle(hIn);
		iFileTotal+= iFileSize;
		}

	CloseHandle(hOut);
	return(TRUE);
	}




