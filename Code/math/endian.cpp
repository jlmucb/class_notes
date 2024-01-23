#include  <stdio.h>

int main(int an,char *av[]) {
    unsigned char*  pb;
    unsigned        u= 0;

    printf("Endianness, 1 is in low order address\n");

    pb= (unsigned char*) &u;
    *(pb++)= 1;
    *(pb++)= 2;
    *(pb++)= 3;

    printf("u: %08x\n",u);
    return 0;
}


