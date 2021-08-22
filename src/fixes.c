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



// Replace standard memcpy with an optimized version that
// can make use of the MVN instruction (not really a fix, but speeds things up)

// MVN instruction constructed in RAM
unsigned char mvn[4] =
{
     0x54 ,    // MVN opcode
     0,        // destination bank
     0,        // source bank
     0x6B      // RTL
};

void *memcpy(void* destination, const void* source, unsigned int len)
{
    // if nothing to copy, just pass
    if (len==0)
    {
    }
    // Simple version with low startup-cost, but higher per-item cost
    else if (len<10)
    {
    #asm
        LDX %%len
        LDY #0
        SEP #$20    
        longa off
    memcpyloop:    
        LDA [%%source],Y
        STA [%%destination],Y
        INY
        DEX
        BNE memcpyloop
        REP #$20    
        longa on
    #endasm      
    }
    // Burst version using the MVN instruction
    else
    {
        unsigned int daddress = ((unsigned int *)(&destination)) [0];
        unsigned int dbank = ((unsigned int *)(&destination)) [1];
        unsigned int saddress = ((unsigned int *)(&source)) [0];
        unsigned int sbank = ((unsigned int *)(&source)) [1];        
        
        for (;;)
        {
            unsigned int span;
            unsigned int span_m_1 = len - 1;
            unsigned int dremain_m_1 = 65535-daddress;
            unsigned int sremain_m_1 = 65535-saddress;
            if (dremain_m_1 < span_m_1) { span_m_1 = dremain_m_1; }
            if (sremain_m_1 < span_m_1) { span_m_1 = sremain_m_1; }
        #asm
            ; prepare MVN instructions with destination and source bank
            SEP #$20    
            longa off
            LDA %%dbank
            STA %%mvn+1
            LDA %%sbank
            STA %%mvn+2
            REP #$20    
            longa on
            ; load positions and counters
            LDY %%daddress
            LDX %%saddress
            LDA %%span_m_1
            ; perform the MVN while retaining data bank register
            PHB
            JSL %%mvn
            PLB
        #endasm       
            span = span_m_1 + 1;
            if (span==len) { break; }
            len -= span;
            daddress += span;
            if (daddress==0) { dbank++; }
            saddress += span;
            if (saddress==0) { sbank++; }
        }
    }
    return destination;
}

