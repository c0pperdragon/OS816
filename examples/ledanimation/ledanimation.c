// Small test program to check the hardware and do a simple LED animation

#include <os816.h>


// fill a whole memory bank with test pattern
void fillram(unsigned int* ptr, unsigned int increment, unsigned int basevalue)
{    
    unsigned int x;
    unsigned int v = basevalue;
    for (x=0; x<32768; x++) 
    {
        ptr[x] = v;
        v+=increment;
    }
}
// compute Fletcher’s checksum for a whole memory bank of 16-bit values
unsigned long checkram(unsigned int* ptr)
{
    unsigned long sum;
    // compute two-part checksum
    unsigned int x;
    unsigned int z1 = 0;
    unsigned int z2 = 0;
    for (x=0; x<32768; x++) 
    {
        z1 += ptr[x];
        z2 += z1;
    }    
    // combine into a 32 bit value (without libraries used)
    sum = z1;
    sum = sum+sum; // 1
    sum = sum+sum; // 2
    sum = sum+sum; // 3
    sum = sum+sum; // 4
    sum = sum+sum; // 5
    sum = sum+sum; // 6
    sum = sum+sum; // 7
    sum = sum+sum; // 8
    sum = sum+sum; // 9
    sum = sum+sum; // 10
    sum = sum+sum; // 11
    sum = sum+sum; // 12
    sum = sum+sum; // 13
    sum = sum+sum; // 14
    sum = sum+sum; // 15
    sum = sum+sum; // 16
    return sum + z2;
}

// simple knight-rider style animation
const unsigned char pattern[] = 
{ 
    0x80, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 
    0x01, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0
};
void animate(void)
{
    unsigned int animpointer = 0; 
    for (;;)
    {
        portout(pattern[animpointer]);
        sleep(100);  
        animpointer=(animpointer+1)&0x0F;
    }
}
  
int main(int argc, char** argv)
{
    unsigned int ok;
    // fill RAM with test patterns
    fillram((unsigned int*)0x10000, 9,       47);
    fillram((unsigned int*)0x20000, 7,       99);
    fillram((unsigned int*)0x30000, 13,     200);
    fillram((unsigned int*)0x40000, 21,      77);
    fillram((unsigned int*)0x50000, 32074, 1235);
    fillram((unsigned int*)0x60000, 6786,   105);
    fillram((unsigned int*)0x70000, 3274,  1412);
    // show progress by turning off all LEDs
    portout(0x00);
    // test RAM content 
    ok = checkram((unsigned int*)0x10000) == 1073741824
      && checkram((unsigned int*)0x20000) == 3221258240
      && checkram((unsigned int*)0x30000) == 3221241856
      && checkram((unsigned int*)0x40000) == 1073774592
      && checkram((unsigned int*)0x50000) == 16384
      && checkram((unsigned int*)0x60000) == 49152
      && checkram((unsigned int*)0x70000) == 2147516416
      ;

    // show progress by turning on all LEDs
    portout(0xFF);
    // when everything was successfull, show animation
    if (ok) { animate(); }
    return -1;
}
