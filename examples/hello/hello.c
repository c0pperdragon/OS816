// example program to do some writing to the serial port

#include <stdio.h>
#include <os816.h>


int main(int argc, char** argv)
{
    int i;
    for (i=0; i<5; i++)
    {
        printf("Mist!\n"); 
        sleep(1000);
    }
    return 0;
}
