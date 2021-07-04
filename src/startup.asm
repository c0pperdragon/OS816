
    ; external references
    XREF _ROM_BEG_DATA
    XREF _BEG_DATA
    XREF _END_DATA
    XREF ~~main

    CODE
START:
    LONGI OFF
    LONGA OFF
    
    ; enter emulation mode - now we can also access RAM
    CLC 
    XCE ;clear emulation

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
    BEQ SKIP ;if none, just skip
    DEC A ;less one for MVN instruction
    LDX #<_ROM_BEG_DATA ;get source into X
    LDY #<_BEG_DATA ;get dest into Y
    MVN #(^_ROM_BEG_DATA)+$80,#^_BEG_DATA ;copy bytes
SKIP: 

    ; start the main function, and stop CPU upon return
    JSL >~~main
    STP
    ENDS

    ; The initial reset vector and a single long jump instruction to 
    ; get the code running from the true ROM address.
    ; If interrupt vectors need to be installed also, this must be done
    ; in RAM, because in native mode the whole of bank 0 is use for RAM
RESET SECTION   ; must locate at 80FFF8 by the linker
    JMP >START   ; long jump to the startup code (4 byte instruction)
    DW $FFF8     ; reset vector is relative to bank 0
    ENDS
    END 
