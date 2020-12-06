#include <stdio.h>

// hexdump.c
//  dump sboxes into boolean function format


const int NBUF= 2048;
int g_iNumBufChars= 0;

char rgBuf[2048];


/* --------------------------------------------------------------------- */


int main(int argn, char** argv)

{
  int in;
  int iNumLine= 0;
  int iInLine=16;
  int iByte;
  unsigned char* p;

  if(argn<2) {
    printf("no file specified");
    exit(1);
    }

  if((in=open(argv[1],0))<=0) {
    printf("cant open: %s\n", argv[1]);
    exit(1);
    }

  printf("File: %s\n\n", argv[1]);

  for(;;) {
    if(g_iNumBufChars<=0) {
      if((g_iNumBufChars=read(in,rgBuf,NBUF))<=0) 
        break;
      p= (unsigned char*)rgBuf;
      }
    if(iInLine>=16) {
      printf("\n%04x  ", iNumLine*16);
      iNumLine++;
      iInLine= 0;
      }
    iByte= *(p++);
    g_iNumBufChars--;
    iInLine++;
    printf("%02x ", iByte);
    }

  printf("\n\n");

  close(in);
  exit();
}


/* --------------------------------------------------------------------- */
