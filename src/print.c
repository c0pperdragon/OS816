
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
