#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>


// BoolFile.cpp
//  dump sboxes into boolean function format


int iSizeofBuf= 6;
int buf[512]= { 
    5,
    0,2,3,4,1
};


/* --------------------------------------------------------------------- */


int main(int an, char** av) {
  int iOut, i;


    if(an<2 || av[1]==NULL) {
        printf("No file\n");
        return(1);
    }
  if((iOut= _creat(av[1], _S_IREAD | _S_IWRITE))<0) {
  	printf("cant creat %s\n", av[1]);
  	return(1);
  	}
  write(iOut,buf,iSizeofBuf*sizeof(int));
  close(iOut);
    printf("Dumping %d integers to %s\n", iSizeofBuf, av[1]);
    for(i=0; i<iSizeofBuf;i++) {
        printf("%03d ",buf[i]);
    }
    printf("\n");
  return(0);
}


/* --------------------------------------------------------------------- */
