
#include "os816.h"


u16 tokenize(char* buffer, char** tokens, u16 maxtokens)
{
    u16 numtokens=0;
    bool intoken=false;
    u16 i;
    
    for (i=0; i<65535; i++)
    {
        u16 c = (u8) buffer[i];
        if (c==0) { break; }
        if (c<=32) 
        { 
            buffer[i] = 0; 
            intoken = false;
        }
        else if (!intoken)
        {
            intoken = true;
            if (numtokens<maxtokens)
            {
                tokens[numtokens] = &(buffer[i]);
                numtokens++;
            }
        }    
    }            
    return numtokens;
}

bool strequal(char* s1, char* s2)
{
    u16 i;
    for (i=0; i<65535; i++)
    {
        if (s1[i] != s2[i]) { return false; }
        if (s1[i]==0) { return true; }
    }
    return false;
}

