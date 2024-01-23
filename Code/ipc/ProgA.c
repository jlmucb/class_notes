//
// Program A
// 	Creates and Event called "JMGEvent1"
// 	Spawns Progrm B
// 	Opens Socket to B
// 	Sends B a signal
// 	Sends B some strings, includind a termination string
// 	Waits for B to terminate
//
//	For Illustrative purposed only
//
//	Windows specific Calls: CreateEvent, SetEvent, close
//		also Windows socket calls
//
//	This works just fine in Linux too except that the Event mechanism must be
//	replaced by signals.

#include <windows.h>
#include <stdio.h>
#include <io.h>

#define SZEVENTNAME 	"JMGSignalEvent1"
#define SZKILLEVENT		"JMGKillIt"
#define SZSOCKETNAME 	"JMGSocket1"

//$ Change the next line to point to new executable
#define SZCHILDAPP 		"c:\\\\jlmtemp\\VisProj\\ProjB\\Debug\\ProjB"


// ----------------------------------------------------------------------------------


static HANDLE		g_hSignalEvent= NULL;
static HANDLE		g_hKillEvent= NULL;
static int			g_iChildId= 0;
static SOCKET		g_oSock;
static HANDLE		g_hAcceptThread= NULL;
static DWORD		g_dwThreadId= 0;


void MyZero(byte* pAdd, int iSize)
{
	while(iSize-->0)
		*(pAdd++)= 0;
}


#define MAXBUF	1024
DWORD WINAPI RunAccept(LPVOID pvParam)
{
	
	SOCKET	oOutSock;
	char	rgBuf[MAXBUF];
	int		iError= 0;

	while(1) {
		oOutSock= accept(g_oSock, NULL, NULL);
		if(INVALID_SOCKET ==oOutSock) {
			DWORD dwErr= WSAGetLastError();
			break;
		}
		iError= recv(oOutSock, rgBuf, MAXBUF, 0);
		if(iError==SOCKET_ERROR){
			break;
		}
		if(iError==0){
			break;
		}
		printf("Got Buffer: %s\n", rgBuf);
	}

	return(0);
}


BOOL InitializeEvent(LPTSTR szEventName, LPTSTR szKillName)
{
	g_hSignalEvent= CreateEvent(
		       		NULL,		// LPSECURITY_ATTRIBUTES
					TRUE,		// BOOL bManualReset, 
					FALSE,		// BOOL bInitialState, 
					szEventName // LPTSTR lpname
		       		); 
	if(g_hSignalEvent==NULL)
		return(FALSE);

	g_hKillEvent= CreateEvent(
		       		NULL,		// LPSECURITY_ATTRIBUTES
					TRUE,		// BOOL bManualReset, 
					FALSE,		// BOOL bInitialState, 
					szKillName	// LPTSTR lpname
		       		); 
	if(g_hKillEvent==NULL)
		return(FALSE);

	return(TRUE);
}



BOOL SpawnChild(LPTSTR szProgName, PROCESS_INFORMATION* poProcessInfo)
{
	BOOL fStarted= FALSE;
	STARTUPINFO oStartup;
	DWORD dwError= 0;

	MyZero((byte*) &oStartup, sizeof(STARTUPINFO));
	MyZero((byte*) poProcessInfo, sizeof(PROCESS_INFORMATION));

	fStarted= CreateProcess(
			NULL,			// Name
			szProgName,		// LPTSTR lpCommandLine, 
			NULL,			// LPSECURITY_ATTRIBUTES lpProcessAttributes, 
			NULL,			// LPSECURITY_ATTRIBUTES lpThreadAttributes, 
			FALSE,			// BOOL bInheritHandles, 
			CREATE_NEW_CONSOLE,			// DWORD dwCreationFlags, 
			NULL,			// LPVOID lpEnvironment, 
			NULL,			// LPCTSTR lpCurrentDirectory, 
			&oStartup,		// LPSTARTUPINFO lpStartupInfo, 
			poProcessInfo	// LPPROCESS_INFORMATION lpProcessInformation
		    ); 

	if(!fStarted)
		dwError= GetLastError();

	return(fStarted);
}



#define TEST
#ifdef TEST

int main(int an, char** av)
{
	PROCESS_INFORMATION oProcessInfo;
	char szBuf[128];
	BOOL fSocket= FALSE;

	if(!InitializeEvent(SZEVENTNAME, SZKILLEVENT)) {
		printf("Failed to Spawn: %s\n", SZEVENTNAME);
		return(1);
	}
	if(!SpawnChild(SZCHILDAPP, &oProcessInfo)) {
		printf("Failed to Spawn: %s\n", SZCHILDAPP);
		goto Done;
	}

	for(;;) {
		printf("SetEvent (q for quit)? ");
		read(0, szBuf, 1);
		if(szBuf[0]=='q') {
			SetEvent(g_hKillEvent);
			break;
		}
		SetEvent(g_hSignalEvent);
	}

Done:
	if(g_hSignalEvent!=NULL)
		CloseHandle(g_hSignalEvent);
	g_hSignalEvent= NULL;
	if(g_hKillEvent!=NULL)
		CloseHandle(g_hKillEvent);
	g_hKillEvent= NULL;
}

#endif

// ----------------------------------------------------------------------------------



