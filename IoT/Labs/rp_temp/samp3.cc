#define BOARD RASPBERRY_PI
#include "gnublin.h"
 
 
int main() {
   gnublin_i2c i2c;
 
 
   char buffer[8];
   char RxBuf[8];
   buffer[0]= 0x22;
 
   i2c.setAddress(0x42); // set the address of the slave you want to read/write
   i2c.send(buffer,5);
   i2c.send(0x12, buffer, 2);   //send 2 bytes from buffer to register-address 0x12
   i2c.receive(RxBuf, 3);       // read 3 bytes and store them in RxBuf
   i2c.receive(0x23, RxBuf, 3);  // read from  tegister-address 0x23 3 bytes and store them in RxBuf
}

