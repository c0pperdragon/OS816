#include <os816.h>

int main(int argc, char** argv)
{
    sendstr("Remote echo demo, try to type keys...\n");
    for (;;)
    {
        int b;
        b = receive();
        if (b>=0) { send(b); }
    }
}
