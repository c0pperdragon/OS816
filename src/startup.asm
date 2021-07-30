
    ; external references
    XREF _ROM_BEG_DATA
    XREF _BEG_DATA
    XREF _END_DATA
    XREF _BEG_UDATA
    XREF _END_UDATA
    XREF ~~main
    
    
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

    ; to work around a bug in sprintf start stack
    ; 1K below end of bank
    LDA #$FC00
    TCS 

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

    ; Copy initial content into DATA segment. there are at least the
    ; heap vectors in this segment, so no need to check for its existence.
    ; Also the Data Bank Register will be correctly set to this bank  
    ; by the MVN instruction    
    LDA #_END_DATA-_BEG_DATA ;number of bytes to copy
    DEC A ;less one for MVN instruction
    LDX #<_ROM_BEG_DATA ;get source into X
    LDY #<_BEG_DATA ;get dest into Y
    MVN #^_ROM_BEG_DATA,#^_BEG_DATA ;copy bytes

    ; start the main function, and stop CPU upon return
    PEA #^argv
    PEA #<argv    
    PEA #2
    JSL >~~main
    STP
    ENDS

    CODE
progname:
    DB 79,83,56,49,54,0  ; "OS816"
gamename:
    DB 100,101,102,97,117,108,116,0     ; "default"
argv:
    DW #<progname
    DW #^progname
    DW #<gamename
    DW #^gamename
    DW 0
    DW 0
    ENDS    

    
    ; heap position when using the WDC library
    ; for heap management
    XDEF ~~heap_start
    XDEF ~~heap_end
    DATA
~~heap_start:
    DW #$0000
    DW #$0001
~~heap_end:
    DW #$0000
    DW #$0002
    ENDS

    
    ; Very simple way to terminate the program
    xdef ~~_exit
    CODE
~~_exit:
    STP
    ENDS

    
        
    ; This is a very tricky startup code that needs to work with both
    ; the original 65c816 as well as the Bernd emulator.
RESET SECTION
   ORG $80FFF2
    ; The original CPU will come up in emulation mode which has a different
    ; memory map so it can access the ROM via bank 0. 
    ; In this case we must switch to a true ROM bank and turn off emulation.
ORIGINAL65C816:           ; 80FFF2
    JMP >TOHIGHBANK  
TOHIGHBANK:               ; 80FFF6
    CLC 
    XCE                
    ; Bernd emulation will not use the reset vectors, but will directly jump to this
    ; location with emulation already turned off
BERND:                    ; 80FFF8
    JMP >START
    ; The reset vector for the 65C816
RESETVECTOR:              ; 80FFFC
    DW $FFF2
    ENDS
    

    END