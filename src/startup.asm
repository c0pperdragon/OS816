
    ; external references
    XREF _ROM_BEG_DATA
    XREF _BEG_DATA
    XREF _END_DATA
    XREF _BEG_UDATA
    XREF _END_UDATA
    XREF ~~main
    XREF ~~portset

    CODE
START:
    ; enter emulation mode - now we can also access RAM
    CLC ;clear carry
    XCE ;clear emulation

    ; set up 16 bit mode for all registers and memory access
    REP #$30 ;16 bit registers
    LONGI ON
    LONGA ON

    ; set the stack to start on top of bank 0
    LDA #$FFFF 
    TCS 

    ; the DATA and UDATA segments must reside in the same bank for 
    ; direct access. The DBR will always contain the number of this bank.
    ; The second block copy instruction below has the side effect of also setting
    ; the DBR.

    ; copy initial content into DATA segment 
    LDA #_END_DATA-_BEG_DATA ;number of bytes to copy
    BEQ SKIP ;if none, just skip
    DEC A ;less one for MVN instruction
    LDX #<_ROM_BEG_DATA ;get source into X
    LDY #<_BEG_DATA ;get dest into Y
    MVN #(^_ROM_BEG_DATA)+$80,#^_BEG_DATA ;copy bytes
SKIP:

    ; clear UDATA segment
    LDA #0
    STA >_BEG_UDATA   ; clear first two bytes (which must exist)
    LDA #_END_UDATA-_BEG_UDATA ;get total number of bytes to clear
    DEC A
    DEC A
    LDX #<_BEG_UDATA ;get start of area into X
    TXY
    INY              ;get the next byte address int Y
    MVN #^_BEG_UDATA,#^_BEG_UDATA ;zero out bytes with this overlapping block copy
DONE: 

;    ; set the output port to a defined state
    LDA #$00FF
    PHA
    JSL	>~~portset

    ; start the main function, and stop CPU upon return
    JSL >~~main
    STP
    ENDS

    ; specify a tiny UDATA section to make sure that UDATA exists in any case,
    ; this makes the initializing code so much shorter, because it does not
    ; need to handle any special cases
    UDATA
    DS 2
    ENDS

    ; The initial reset vector and a single long jump instruction to 
    ; get the code running from the true ROM address.
    ; If interrupt vectors need to be installed also, this must be done
    ; in RAM, because in native mode the whole of bank 0 is use for RAM
RESET SECTION   ; must locate at 80FFF8 (FFF8 in ROM) by the linker
    JMP >START   ; long jump to the startup code (4 byte instruction)
    DW $FFF8     ; reset vector is relative to bank 0
    ENDS
    END 
