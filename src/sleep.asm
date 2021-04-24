
    CODE
    xdef ~~sleep
~~sleep:
    longa on
    longi on
    ; initial stack layout:  
    ;   SP+1, SP+2, SP+3    return address
    ;   SP+4, SP+5          16-bit parameter: milliseconds

    LDA <4,S   

    ; this loop is fine-tuned to take exactly 12000 clocks per iteration
    ; (one-time method call overhead can not be avoided) 
    beq done
continue:
    ldx #0           ; 3 cycles
    ldx #2398        ; 3 cycles
continue2:
    dex              ;   2 cycles
    bne continue2    ;   2 or 3 (if taken) cycles
    dec a            ; 2 cycles
    bne continue     ; 2 or 3 (if taken) cycles
done:
                     ; SUM = 3 + 3 + (2+3)*198 - 1 + 2 + 3 = 12000
    ; take down stack and return
    LDA <2,S
    STA <4,S
    PLA 
    STA <1,S
    RTL

    ENDS
    END 
