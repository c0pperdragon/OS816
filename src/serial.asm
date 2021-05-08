; Serial communication via the generic input and output ports.
; In order to not need any form of expernal uart receiver, hardware flow control
; via a CTS/RTS handshake is applied. With this the CPU only needs to handle input
; when it is actually prepared for it.
; Communication is done with 19200 baud, one start bit, one stop bit, no parity.

; Pin assignments:
;   TX    Output bit 0
;   RX    Input bit 0
;   RTS   Output bit 1
;   CTS   Input bit 1
; All other output pins are just set high when the serial communcation is running, and
; are left high after that. 

    CODE
    xdef ~~send
~~send:
    ; initial stack layout:  
    ;   SP+1, SP+2, SP+3    return address
    ;   SP+4, SP+5          data to send (in lower bits only)
    
    SEP #$30 ; switch to 8 bit registers
    longa off
    longi off

    ; must wait until the receiver is ready to accept new data (our CTS must be low)
waitforready:
    LDA >$400000
    AND #$02
    BNE waitforready

    ; send one byte bit by bit (1 start bit, 1 stop bit, no parity)
    LDA <4,S
    CLC         
    JSR sendbit ; start bit
    LSR A       
    JSR sendbit ; bit 0
    LSR A       
    JSR sendbit ; bit 1
    LSR A       
    JSR sendbit ; bit 2
    LSR A       
    JSR sendbit ; bit 3
    LSR A       
    JSR sendbit ; bit 4
    LSR A       
    JSR sendbit ; bit 5
    LSR A       
    JSR sendbit ; bit 6
    LSR A       
    JSR sendbit ; bit 7
    SEC        
    JSR sendbit  ; stop bit
    
    ; switch to 16 bit registers
    REP #$30 
    LONGI ON
    LONGA ON
    
    ; take down stack and return
    LDA <2,S
    STA <4,S
    PLA 
    STA <1,S
    RTL

    ; Send the bit that is given in the carry flag.
    ; To conserve the A register, use X as temporary storage.
    ; This subroutine uses 8-bit registers.
    longa off
    longi off
sendbit:
    TAX
    BCS send1
send0:
    LDA #$FE
    STA >$40FE00  ; send data on data bus and high address lines
    BRA donesend
send1: 
    LDA #$FF
    STA >$40FF00  ; send data on data bus and high address lines
    BRA donesend
donesend:
    TXA
    
    ; tuned delay loop to get a data rate of 9600 baud @ 10 Mhz
    LDX #202  

    ; detect underlying hardware
    CLC
    XCE 
    BCC delay2  ; genuine 65c816
    
    LDX #2   ; fine-tuned to work on Bernd also
    
delay2:
    DEX        ; 2 cycles
    BNE delay2 ; 3 cycles
    RTS
    ENDS
