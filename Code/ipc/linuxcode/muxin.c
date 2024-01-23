#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int 	g_iSocServerPid= 0;
int 	g_iParentPid= 0;
char*	g_strServerName= NULL;


#define SIZEBUF 1024
#define MUXPORT 5000


// -------------------------------------------------------------------------------------


void MyZero(char* pAdd, int iSize)
{
	while(iSize-->0)
		*(pAdd++)= 0;
}


int ProcessOpenConnection(int iClientSock, int iParentPid)
{
	int 	i;
	FILE*   stream= NULL;
	char 	rgszBuf[SIZEBUF];
	int 	iPos=	0;

 	stream= fdopen(iClientSock, "r");
	for(;;) {
 		i= fgetc(stream);     
		if(iPos>(SIZEBUF-2)) {
 			rgszBuf[iPos++]= 0;
			fprintf(stdout, "%s", rgszBuf);
			iPos= 0;
		}
		if(i==(int)'\n') {
			rgszBuf[iPos++]= '\n';
 			rgszBuf[iPos++]= 0;
			fprintf(stdout, "%s", rgszBuf);
			iPos= 0;
		}
		if(i==EOF) {
 			rgszBuf[iPos++]= 0;
			fprintf(stdout, "%s", rgszBuf);
			return(0);
		}
	}
	return(0);
}


int RunServer(void)
{
	int 	iSockfd= 0;
	struct  sockaddr_un     oSockIn;
	socklen_t osSize;

	int 	iAcceptedSockfd= 0;
	int 	iConnectedPid= 0;
	struct	sockaddr_un	oSockNewClient;
	int 	iSize= 0;

	if(g_strServerName==NULL)
		return(-1);

	if((iSockfd= socket(AF_LOCAL, SOCK_STREAM, 0))<0)
		return(-1);

 	MyZero((char*)&oSockIn, sizeof(oSockIn));
	strcpy(oSockIn.sun_path, "/tmp/");
	strcat(oSockIn.sun_path, g_strServerName);
	iSize= SUN_LEN(&oSockIn);

	if(bind(iSockfd, (struct sockaddr *)&oSockIn, iSize) < 0) {
		close(iSockfd);
		return(-1);
	}

 	MyZero((char*)&oSockNewClient, sizeof(oSockNewClient));
	listen(iSockfd, 5);
	// iAcceptedSockfd, iConnectedPid, oSockNewClient;
	for(;;) {
		iAcceptedSockfd= accept(iSockfd, 
				        (struct sockaddr *)&oSockNewClient, &osSize);
		if(iAcceptedSockfd<0)
			continue;
		if((iConnectedPid=fork())>0)
			ProcessOpenConnection(iAcceptedSockfd, getpid());
	}

	close(iSockfd);
	return(0);
}


int main(int an, char** av)

{
	int 	i;
	char 	rgszBuf[SIZEBUF];
	int	iError= 1;

	// Server Name
	g_iParentPid= getpid();
	if(an<2 || av[1]==NULL) {
	       g_strServerName= "Jerrold";
	}
	else {
	       g_strServerName= av[1];
	}
	fprintf(stderr, "%s\n", g_strServerName);

	// fork off Server that accepts connections
	if((g_iSocServerPid=fork())<0) {
		 fprintf(stderr, ";Cant start Socket Server Process\n");
		 goto Done;
	}
	if(g_iSocServerPid>0) {
		// Server Process
		if(RunServer()<0) {
		 	fprintf(stderr, ";Cant start Socket Server Process\n");
		 	goto Done;
	 	}
		iError= 0;
		goto Done;
	}

	for(;;) {
		i= read(0, rgszBuf, SIZEBUF-1);
	  	rgszBuf[i]= 0;
		printf("Echo: %s\n", rgszBuf);
		}

Done:
	if(getpid()==g_iParentPid) {
		// kill the Server
		// kill(g_iSocServerPid);
	}
	return(0);
}


// -------------------------------------------------------------------------------------


