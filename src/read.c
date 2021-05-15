
#include "os816.h"

void readline(char* buffer, u16 bsize)
{
    u16 len=0;
    while (len+1<bsize)
    {
        u16 c = receive();
        if (c==10 || c==13) break; // terminate line with either CR or LF
        if (c<32) continue;        // ignore other control characters
        buffer[len] = (byte)c;
        len++;
    }
    buffer[len] = '\0';
}
