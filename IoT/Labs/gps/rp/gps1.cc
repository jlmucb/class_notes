// Manferdelli, GPS
// This used the standard serial interface via /dev/ttyS0
// You'll need to use an fdmi connector


//  GPS Interfacing with Raspberry Pi using C (WiringPi Library)
//    vcc to 1
//    gnd to 6
//    tx to 5
//    Raspberry Pi      GPS
//    5V  (GPIO Pin  2) VCC
//    GND (GPIO Pin  6) GND
//    RXD (GPIO Pin 10) TXD

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

int main () {
  int serial_port; 
  char dat,buff[100],GGA_code[3];
  unsigned char IsitGGAstring=0;
  unsigned char GGA_index=0;
  unsigned char is_GGA_received_completely = 0;
  
  if ((serial_port = serialOpen ("/dev/ttyS0", 9600)) < 0) {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

  if (wiringPiSetup () == -1) {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }

  while(1) {
    if(serialDataAvail (serial_port) ) { 
      dat = serialGetchar(serial_port);    /* receive character serially */    
      if(dat == '$'){
        IsitGGAstring = 0;
        GGA_index = 0;
      }
      else if(IsitGGAstring ==1){
        buff[GGA_index++] = dat;
        if(dat=='\r')
          is_GGA_received_completely = 1;
        }
      else if(GGA_code[0]=='G' && GGA_code[1]=='G' && GGA_code[2]=='A'){
        IsitGGAstring = 1;
        GGA_code[0]= 0; 
        GGA_code[0]= 0;
        GGA_code[0]= 0;    
        }
      else{
        GGA_code[0] = GGA_code[1];
        GGA_code[1] = GGA_code[2];
        GGA_code[2] = dat;
        }
      }
    if(is_GGA_received_completely==1){
      printf("GGA: %s",buff);
      is_GGA_received_completely = 0;
    }
  }
  return 0;
}

