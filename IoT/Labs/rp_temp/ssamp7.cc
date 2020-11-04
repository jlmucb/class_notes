#define BOARD RASPBERRY_PI

#include "gnublin.h"
 
int main() {
   gnublin_gpio gpio;
 
   gpio.pinMode(3,OUTPUT);
   while(1){
     gpio.digitalWrite(3,HIGH);
     sleep(2);
     gpio.digitalWrite(3,LOW);
     sleep(2);
   }
}

#define BOARD RASPBERRY_PI
 
#include "gnublin.h"
 
 
int main()
{
   gnublin_i2c i2c;
 
   i2c.setAddress(0x42); // set the address of the slave you want to read/write
 
   char buffer[8];
   char RxBuf[8];
 
   buffer[0]=0x22;
 
   i2c.send(buffer,5);
   i2c.send(0x12, buffer, 2);   //send 2 bytes from buffer to register-address 0x12
 
   i2c.receive(RxBuf, 3);       // read 3 bytes and store them in RxBuf
   i2c.receive(0x23, RxBuf, 3);  // read from  tegister-address 0x23 3 bytes and store them in RxBuf
 
}

        static void Main(string[] args)
        {
            gnublin_module_relay relay = new gnublin_module_relay();
            relay.setAddress(0x20);
            gnublin_gpio gpio = new gnublin_gpio();

            SWIGTYPE_p_std__string output = libgnublin_wrap.numberToString(libgnublin_wrap.OUT);

            gpio.pinMode(3, output);

            while (true)
            {
                for (int i = 1; i <= 8; i++)
                {
                    relay.switchPin(i, 1);
                    Thread.Sleep(100);
                }
                for (int i = 1; i <= 8; i++)
                {
                    relay.switchPin(i, 0);
                    Thread.Sleep(100);
                }


            }

