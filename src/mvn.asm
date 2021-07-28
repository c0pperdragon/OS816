
    ; give access to the MVN instruction for fast memory transfer
    ; this is pretty tricky to implement, because the MVN instruction has
    ; the source and destination banks hardcoded as part of the instruction
    ; itself. So we need a way to temporarily modify this instruction for execution.

    ; this code is in the DATA region, so it can be modified
    DATA
template:    
    DB #$54    ; MVN opcode
destinationbank:
    DB 0
sourcebank:
    DB 0
    RTL
    ENDS

    
    CODE
    xdef ~~MVN
~~MVN:
    PHB  ; save data bank (is overwritten by MVN)
    
    ; initial stack layout:  
    ;   SP+1                previous data bank
    ;   SP+2, SP+3, SP+4    return address
    ;   SP+5, SP+6          destination address
    ;   SP+7, SP+8          destination bank
    ;   SP+9, SP+10,        source address
    ;   SP+11, SP+12        source bank
    ;   SP+13, SP+14        length_minus_one

    ; prepare the MVN instruction
    SEP #$30 ; switch to 8 bit registers
    longa off
    longi off
    LDA <7,S     
    STA |destinationbank
    LDA <11,S    
    STA |sourcebank

    REP #$30 ; switch to 16 bit registers
    longa on
    longi on
    
    ; set up parameters and call prepared instruction
    LDA <5,S
    TAY
    LDA <9,S
    TAX
    LDA <13,S
    JSL template
    
    ; construct the return address in the correct place
    LDA <2,S
    STA <12,S
    LDA <3,S
    STA <13,S

    ; repair the data bank register
    PLB
    
    ; take down rest of the stack and return
    PLA
    PLA
    PLA
    PLA
    PLA
    RTL
    
    ENDS
    END 


