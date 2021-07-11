
#include "os816.h"

void portout(int data)
{
    *((unsigned char *) 0x400000) = (unsigned char) data; 
}

int portin(void)
{
    unsigned char b = *((unsigned char *) 0x400000);
    return (int)b;
}
