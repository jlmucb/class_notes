//
// Program B
// 	Opens Socket 
// 	Responds to event called "JMGEvent1"
// 	Reads socket until termination
// 	Terminates
//
//	For Illustrative purposed only
//
//	Windows specific Calls: OpenEvent, close
//		also Windows socket calls
//
//	This works just fine in Linux too except that the Event mechanism must be
//	replaced by signals.
//

#include <windows.h>
#include <stdio.h>
#include <io.h>


#define SZEVENTNAME 	"JMGSignalEvent1"
#define SZKILLEVENT		"JMGKillIt"
#define SZSOCKETNAME 	"JMGSocket1"


// ----------------------------------------------------------------------------------


static HANDLE		g_hSignalEvent= NULL;
static HANDLE		g_hKillEvent= NULL;
static int			g_iParentId= 0;


void MyZero(byte* pAdd, int iSize)
{
	while(iSize-->0)
		*(pAdd++)= 0;
}



BOOL WaitforEvent(LPTSTR szSignalEventName, LPTSTR szKillEventName)
{
	DWORD	dwWait= 0;
	BOOL	fReturn= FALSE;
	HANDLE	rghHandles[2];
	int		iError= 0;

	g_hSignalEvent= OpenEvent(
			 EVENT_ALL_ACCESS, 	// access
			 FALSE,    			// inheritance option
			 szSignalEventName		// object name
			 );
	if(g_hSignalEvent==NULL)
		goto Done;

	g_hKillEvent= OpenEvent(
			 EVENT_ALL_ACCESS, 	// access
			 FALSE,    			// inheritance option
			 szKillEventName		// object name
			 );
	if(g_hKillEvent==NULL)
		goto Done;


	fReturn= TRUE;
	rghHandles[0]= g_hSignalEvent;
	rghHandles[1]= g_hKillEvent;
	for(;;) {
		dwWait= WaitForMultipleObjects(2, rghHandles, FALSE, 200);
		if(dwWait!=WAIT_TIMEOUT) {
			dwWait-= WAIT_OBJECT_0;
			if(dwWait==0) {
				ResetEvent(g_hSignalEvent);
				printf("Got Signal Event\n");
			}
			else {
				printf("Got Kill Event\n");
				break;
			}
		}
	}

Done:
	if(g_hSignalEvent!=NULL)
		CloseHandle(g_hSignalEvent);
	g_hSignalEvent= NULL;
	if(g_hKillEvent!=NULL)
		CloseHandle(g_hKillEvent);
	g_hKillEvent= NULL;

	return(TRUE);
}


/*

// Note to Jerrold:

// This is the code to start another thread that runs a function "RunAccept"
// You might use this to create a thread that waits for messages or signals while the
// main thread does other processing

static HANDLE g_hAcceptThread= NULL;
DWORD  g_dwThreadId= 0;
g_hAcceptThread= CreateThread(
					NULL,				// SD
					64000,				// initial stack size
					RunAccept,			// thread function
					NULL,				// thread argument
					0,					// creation option
					&g_dwThreadId		// thread identifier
					);
*/

#define TEST
#ifdef TEST

// TestLoop

int main(int an, char** av)
{
	BOOL fSuccess= FALSE;

	printf("ProgB\n");

	fSuccess= WaitforEvent(SZEVENTNAME, SZKILLEVENT);
	if(fSuccess)
		printf("Child: Event Succeeded\n");
	else
		printf("Child: Event Failed\n");

	return(0);
}

#endif

// ----------------------------------------------------------------------------------



