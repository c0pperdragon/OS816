// short program for the OS816 environment
// simple LED animation in C

#include "os816.h"

const byte pattern[] =   // constant data (KDATA)
{ 
    0x80, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 
    0x01, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0
};

// static data with initialization (DATA)
word speed = 100; 
word animpointer = 8;   

// UDATA segment (should initially be zeroed out)
byte zeroinit;  

void portout_indirect(byte* p)
{
    portclear((byte)0xff);
    portset(*p);
}

void main()
{
    while(!zeroinit)
    {
        byte p = portin();
        animpointer = (animpointer+1) % 16;
        p = p & pattern[animpointer];
        portout_indirect(&p);
        sleep(speed);  
    }
}
