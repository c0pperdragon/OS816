
#include "os816.h"

void portout(u16 data)
{
    *((byte*) 0x400000) = (byte) data; 
}

u16 portin(void)
{
    byte b = *((byte*) 0x400000);
    return (u16)b;
}
