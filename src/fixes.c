#include <string.h>
#include <stdlib.h>
#include <stdio.h>


// Replace broken implementation in the WDC library
char* strdup(char* s)
{
    unsigned int len;
    char* m;
    
    for (len=0; (len<65534) && (s[len]!=0); len++);
    m = (char*) malloc(len+1);
    if (m)
    {   
        memcpy(m,s,len);
        m[len] = 0;
    }
    return m;
}    

// Replacement for broken fread of the WDC library which does not 
// handle bank boundaries correcty.
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t i,j;
    int b;
    unsigned char* p = (unsigned char*) ptr;
    
    if (size==1)
    {
        for (i=0; i<nmemb; i++)
        {
            b = getc(stream);
            if (b<0) { return i; }  // EOF encountered
            p[i] = b;
        }
    }
    else    
    {
        for (i=0; i<nmemb; i++)
        {
            for (j=0; j<size; j++) 
            {
                b = getc(stream);
                if (b<0) { return i; }  // EOF encountered
                p[j] = b;
            }
            p+=size;
        }
    }
    return nmemb;
}
