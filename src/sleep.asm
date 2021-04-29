
    CODE
    xdef ~~sleep
~~sleep:
    longa on
    longi on
    ; initial stack layout:  
    ;   SP+1, SP+2, SP+3    return address
    ;   SP+4, SP+5          16-bit parameter: milliseconds
    
    ; normally this loop is fine-tuned to take 10000 clocks per iteration
    ; (one-time method call overhead can not be avoided) 
    LDY #1998    
    
    ; check if the code is running on a true 65c816 - use cycle-exact timing
    CLC
    XCE 
    BCC delayloop
    ; emulated by Bernd. use different delays for this (tuned by measurment)
    LDY #60
    
delayloop:
    LDA <4,S   
    BEQ done
continue:
    TYX              ; 2 cycles
    TYX              ; 2 cycles
    TYX              ; 2 cycles
continue2:
    DEX              ;   2 cycles
    BNE continue2    ;   2 or 3 (if taken) cycles
    DEC a            ; 2 cycles
    BNE continue     ; 2 or 3 (if taken) cycles
done:
                     ; SUM = 2+2+2 + (2+3)*1998 - 1 + 2 + 3 = 10000
                     
    ; take down parameters, fix return address and return
    LDA <2,S
    STA <4,S
    PLA 
    STA <1,S
    RTL
  
    ENDS
    END 
