/*
 * T. Montemarano 2014-08-01,  MIT License
 * Joystick data is x = -100 (Full Left), y=-100 (Full down), x=+100 full right, y=+100 full up.
 * The buttons are decoded as Ascii 'A' Button 1 lit, 'B' 1 grey, 'C' 2 lit, etc.  
 * Button code is sent when button is pressed.
 * Joystick zones are setup as approx +/- 15 y = 0 speed, +/- 15 x = drive straight fwd or back.
 * (y) >15 = move fwd (JS positive), move backwards (JS negative),  
 * (x) > approx 15 = add differential to y speed to turn or pivot (x positive -turn right.
 * x and y are scaled to set max speed and turning rates in the calculations of x2 and y2.  
 * cathode rbg led has red to pin 6 with 100 ohm resistor, green to pin 7 with 120 ohm resistor, cathode to gnd.
 * do: add pixy color tracking (use BT button 3) when active pixy tracking takes control 
 * and drives aBot to defined color object.
 * BT app is free on Googleplay 'joystick BT Commander".  The link on the app page leads to the details 
 * the data protocols used.
 */

#include "simpletools.h" 
#include "fdserial.h"
#include "abdrive.h"
#include "ping.h"

volatile char c1[9];
volatile int c0 = 1;
volatile int nbytes;   
volatile int joydata;
int flgping;  
int xval; 
int yval; 
int flgbt;    
int dist;
int *cogping; 
fdserial *blut; 
void getbtjoy();
void do_ping(); 

int main() {
    freqout(4, 1000, 2000); // Speaker tone: 1 s, 2 kHz
    int x2;
    int y2;
    int xy;
    int yspd = 0;
    int xspd = 0;
    flgping = 0;                //    ping cog is not running

    cog_run(getbtjoy, 220);  
    drive_ramp(0, 0);
    while(c0) {
        if(nbytes == 8) {
            pause(50);        //   try to get rid of pause
            xy = joydata;
            y2 = (xy & 511) - 200;
            x2 = (xy >> 10) - 200;
            printi("x = %d\n", x2);
            printi("y = %d\n", y2);
            yspd = (abs(y2) / 15) * 15;         // scale the fwd & rev motion
            if(y2 <0) yspd = yspd / -2;         //  go slower in reverse  
            xspd = (abs(x2) / 15) * 4;          //  scale the turning
            if(x2<0) xspd = xspd * -1;
            printi(" left right %d %d\n",yspd+xspd, yspd-xspd);
            drive_ramp(yspd+xspd, yspd-xspd);         // drive, turning differential added to yspd
        }
        if(nbytes == 3) {
            nbytes = 0;
            printi(" button = %d\n\n", c1[2]);
            if(c1[2]=='C')  {                // Button 2, on turns on ping
                cogping = cog_run(do_ping,120);  
                printi(" ping cog = %d\n\n", *cogping);
                nbytes = 0;
                flgping = 1;                      // set flgping -- ping cog is running
            }
            if(c1[2]=='D' && flgping) {              // Button 2, off turns off ping, don't try to stop if not running
                cog_end(cogping);
                nbytes = 0;
                flgping = 0;                      // clear flgping -- ping cog is stopped
            }
        }
    } // end while c0
    drive_ramp(0, 0);     // stop movement at end of program
    printi(" end \n\n");
    if(flgping) cog_end(cogping);       //  if ping running when quit is pressed, turn off
} // end main

void getbtjoy()
{
    // fdserial * fdserial_open(int rxpin, int txpin, int mode, int baudrate)
    blut = fdserial_open(9, 8, 0, 115200); 
    while(c0) { // continue until button 6 - quit - is pressed 
        nbytes = 0;
        int i = 1;
        flgbt= 1; 
        c1[1] = fdserial_rxChar(blut); // read first byte from blut
        if(c1[1] == 2) { // if STX read next 7 bytes 
            flgbt = 1;
            while(flgbt) {
                i++;
                c1[i] = fdserial_rxChar(blut); 
                if((c1[i]==3) && (i==3 || i==8)) flgbt=0;
            }       // end while flgbt, android string i = 2 => button, i = 7 => x,y values
            if(i==8) {
                nbytes = 8;
                xval = (c1[2] - 48) * 100 + (c1[3] - 48) * 10 + (c1[4] - 48);
                yval = (c1[5] - 48) * 100 + (c1[6] - 48) * 10 + (c1[7] - 48);
                joydata=(xval << 10) | yval;
            } // end if i= 8
            if(i==3) {
                nbytes = 3;
                // printi(" button = %d\n\n", c1[2]);
            }
            if((i==3) && (c1[2]=='K' || c1[2]=='L')) {    //  Button 6, ends program
                c0 = 0;
                //printi(" end \n\n");
            }
            if((i==3) && (c1[2]=='I'  || c1[2]=='J'))     {
                //E-stop on button 5 press, using JS restarts movement
                drive_ramp(0, 0);
            }
        } // end if c1=2
    } // end while c0
} // end getbtjoy 

void do_ping() {
    while(1) {
        dist = ping_cm(5);
        if(dist<=20) {          // turn on red LED
            high(6);
            low(7);
        }
        else if(dist<=36) {    // turn on yellow LED
            high(6);
            high(7);
        }
        else  {                 //  turn on green LED
            low(6);
            high(7);
        }
    pause(50);
    }   // end while
}

