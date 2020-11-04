#include <wiringPi.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pcf8591.h>

#define PINBASE 100

/*
cc -o test2 test2.c -lwiringPi
*/

int main (void)
{
  int i;
  int a2dChannel = 0; // analog channel
  int a2dVal;
  float a2dVol;
  float Vref = 3.3;

  if (pcf8591Setup(PINBASE, 0x48) < 0) {
    printf("pcf8591Setup failed:\n");
  }

  for(i=0;i<20;i++) {
    a2dVal = analogRead(PINBASE + a2dChannel);
    a2dVol = a2dVal * Vref / 255;
    printf("a2dVal=%d\n",a2dVal);
    printf("a2dVol=%f[V]\n",a2dVol);
    sleep(1);
  }
}


I wrote the following code for PCF8591 using a C language. This cord work fine.

#include <wiringPi.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

/*
cc -o test1 test1.c -lwiringPi
*/

int main (void)
{
  int fd;
  int i;
  int a2dChannel = 0; // analog channel
  int a2dVal;
  float a2dVol;
  float Vref = 3.3;

  if ((fd = wiringPiI2CSetup(0x48)) < 0) {
    printf("wiringPiI2CSetup failed:\n");
  }

  for(i=0;i<20;i++) {
    wiringPiI2CWrite(fd,0x40 | a2dChannel);
    a2dVal = wiringPiI2CRead(fd); // Previously byte
    a2dVal = wiringPiI2CRead(fd);
    a2dVol = a2dVal * Vref / 255;
    printf("a2dVal=%d\n",a2dVal);
    printf("a2dVol=%f[V]\n",a2dVol);
    sleep(1);
  }
}

