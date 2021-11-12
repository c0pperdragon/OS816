#include <os816.h>

int main(int argc, char** argv)
{
    sendstr("Remote echo demo, try to type keys...\r\n");
    for (;;)
    {
        int b;
        b = receive();
        if (b>=0) 
        {   
            // show low bits on 6 spare LEDs
            portout( ((b & 0x3F) << 2) | 0x03); 
            // send back received byte
            send(b); 
        }
    }
}
