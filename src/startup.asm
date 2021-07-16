
    ; external references
    XREF _ROM_BEG_DATA
    XREF _BEG_DATA
    XREF _END_DATA
    XREF _BEG_UDATA
    XREF _END_UDATA
    XREF ~~main
    
    ; exports 
    xdef ~~heap_end
    xdef ~~heap_start
~~heap_start  equ $20000 
~~heap_end    equ $30000
    
    CODE
START:

    LONGI OFF
    LONGA OFF

    ;  ; set the output port to a defined state
    LDA #$FF
    STA >$400000
    
    ; set up 16 bit mode for all registers and memory access
    REP #$30 ;16 bit registers
    LONGI ON
    LONGA ON

    ; set the stack to start on top of bank 0
    LDA #$FFFF 
    TCS 

    ; copy initial content into DATA segment 
    LDA #_END_DATA-_BEG_DATA ;number of bytes to copy
    BEQ SKIPCOPY ;if none, just skip
    DEC A ;less one for MVN instruction
    LDX #<_ROM_BEG_DATA ;get source into X
    LDY #<_BEG_DATA ;get dest into Y
    MVN #(^_ROM_BEG_DATA)+$80,#^_BEG_DATA ;copy bytes
SKIPCOPY: 

    ; set D register to the bank of UDATA
    LDA #^_BEG_UDATA
    TCD

    ; clear UDATA segment
    LDA #_END_UDATA-_BEG_UDATA ; number of bytes to clear
    BEQ NOCLEAR
    LDX #<_BEG_UDATA  
    SEP #$20     ; for 8-bit memory access
    STZ <0,X     ; clear first byte
    REP #$20  
    DEA     
    BEQ NOCLEAR
    TXY   ; set up copy instruction for forward replication
    INY
    DEA  
    MVN #^_BEG_UDATA,#^_BEG_UDATA
NOCLEAR:

    ; start the main function, and stop CPU upon return
    LDA #0
    PHA
    PHA
    PHA
    JSL >~~main
    STP
    ENDS

    
    
    ; This is a very tricky startup code that needs to work with both
    ; the original 65c816 as well as the Bernd emulator.
RESET SECTION
   ORG $FFF2
    ; The original CPU will come up in emulation mode which has a different
    ; memory map so it can access the ROM via bank 0. 
    ; In this case we must switch to a true ROM bank and turn off emulation.
ORIGINAL65C816:           ; FFF2
    JMP >TOHIGHBANK  
TOHIGHBANK:               ; FFF6
    CLC 
    XCE                
    ; Bernd emulation will not use the reset vectors, but will directly jump to this
    ; location with emulation already turned off
BERND:                    ; FFF8
    JMP >START
    ; The reset vector for the 65C816
RESETVECTOR:              ; FFFC
    DW $FFF2
    ENDS
    
    END 
