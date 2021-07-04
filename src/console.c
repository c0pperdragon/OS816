
#include "os816.h"

void print(char* text)
{
    u16 i;    
    for (i=0;;i++)
    {
        char c = text[i];
        if (c=='\0') { return; }
        send (c);
    }    
}

const u16 digitvalues[4] = { 10000, 1000, 100, 10 };
void printu16(u16 value)
{
    bool haveleading = false;
    u16 pos;
    
    for (pos=0; pos<4; pos++)
    {
        u16 digitvalue = digitvalues[pos];
        u16 digit = 0;
        while (value>=digitvalue)
        {
            value -= digitvalue;
            digit++;
        }
        if ((digit>0) || haveleading)
        {
            haveleading = true;
            send ('0' + digit);
        }
    }
    send ('0' + value);    
}

void input(char* buffer, u16 bsize)
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

