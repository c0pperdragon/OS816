// example program to access the UART for output and input

#include "os816.h"

void main()
{
    byte buffer[50];
    for (;;)
    {
        print("What is your name?\n"); 
        input(buffer, 50);
        print("Hi, '");
        print(buffer);
        print("'!\n");
    }
}
