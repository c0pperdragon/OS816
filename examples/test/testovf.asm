    CODE
    xdef ~~testovf
~~testovf:
    longa on
    longi on
    ; initial stack layout:  
    ;   SP+1, SP+2, SP+3    return address
    ;   SP+4, SP+5          16-bit parameter: a
    ;   SP+6, SP+7          16-bit parameter: b
    

    ; check what happens with overflow on add
    LDA <4,S
    CLC
    ADC <6,S
    
    LDA #0
    BVC noovf1
    LDA #2
noovf1:

    TAY

    ; check what happens with overflow on sub
    LDA <4,S
    SEC
    SBC <6,S

    BVC noovf2
    INY
noovf2:
                     
    ; take down parameters, fix return
    LDA <2,S
    STA <6,S
    PLA 
    STA <3,S
    PLA
    
    TYA
    RTL
  
    ENDS
    END 
