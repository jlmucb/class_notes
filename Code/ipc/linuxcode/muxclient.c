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


int main(int an, char** av)

{
	int 	i;
	int iSockfd= 0;
	FILE*   stream= NULL;
	char 	rgszBuf[SIZEBUF];
	int 	iPos=	0;
	int	iSize= 0;
	struct  sockaddr_un     oSockOut;


	// Server Name
	g_iParentPid= getpid();
	if(an<2 || av[1]==NULL) {
	       g_strServerName= "Jerrold";
	}
	else {
	       g_strServerName= av[1];
	}

	if((iSockfd= socket(AF_LOCAL, SOCK_STREAM, 0))<0) {
		printf("Cant create socket\n");
		return(1);
	}

 	MyZero((char*)&oSockOut, sizeof(oSockOut));
	strcpy(oSockOut.sun_path, "/tmp/");
	strcat(oSockOut.sun_path, g_strServerName);
	iSize= SUN_LEN(&oSockOut);

	if(connect(iSockfd, (struct sockaddr *)&oSockOut, iSize)<0) {
		printf("Cant Connect\n");
		return(1);
	}

 	stream= fdopen(iSockfd, "r");
	for(;;) {
 		i= fgetc(stdin);     
		if(iPos>(SIZEBUF-2)) {
 			rgszBuf[iPos++]= 0;
			fprintf(stream, "%s", rgszBuf);
			iPos= 0;
		}
		if(i==(int)'\n') {
			rgszBuf[iPos++]= '\n';
 			rgszBuf[iPos++]= 0;
			fprintf(stream, "%s", rgszBuf);
			iPos= 0;
		}
		if(i==EOF) {
 			rgszBuf[iPos++]= 0;
			fprintf(stream, "%s", rgszBuf);
			return(0);
		}
	}

Done:
	return(0);
}


// -------------------------------------------------------------------------------------


