// test behaviour of integer manipulation and 
// indexed addressing across bank boundaries
//
// Test results:  
// - 32 bit integers are supported
// - array indizes are truncated to 64K bytes range
// - arrays crossing page boundaries can be accessed
// - pointer arithmethic of the C compiler seems inconsistent (16-bit only?)

#include "os816.h"


void putbyte(byte* buffer, u32 pos, byte value)
{
    buffer[pos] = value;
}

void prepare(byte* buffer)
{
    putbyte(buffer,0L,(byte)'M');
    putbyte(buffer,1L,(byte)'i');
    putbyte(buffer,2L,(byte)'s');
    putbyte(buffer,3L,(byte)'t');
    putbyte(buffer,4L,(byte)'\0');
}

u16 subl(long int a, long int b)
{
    return (u16) (a-b);
}

void printindirect(u16 *p)
{
    printu16(*p);
}

void main()
{
    // the bank we intend to work in
    byte* buffer = (byte*) 0x20000; 
    // extra banks
    byte* extension = (byte*) 0x30000;
    byte* extension2 = (byte*) 0x40000;

    extension[0] = 64; 
    extension[1] = 0; 
    prepare(buffer+0xFFFE), 
    
    extension[0xFFFF] = 47;
    extension2[0] = 11;
    
    print("\n");
    printu16(subl(206544421, 206500000));
    print("\n");
    
    for (;;)
    {
        printu16(buffer[0]);
        print(" ");
        printu16(buffer[1]);
        print(" ");
        printu16(buffer[0xFFFE]);
        print(" ");
        printu16(buffer[0xFFFF]);
        print(" ");
        printu16(extension[0]);
        print(" ");
        printu16(extension[1]);
        print(" ");

        print(buffer+0xFFFE);
        print(" ");
        
        // try to access a 16-bit word accross bank boundaries
        printindirect((u16*) (extension+0xFFFF)); // writes 2863

        print("\n");
        sleep(1000);
    }
}

