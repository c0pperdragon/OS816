
#include "os816.h"

void print(char* text)
{
    for (;;)
    {
        char c = *text;
        if (c=='\0') { return; }
        send ((byte)c);
        text++;
    }    
}
