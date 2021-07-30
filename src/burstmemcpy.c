#include <os816.h>

// MVN instruction constructed in RAM
unsigned char mvn[4] =
{
     0x54 ,    // MVN opcode
     0,        // destination bank
     0,        // source bank
     0x6B      // RTL
};

void burstmemcpy(void* destination, void* source, unsigned int len)
{
    unsigned int daddress = ((unsigned int *)(&destination)) [0];
    unsigned int dbank = ((unsigned int *)(&destination)) [1];
    unsigned int saddress = ((unsigned int *)(&source)) [0];
    unsigned int sbank = ((unsigned int *)(&source)) [1];
    
    if (len==0) { return; }
    
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
