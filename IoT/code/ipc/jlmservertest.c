/*
   gcc jlmservertest.c -lWs2_32 -o c:\nl\bin\server.exe
   server.exe -P client.exe


*/
// Jerrold's Server Program
//
// Calling Sequence
//		jmgServer -P Client-Program
// Initializes Server on Port Number MYPORT and then calls Client-Program
//
//	For Illustrative purposes only
//	This works just fine in Linux too except that the Event mechanism must be
//	replaced by signals.


#include <windows.h>
#include <stdio.h>
#include <io.h>

#define SZSOCKETNAME 	"JMGSocket"

//#define SZKILLEVENT		"JMGKillIt"

#define MYPORT			2400

#define MAXBUF			16384

//    gcc z:/home/jmg1/jlmservertest.c -lWs2_32
// ------------------------------------------------------------------------------------------------


static SOCKET		g_oSock;					// Socket to listen on
static DWORD		g_dwMyProcessId= 0;			// This Process Id
static DWORD		g_dwChildProcessId= 0;		// Id of Process spawned by CreateProcess
static HANDLE		g_hKillEvent= NULL;			// Handle of Event that notifies child to terminate
static HANDLE		g_hAcceptThread= NULL;		// Thread to listen for Kill
DWORD				g_dwChildThreadId= 0;


void MyZero(byte* pAdd, int iSize)
{
	while(iSize-->0)
		*(pAdd++)= 0;
}


