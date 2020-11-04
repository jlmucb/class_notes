#include <iostream>
#include <wiringPi.h>
#include <wiringPiI2C.h>

using namespace std;

int readInput(int fd, int reg) {
    wiringPiI2CReadReg8(fd, reg);
    return wiringPiI2CReadReg8(fd, reg);
}

int main() {
    wiringPiSetupGpio();
    int dacModule = wiringPiI2CSetup(0x48);
    if (dacModule < 0) {
        cout << "I2C Setup Error" << endl;
        return 0;
    }

    int i;
    int A[4] = {0, 0, 0, 0};
    int A_Reg[4] = {0x40, 0x41, 0x42,0x43};

    while (1) {
        for (i=0;i<4;i++) A[i] = readInput(dacModule, A_Reg[i]);
        wiringPiI2CWriteReg8(dacModule, 0x40, (A[0]+A[1]+A[2]+A[3])/4);
        cout << endl;
        for (i=0;i<4;i++) cout << i << " : " << A[i] << endl;
        delay(500);
    }
    return 0;
}
