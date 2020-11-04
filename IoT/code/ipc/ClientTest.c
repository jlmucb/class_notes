//
//
// Jerrold's Client Program
//
// Calling Sequence
//		jmgServer -P Client-Program
// Initializes Server on Port Number MYPORT
//
//	For Illustrative purposes only
//	This works just fine in Linux too except that the Event mechanism must be
//	replaced by signals.
// 	Opens Socket 
// 	Opens Event called "JMGEvent1" and sends it when it is ready to quit
//
//	For Illustrative purposed only
//
//	Windows specific Calls: OpenEvent
//		also Windows socket calls
//
//	This works just fine in Linux too except that the Event mechanism must be
//	replaced by signals.
//

#include <windows.h>
#include <stdio.h>
#include <io.h>

#define SZSOCKETNAME 	"JMGSocket"
#define SZKILLEVENT		"JMGKillIt"

#define MYPORT			2400

#define MAXBUF			4096
// Compile with: gcc Servertest.c -lWs2_32


// ----------------------------------------------------------------------------------

static int			g_iParentId= 0;
static int			g_iSocketFd= 0;
static SOCKET		g_oSock;
static HANDLE		g_hKillEvent= NULL;


void MyZero(byte* pAdd, int iSize)
{
	while(iSize-->0)
		*(pAdd++)= 0;
}


void CloseEvent()
{
	if(g_hKillEvent!=NULL) {
		CloseHandle(g_hKillEvent);
		g_hKillEvent= NULL;
	}

	return;
}


BOOL InitializeEvent(LPTSTR szKillEventName)
{
	g_hKillEvent= OpenEvent(
			 EVENT_ALL_ACCESS, 	// access
			 FALSE,    			// inheritance option
			 szKillEventName		// object name
			 );
	if(g_hKillEvent==NULL)
		return(FALSE);

	return(TRUE);
}


BOOL CloseSocket()
{    
	// shutdown(g_oSock, SD_BOTH);
	closesocket(g_oSock);
	WSACleanup();
	return(TRUE);
}


BOOL InitializeClientSocket()
{
	int 	iSize= 0;
	WSADATA wsaData;
	struct	sockaddr_in oAddress;
	BOOL	fReturn= FALSE;
	char	szName[MAXBUF];
	struct hostent* poHost;
 
	// Windows specific check
	if (WSAStartup(MAKEWORD(2, 2),&wsaData)!= 0)
		goto Done;
	if (LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!= 2)
		goto Done;

	// Get endpoint
	if((g_oSock= socket(AF_INET, SOCK_STREAM, 0))==INVALID_SOCKET)
		goto Done;

	// connect
	iSize= sizeof(&oAddress);
 	MyZero((byte*)&oAddress, sizeof(oAddress));
	oAddress.sin_family= AF_INET;
	oAddress.sin_port = MYPORT;
	if(gethostname(szName, MAXBUF)>0)
		goto Done;
	if((poHost=gethostbyname(szName))==NULL)
		goto Done;
    // host address
    // oAddress.sin_addr.s_addr= *((unsigned long *)poHost->h_addr_list);
	oAddress.sin_addr.S_un.S_un_b.s_b1= 127;
	oAddress.sin_addr.S_un.S_un_b.s_b2= 0;
	oAddress.sin_addr.S_un.S_un_b.s_b3= 0;
	oAddress.sin_addr.S_un.S_un_b.s_b4= 1;
	if((connect(g_oSock,(struct sockaddr *) &oAddress, sizeof(oAddress)))!=0){
		DWORD dwErr= WSAGetLastError();
		printf("Connect error %d\n", dwErr);
		goto Done;
	}
	fReturn= TRUE;

Done:
	return(fReturn);
}


int main(int an, char** av)
{
	BOOL fSuccess= FALSE;
	int i, iError, iSize;
	char rgBuf[MAXBUF];
	DWORD dwErr= 0;

	if(!InitializeEvent(SZKILLEVENT)){
		printf("Can't open Client Socket\n");
		return(1);
	}
	if(!InitializeClientSocket()){
		dwErr= GetLastError();
		printf("Can't open Client Socket %d\n", dwErr);
		return(1);
	}

	for(i=0;i<10;i++) {
		iSize= sprintf(rgBuf, "From Client Test message %d\n", i);
		if(iSize>=4095)
			iSize= 4095;
		rgBuf[iSize]= 0;
		iError= send(g_oSock, rgBuf, iSize,0);
		if(SOCKET_ERROR==iError) {
			DWORD dwErr= WSAGetLastError();
			printf("Send error %d\n", dwErr);
			break;
			}
	}
	CloseSocket();

	SetEvent(g_hKillEvent);
	CloseEvent();
	return(0);
}



// ----------------------------------------------------------------------------------



