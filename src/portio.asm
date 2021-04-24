    udata
    xdef outportdata
outportdata:
    ds 1     ; shadow register for output value
    ends

    CODE
    xdef ~~portset
~~portset:
    longa on
    longi on
    ; initial stack layout:  
    ;   SP+1, SP+2, SP+3    return address
    ;   SP+4, SP+5          16-bit parameter 

    SEP #$30 ;8 bit registers
    longa off

    LDA <4,S
    EOR #$00                ; only necessary to make execution time equal to portclear
    ORA |outportdata        ; set more bits
    STA >$800000            ; send to port and temporarily switch to IO mode
    STA |outportdata        ; keep value and switch back to normal mode

    REP #$30 ;16 bit registers
    longa on

    ; take down stack and return
    LDA <2,S
    STA <4,S
    PLA 
    STA <1,S
    RTL
    ENDS

    CODE
    xdef ~~portclear
~~portclear:
    longa on
    longi on
    ; initial stack layout:  
    ;   SP+1, SP+2, SP+3    return address
    ;   SP+4, SP+5          16-bit parameter 

    SEP #$30 ;8 bit registers
    longa off

    LDA <4,S
    EOR #$FF 
    AND |outportdata
    STA >$800000            ; send to port and temporarily switch to IO mode
    STA |outportdata        ; keep value and switch back to normal mode

    REP #$30 ;16 bit registers
    longa on

    ; take down stack and return
    LDA <2,S
    STA <4,S
    PLA 
    STA <1,S
    RTL
    ENDS

    CODE
    xdef ~~portin
~~portin:
    longa on
    longi on
    ; initial stack layout:  
    ;   SP+1, SP+2, SP+3    return address

    SEP #$30 ;8 bit registers
    longa off

    LDA |outportdata        ; get current output pattern
    STA >$800000            ; switch to IO mode without disturbing the output signal
    LDA <1,S                ; any read from RAM now reads from input port instead
    PHA                     ; writing to RAM turns off IO mode 
    PLA                     ;

    REP #$30 ;16 bit registers
    longa on

    AND #$00FF               ; clear higher bits
    RTL
    ENDS

    END 
