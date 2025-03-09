#include <string.h>

// Replace standard memcpy with an optimized version that
// can make use of the MVN instruction (not really a fix, but speeds things up)

void *memcpy(void* destination, const void* source, unsigned int len)
{
	#asm
		; reset read/write cursor 
		LDY #0
		; number of bytes to transfer
		LDX %%len
		BEQ copydone
		; test if the number of bytes is odd
		TXA 
		AND #1
		BEQ copywords
		; copy the single odd byte
		SEP #$20      ; switch to single byte accu
		LDA [%%source],y
		STA [%%destination],y
		REP #$20      ; switch back to 2-byte accu
		INY
		DEX
		; if it was only 1 byte long
		BEQ copydone
		; loop to copy 2 bytes at a time (13.5 cycles per byte)
	copywords:
		LDA [%%source],y    ; 8 cycles
		STA [%%destination],y     ; 8 cycles
		INY           ; 2 cycles 
		INY           ; 2 cycles
		DEX           ; 2 cycles
		DEX           ; 2 cycles
		BNE copywords ; 3 cycles
	copydone:
	#endasm
    return destination;
}

/*

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
    // For longer transfer may utilize the MVN
    else
    {
        // compute how many bytes could be copied without crossing a bank boundary
        unsigned int dadr16 = (unsigned int)(unsigned long) destination;
        unsigned int sadr16 = (unsigned int)(unsigned long) source;
        unsigned int maxspan = ( ~( (dadr16>sadr16) ? dadr16 : sadr16 ) ) + 1;   // if 0, this means actually 65536

        // Can use a single MVN when no bank boundaries are crossed
        if (maxspan==0 || len<=maxspan)
        {
        #asm
            ; prepare MVN instructions with destination and source bank
            SEP #$20    
            longa off
            LDA %%destination+2
            STA %%mvn+1
            LDA %%source+2
            STA %%mvn+2
            REP #$20    
            longa on
            ; load positions and counters
            LDY %%destination
            LDX %%source
            LDA %%len
            DEC A
            ; perform the MVN while retaining data bank register
            PHB
            JSL %%mvn
            PLB
        #endasm       
        }
        // when crossing bank boundaries, must split operation
        else
        {
            memcpy(destination, source, maxspan);
            memcpy(
                (void*) (((unsigned long)destination) + maxspan),
                (void*) (((unsigned long)source) + maxspan), 
                len-maxspan);         
        }
    }
    return destination;
}

*/