
    ; external references
    XREF _BEG_UDATA
    XREF _END_UDATA
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
    STA >$40FFFF
    
    ; set up 16 bit mode for all registers and memory access
    REP #$30 ;16 bit registers
    LONGI ON
    LONGA ON

    ; set the stack to start on top of bank 0
    LDA #$FFFF 
    TCS 

    ; the UDATA segment is cleared before start. The DBR will then always contain
    ; the bank of this the UDATA, which must not exceed 64K.
    LDA #_END_UDATA-_BEG_UDATA ;get total number of bytes to clear
    BEQ SKIPUDATA 
    ; clear first byte of buffer
    SEP #$20 ;8 bit accu/memory
    LONGA OFF
      LDA #0
      STA >_BEG_UDATA  
    REP #$20 ;16 bit accu/memory
    LONGA ON   
    LDA #_END_UDATA-_BEG_UDATA
    DEC A
    BEQ SKIPUDATA ; buffer is of length 1, so no clear loop is needed
    DEC A    
    ; make a overlapping transfer to zero out the whole buffer
    LDX #<_BEG_UDATA ;get start of area into X
    TXY
    INY              ;get the next byte address into Y
    MVN #^_BEG_UDATA,#^_BEG_UDATA 
SKIPUDATA: 

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