BOOL InitializeEvent(LPTSTR szKillName)
{
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


BOOL WaitforEvent(LPTSTR szKillEventName)
{
	DWORD	dwWait= 0;
	BOOL	fReturn= FALSE;

	if(g_hKillEvent==NULL)
		goto Done;
	fReturn= TRUE;

	for(;;) {
		dwWait= WaitForSingleObject(g_hKillEvent, 200);
		if(dwWait!=WAIT_TIMEOUT) {
				printf("Got Kill Event\n");
				break;
		}
	}

Done:
	if(g_hKillEvent!=NULL)
		CloseHandle(g_hKillEvent);
	g_hKillEvent= NULL;
	return(fReturn);
}


void CloseEvent()
{
	if(g_hKillEvent!=NULL) {
		CloseHandle(g_hKillEvent);
		g_hKillEvent= NULL;
	}

	return;
}


/*

DWORD WINAPI RunListen(LPVOID lpParam)
{
	if(WaitforEvent(SZKILLEVENT))
		return(1);
	return(0);
}

*/


BOOL SpawnChild(LPTSTR szProgName, PROCESS_INFORMATION* poProcessInfo)  // spawn
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


BOOL InitializeServerSocket()
{
	int 	iSize= 0;
	int	iError= 0;
	WSADATA wsaData;
	struct	sockaddr_in oAddress;
	BOOL	fReturn= FALSE;
	struct	hostent* poHost;
	DWORD	dwErr= 0;
	char	szName[MAXBUF];


	// Windows specific check
	if (WSAStartup(MAKEWORD(2, 2),&wsaData)!= 0)
		goto Done;
	if (LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!= 2)
		goto Done;

	if((g_oSock= socket(AF_INET, SOCK_STREAM, 0))==INVALID_SOCKET)
		return(FALSE);

	iSize= sizeof(&oAddress);
	MyZero((byte*)&oAddress, sizeof(oAddress));
	oAddress.sin_family= AF_INET;
	oAddress.sin_port = MYPORT;
	// The following 4 lines are not actually used since we know on the local machine the address of the
	// loopback endpoint is 127.0.0.1.  However, for remote machines, this can be used to identify local
	// addresses
	if(gethostname(szName, MAXBUF)>0)
		goto Done;
	if((poHost=gethostbyname(szName))==NULL)
		goto Done;

    // host address (same machine)
    oAddress.sin_addr.S_un.S_un_b.s_b1= 127;
	oAddress.sin_addr.S_un.S_un_b.s_b2= 0;
	oAddress.sin_addr.S_un.S_un_b.s_b3= 0;
	oAddress.sin_addr.S_un.S_un_b.s_b4= 1;

	if((iError=bind(g_oSock,(struct sockaddr *) &oAddress, 
sizeof(oAddress)))!=0) {
		dwErr= GetLastError();
		goto Done;
	}
	if(listen(g_oSock, 5)!=0)
		goto Done;
	fReturn= TRUE;

Done:
	return(fReturn);
}


BOOL CloseSocket()
{
	// shutdown(g_oSock, SD_BOTH);
	closesocket(g_oSock);
	WSACleanup();
	return(TRUE);
}


int main(int an, char** av)
{
	PROCESS_INFORMATION		oProcessInfo;
	int        i= 0;
	SOCKET	   oOutSock;
	char*	   szClientProgramName= NULL;
	DWORD	   dwErr;
	int			iError= 0;
	char	   rgBuf[MAXBUF];
	char       sgBuf[MAXBUF];
	char *p,*q;
	int zcount= 0;
	int stopper= 0;
	int flipper= 0;
	int client_error; 
	int iSize; 

	// Get Client Program
	for(i=0; i<an; i++) {
		if(strcmp("-P", av[i])==0) {
			szClientProgramName= av[i+1];
			break;
		}
	}
	if(szClientProgramName==NULL) {
		printf("Client program name unspecified\n");
		return(1);
	}

	/*
	if(!InitializeEvent(SZKILLEVENT)) {
		DWORD dwErr= GetLastError();
		printf("Can't Initialize Event: %d\n", dwErr);
		return(1);
	}
	*/ 
	if(!InitializeServerSocket()) {
		DWORD dwErr= GetLastError();
		printf("Can't open Server Socket: %d\n", dwErr);
		return(1);
	}
	if(!SpawnChild(szClientProgramName, &oProcessInfo)) {
		DWORD dwErr= GetLastError();
		printf("Can't Spawn Process %s: %d\n", szClientProgramName, dwErr);
		return(1);
	}

	/*
	g_hAcceptThread= CreateThread(
						NULL,					// SD
						64000,					// initial stack size
						RunListen,				// thread function
						NULL,					// thread argument
						0,					// creation option
						&g_dwChildThreadId		        // thread identifier
						);

        */
	while(1) {
		oOutSock= accept(g_oSock, NULL, NULL);
		if(INVALID_SOCKET ==oOutSock) {
			dwErr= WSAGetLastError();
			printf("Invalid Socket: %d\n", dwErr);
			break;
		}
		do {
			readtag:
			iError= recv(oOutSock, rgBuf, MAXBUF-1, 0);
			if(iError==SOCKET_ERROR){
				dwErr= WSAGetLastError();
				printf("Tough on server side- Socket Error: %d\n", dwErr);
				break;
			}

			if (iError==0) {
				zcount++;
				if (zcount>20) {
					closesocket(oOutSock);
					CloseSocket();
					CloseEvent();
					return(0);
					}
				goto readtag;
				}

			p=q=rgBuf;
			tag:
			while ((*p++)!='\0') stopper++;
			stopper++;
			if(strcmp(q,"EXIT")==0) {
				printf("                  EXITING PROPERLY\n");
				CloseSocket();
				return(0);
				}
			printf("| Msg= %s  END\n", q);

/// LOOKAT  - Here is where we try to send a msg to the client
			//flipper= 1;
			if (flipper) {
				iSize= sprintf(sgBuf, "GOT the msg: %s with stopper= %d", q, stopper );
			//
			// sending on g_oSock, recieving on oOutSock
                        //printf("About to send: %s\n",sgBuf);
				client_error=  send(oOutSock, sgBuf, iSize+1,0);
				flipper=0;
                if(SOCKET_ERROR==client_error) {
                              DWORD dwErr= WSAGetLastError();
                              printf("Could not send to the client Send error %d\n", dwErr);
                               break;
                }
			}
			else
				flipper=1;

/// END of LOOKAT
			
			if (stopper<iError) {
				q=p;
				goto tag;
				}
			//printf("                  TRANSMISSION OVER stop= %d, iError= %d\n", stopper,iError);
			stopper=0;

		} while(iError!=0);   // This belongs to the do above

		closesocket(oOutSock);
		break;
	}

	CloseSocket();
	CloseEvent();
	return(0);
}


// ----------------------------------------------------------------------------------

