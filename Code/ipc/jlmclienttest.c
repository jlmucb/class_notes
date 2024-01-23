/*
      gcc jlmclienttest.c -lWs2_32 -o c:\nl\bin\client.exe
      server.exe -P client.exe

*/
//
// Jerrold's Client Program
//
// Calling Sequence
//    jmgServer -P Client-Program
// Initializes Server on Port Number MYPORT
//
//  For Illustrative purposes only
//  This works just fine in Linux too except that the Event mechanism must be
//  replaced by signals.
//   Opens Socket
//   Opens Event called "JMGEvent1" and sends it when it is ready to quit
//
//  For Illustrative purposed only
//
//  Windows specific Calls: OpenEvent
//    also Windows socket calls
//
//  This works just fine in Linux too except that the Event mechanism must be
//  replaced by signals.
//

#include <windows.h>
#include <stdio.h>
#include <io.h>

#define SZSOCKETNAME   "JMGSocket"
//#define SZKILLEVENT    "JMGKillIt"

#define MYPORT      2400

#define MAXBUF      16384
// Compile with: gcc Servertest.c -lWs2_32


// ----------------------------------------------------------------------------------

static int      g_iParentId= 0;
static int      g_iSocketFd= 0;
static SOCKET    g_oSock;
static HANDLE    g_hKillEvent= NULL;


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
       EVENT_ALL_ACCESS,   // access
       FALSE,          // inheritance option
       szKillEventName    // object name
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
  int   iSize= 0;
  WSADATA wsaData;
  struct  sockaddr_in oAddress;
  BOOL  fReturn= FALSE;
  char  szName[MAXBUF];
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
  //  Cant do this...
    //if(listen(g_oSock, 5)!=0)
                      //goto Done;



  fReturn= TRUE;

Done:
  return(fReturn);
}


int main(int an, char** av)
{
  BOOL fSuccess= FALSE;
  int i, iError, iSize, read_error;
  char rgBuf[MAXBUF];
  DWORD dwErr= 0;
  SOCKET     oOutSock;
  FILE *f1;

  /*
  if(!InitializeEvent(SZKILLEVENT)){
    printf("Can't open Client Socket\n");
    return(1);
  }
  */
  if(!InitializeClientSocket()){
    dwErr= GetLastError();
    printf("Can't open Client Socket %d\n", dwErr);
    return(1);
  }

#if 0
  // putting this in causes an error on the 
  // read in jlmservertest.c
  //oOutSock= accept(g_oSock, NULL, NULL);
  //oOutSock= select(g_oSock, NULL, NULL);

  
    if(INVALID_SOCKET==oOutSock) {
    dwErr= WSAGetLastError();
    printf("Invalid Socket: %d\n", dwErr);
    return(1);
    }
#endif

  for(i=0;i<50;i++) {
    iSize= sprintf(rgBuf, "MSG %d", i);
    // Upon successful return, these functions return the number of characters
    // printed (not including the trailing’'\0'’used  to  end  output to strings). 
    if(iSize>=MAXBUF)
      iSize= MAXBUF-1;
    iError= send(g_oSock, rgBuf, iSize+1,0);
    if(SOCKET_ERROR==iError) {
      DWORD dwErr= WSAGetLastError();
      printf("Send error %d\n", dwErr);
      break;
      }

    /*
    LOOKAT
    1. can't recv on g_oSock cause it hangs
    2. can't recv on oOutSock cause we when we init it above
    we get an error from the server,
        //printf("nnnnnnnnnnnnnnnnnnnnnnnnn\n");
        //read_error= read(g_oSock, rgBuf, MAXBUF-1);
        //
        //  read_error= recv(g_oSock, rgBuf, MAXBUF-1, 0);  // TRY1
        //printf("xxxxxxxxxxxxxxxxxxxxxxxxx\n");
    */ 
    read_error= recv(g_oSock, rgBuf, MAXBUF-1, 0);  // TRY2
    printf("READ_ERROR %d\n",read_error);
    if(read_error==SOCKET_ERROR){
      dwErr= WSAGetLastError();
      fprintf(f1,"jlmclienttest.c Very Tough - Socket Error: %d\n", dwErr) ;
      break;
    }
    if (read_error!=0)
      printf("CLIENT_GOT %s\n",rgBuf);
    }

 
  iSize= sprintf(rgBuf, "EXIT");
  iError= send(g_oSock, rgBuf, iSize+1,0);
  CloseSocket();

  //SetEvent(g_hKillEvent);
  //CloseEvent();
  return(0);
}


// ----------------------------------------------------------------------------------




