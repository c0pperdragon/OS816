// short program for the OS816 environment
// simple LED animation in C

#include "os816.h"

const byte pattern[] =   // constant data (KDATA)
{ 
    0x80, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 
    0x01, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0
};

u16 animpointer;  // uninitialized variable data (UDATA)
    
void main()
{

    for (;;)
    {
        u16 i = portin();
        i = i & pattern[animpointer];
        portout (i);
        sleep(100);  
        
        animpointer=(animpointer+1)&0x0F;
    }
}
