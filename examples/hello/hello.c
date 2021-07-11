// example program to do some writing to the serial port

#include <stdio.h>
#include <os816.h>


int main(int argc, char** argv)
{
    int i;
    for (i=0; i<5; i++)
    {
        puts("Mist!"); 
        sleep(1000);
    }
}
