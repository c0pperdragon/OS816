
#include "os816.h"

void readline(char* buffer, u16 bsize)
{
    u16 len=0;
    while (len+1<bsize)
    {
        u16 c = receive();
        if (c=='\n') break;  // terminate line with '\n'
        if (c<' ') continue;  // ignore other control characters
        buffer[len] = (byte)c;
        len++;
    }
    buffer[len] = '\0';
}
