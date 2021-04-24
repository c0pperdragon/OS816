// example program to access the UART for output

#include "os816.h"

void send(byte* data, word length)
{
    word i;
    for (i=0; i<length; i++) { uartsend(data[i]); }
}

void main()
{
    for (;;)
    {
        send((byte*)"Hello world!\n", 13); 
        sleep(1000);
    }
}
