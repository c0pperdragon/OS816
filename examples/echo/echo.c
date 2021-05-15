// example program to access the UART for output and input

#include "os816.h"

void main()
{
    byte buffer[100];
    for (;;)
    {
        print("What is your name?\n"); 
        readline(buffer, 100);
        print("Hi, '");
        print(buffer);
        print("'!\n");
    }
}
