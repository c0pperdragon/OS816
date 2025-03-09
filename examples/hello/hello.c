// example program to do some writing to the serial port
#include <string.h>
#include <os816.h>

int main(int argc, char** argv)
{
    int i;
	sendstr("Hi Bernd!\r\n");
    for (i=0; i<5; i++)
    {
        sendstr("Mist!\r\n"); 
        sleep(1000);
    }
    return 0;
}
