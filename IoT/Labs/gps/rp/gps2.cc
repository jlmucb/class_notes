#include <stdio.h>
#include <errno.h>
#include <wiringPi.h>
#include <string.h>
#include <wiringSerial.h>

int main() {
  char gps[65];   //gps string array
  int fd, flag;    //handler
  char arr[]="$GPGGA";
  printf("raspberry gps programme");
  if(wiringPiSetup()<0)     //setup wiring pi
    return -1;
  else
    printf("setup is ok\n");
  while(1) {
    int i=0;
    if((fd=serialOpen("/dev/ttyAMA0",9600))<0) {
      fprintf(stderr,"unable to open serial device%s\n",strerror(errno));
    } else {
      printf("serial UART is Ok\n");
    }
    if(serialGetchar(fd)==13) {
      for(i=0;i<6;i++) {
        if(serialGetchar(fd)==arr[i])
          flag++;
    }
  }
  if(flag==5) {
    for(i=0;i<=65;i++)
      gps[i]=serialGetchar(fd);
  }
  serialClose(fd);
  flag=0;
  printf("%s",gps);
  }
  fflush(fd);
}

