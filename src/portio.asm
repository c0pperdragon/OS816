
    CODE
    xdef ~~portout
~~portout:
    longa on
    longi on
    ; initial stack layout:  
    ;   SP+1, SP+2, SP+3    return address
    ;   SP+4, SP+5          16-bit parameter 

    LDA <3,S                
    TAX                     ; get low 8 bits of parameter into the high bits of X    

    SEP #$20 ;8 bit accu/memory
    longa off

    LDA <4,S                ; get low 8 bits of parameter from stack
    STA >$400000,X          ; write 8 bits to IO range, provide data also on address lines A8-A15
     
    REP #$20 ;16 bit accu/memory
    longa on

    ; take down parameters, fix return address and return
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

    LDA >$400000            ; read 8 bits from IO range

    REP #$30 ;16 bit registers
    longa on
    
    AND #$00FF               ; clear higher bits
    RTL
    ENDS

    END 
