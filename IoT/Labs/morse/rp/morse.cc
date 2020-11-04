//  Manferdelli
//    Raspberry Pi morse

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringSerial.h>
#include <wiringPiI2C.h>
#include <wiringShift.h>
#include <softPwm.h>
#include <softTone.h>

#ifndef byte
typedef unsigned char byte;
#endif

// /dev/ttyAMO is the Tx/Rx pins for raspberry pi.
// Mini uart is /dev/ttyS0

#define RxTxDevice "/dev/ttyAM0"

void clear(byte* buf, int n) {
  for (int i = 0; i < n; i++) {
    buf[i] = 0;
  }
}

void printBuf(byte* in, int n) {
  for (int i = 0; i < n; i++) {
    printf(" %02x", in[i]);
    if ((i % 16) == 15 {
      printf("\n");
    }
  }
  printf("\n");
}

const int base = 120;
const int i2c_address = 0x48;
const int outPin = 1;

//
//  Connection scheme
//

int main(int an, char** av) {

  if (wiringPiSetup() < )) {
    printf("Can't initialize Wiring Pi\n");
    return 1;
  }
  int fd = wiringPiSPISetup(chip_select, bus_speed);
  if (fd < 0) {
    printf("Can't initialize SPI interface\n");
    return 1;
  }
  pcf8591Setup(base, i2c_address);
  pinMode(outPin, INPUT);
  delay(10);

  // wiringPiSetup();
  // analogRead(base);
  // pwmWrite(outPin, v * 5);
  // int fd = wiringPiI2CSetup(0x60);
  // pcf8574Setup(100,  0x27);
  // wiringPiI2CWrite(fd, int);
  // the following requires root access
  // int wiringPiSetup(void);
  // int wiringPiSetupGpio(void);
  // int wiringPiSetupPhys(void);
  // int wiringPiSetupSys(void);
  // void pinMode (int pin, int mode);
  // void pullUpDnControl (int pin, int pud);
  // void digitalWrite (int pin, int value);
  // void pwmWrite (int pin, int value);
  //    The Raspberry Pi has one on-board PWM pin, pin 1 
  //    (BMC_GPIO 18, Phys 12) and the range is 0-1024. 
  //    Other PWM devices may have other PWM ranges.
  // int digitalRead (int pin);
  // analogRead (int pin);
  // analogWrite (int pin, int value);
  // unsigned int millis (void);
  // unsigned int micros (void);
  // void delay (unsigned int howLong);
  // void delayMicroseconds (unsigned int howLong);
  // int wiringPiISR (int pin, int edgeType,  void (*function)(void));
  //    This function registers a function to received interrupts on the specified pin. 
  //    The edgeType parameter is either INT_EDGE_FALLING, INT_EDGE_RISING, INT_EDGE_BOTH 
  //    or INT_EDGE_SETUP. If it is INT_EDGE_SETUP then no initialisation of the pin will 
  //    happen – it’s assumed that you have already setup the pin elsewhere (e.g. with the 
  //    gpio program), but if you specify one of the other types, then the pin will be 
  //    exported and initialised as specified. This is accomplished via a suitable call to 
  //    the gpio utility program, so it need to be available.
  // int piThreadCreate (name);
  // piLock (int keyNum);
  // piUnlock (int keyNum);
  // int serialOpen (char *device, int baud);
  // void serialClose (int fd);
  // void  serialPuts (int fd, char *s);
  // void  serialPrintf (int fd, char *message, …);
  // int   serialDataAvail (int fd);
  // int serialGetchar (int fd);
  // void serialFlush (int fd);
  // int wiringPiSPISetup (int channel, int speed);
  // int wiringPiSPIDataRW (int channel, unsigned char *data, int len);
  // int wiringPiI2CSetup (int devId);
  // int wiringPiI2CRead (int fd);
  // int wiringPiI2CWrite (int fd, int data);
  // int wiringPiI2CWriteReg8 (int fd, int reg, int data);
  // int wiringPiI2CWriteReg16 (int fd, int reg, int data);
  // int wiringPiI2CReadReg8 (int fd, int reg);
  // int wiringPiI2CReadReg16 (int fd, int reg);
  // uint8_t shiftIn (uint8_t dPin, uint8_t cPin, uint8_t order);
  //    This shifts an 8-bit data value in with the data appearing on the 
  //    dPin and the clock being sent out on the cPin. Order is either 
  //    LSBFIRST or MSBFIRST. The data is sampled after the cPin goes high. 
  //    (So cPin high, sample data, cPin low, repeat for 8 bits) The 8-bit 
  //    value is returned by the function.
  // void shiftOut (uint8_t dPin, uint8_t cPin, uint8_t order, uint8_t val);
  //    The shifts an 8-bit data value val out with the data being sent 
  //    out on dPin and the clock being sent out on the cPin. order is as 
  //    above. Data is clocked out on the rising or falling edge – ie. dPin 
  //    is set, then cPin is taken high then low – repeated for the 8 bits.
  // int softPwmCreate (int pin, int initialValue, int pwmRange);
  // void softPwmWrite (int pin, int value);
  // int softToneCreate (int pin);
  // void softToneWrite (int pin, int freq);
  return 0;
}
