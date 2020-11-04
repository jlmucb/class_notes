#include <stdio.h>


typedef unsigned char byte;


/* --------------------------------------------------------------------- */


/*
 *  S(t,u,v,w)= (t+tw, u+uv, v+uvw, w+tw).
 */

byte Sb[16];


byte S(byte in)
{
    byte w= in&0x01;
    byte v= (in>>1)&0x01;
    byte u= (in>>2)&0x01;
    byte t= (in>>3)&0x01;
    byte out= 0;

    out|= (t^(t&w))<<3;
    out|= (u^(u&v))<<2;
    out|= (v^(u&v&w))<<1;
    out|= w^(t&w);
    return out; 
}


int diff(byte x, byte y)
{
    int     t= 0;
    byte    i;

    for(i=0;i<16;i++) {
        if((Sb[i]^Sb[i^x])==y)
            t++;
    }
    return t;
}


main(int an, char *av[])

{
        int	t;
        byte    x, y;

        for(x=0;x<16;x++)
            Sb[x]= S(x);

        printf("SBox\n");
        printf("x: ");
        for(x=0; x<16; x++) 
            printf(" %02x ", x);
        printf("\n");
        printf("   ");
        for(x=0; x<16; x++) 
            printf(" %02x ", Sb[x]);
        printf("\n");

        printf("\n\nDifferences\n");
        printf("y': ");
        for(y=0; y<16; y++) 
            printf(" %02x ", y);
        printf("\n");
        for(x=0;x<16;x++) {
            t= 0;
            printf("    ");
            for(y=0;y<16;y++) {
                t= diff(x,y);
                printf(" %02d ", (int) t);
            }
            printf("  x'= %02x\n", x);
        }

        printf("\n\ndone\n\n");
        return 0;
}


/* ----------------------------------------------------------------------- */

