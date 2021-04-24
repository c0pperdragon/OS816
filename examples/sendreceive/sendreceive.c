// example program to access the UART for output and input

#include "os816.h"

void send(byte* data, word length)
{
    word i;
    for (i=0; i<length; i++) { uartsend(data[i]); }
}

word receiveline(byte* buffer, word maxlength)
{
    word len = 0;
    while (len<maxlength)
    {
        byte x = uartreceive();
        if (x==13 || x==10) { break; }
        buffer[len] = x;
        len++;
    }
    return len;
}

void main()
{
    byte buffer[100];
    word len;

    for (;;)
    {
        send((byte*)"What is your name?\n", 19); 
        len = receiveline(buffer, 100);
        send((byte*) "Hello, '", 8);
        send(buffer, len);
        send((byte*) "'!\n", 3);
    }
}
