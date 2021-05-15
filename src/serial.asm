; Serial communication via the generic input and output ports.
; In order to not need any form of expernal uart receiver, hardware flow control
; via a CTS/RTS handshake is applied. With this the CPU only needs to handle input
; when it is actually prepared for it.
; Communication is done with 9600 baud, one start bit, one stop bit, no parity.

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
    JSR sendreceivebit ; start bit
    ROR A       
    JSR sendreceivebit ; bit 0
    ROR A       
    JSR sendreceivebit ; bit 1
    ROR A       
    JSR sendreceivebit ; bit 2
    ROR A       
    JSR sendreceivebit ; bit 3
    ROR A       
    JSR sendreceivebit ; bit 4
    ROR A       
    JSR sendreceivebit ; bit 5
    ROR A       
    JSR sendreceivebit ; bit 6
    ROR A       
    JSR sendreceivebit ; bit 7
    SEC        
    JSR sendreceivebit  ; stop bit
    
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



    xdef ~~receive
~~receive:
    SEP #$30 ; switch to 8 bit registers
    longa off
    longi off

    ; must notify the sender that we are read to accept data
    LDA #$FD      ; set RTS low
    STA >$40FD00  ; sent value on data bus and high address lines

    ; wait for incomming start bit
waitforincommingdata:
    LDA >$400000
    LSR
    BCS waitforincommingdata
    
    ; wait to reach the middle of the first data bit
    JSR waitfor1_5bits

    LDA #$FF   ; outgoing bits are all 1s
    ; receive the byte bit by bit
    ROR A
    JSR sendreceivebit ; bit 0
    ROR A       
    JSR sendreceivebit ; bit 1
    ROR A       
    JSR sendreceivebit ; bit 2
    ROR A       
    JSR sendreceivebit ; bit 3
    ROR A       
    JSR sendreceivebit ; bit 4
    ROR A       
    JSR sendreceivebit ; bit 5
    ROR A       
    JSR sendreceivebit ; bit 6
    ROR A       
    JSR sendreceivebit ; bit 7
    ROR A
    
    ; switch to 16 bit registers
    REP #$30 
    LONGI ON
    LONGA ON
    
    ; take down stack and return result in 16 bit
    AND #$00FF
    RTL
    
    
; ---------------------------------------------------------------------------

    ; Send the bit that is given in the carry flag while at the
    ; same time receive the current input into the carry flag. 
    ; The A register needs to be conserved during this.
    ; This subroutine uses 8-bit registers.
    LONGA OFF
    LONGI OFF
sendreceivebit:
    TAY
   
    ; send either bit 0 or bit 1
    BCS send1
send0:
    LDA #$FE
    STA >$40FE00  ; send data on data bus and high address lines
    SEC
    BCS donesend  ; do branch-taken to balance out the previous branch-not-taken
send1: 
    LDA #$FF
    STA >$40FF00  ; send data on data bus and high address lines
    SEC
    BCS donesend  ; do branch-not-taken to balance out the previous branch-taken
donesend:

    ; fetch input
    LDA >$400000
    
    ; tuned delay loop 
    LDX #202    ; 9600 baud on genuine 65c816 @ 10 Mhz 
    ; detect underlying hardware
    CLC
    XCE 
    BCC delay2  
    LDX #17     ; fine-tuned to give same speed on Bernd €12 Mhz
delay2:
    DEX        ; 2 cycles
    BNE delay2 ; 3 cycles
    
    ; put input bit into carry flag
    LSR
    
    ; repair A and return
    TYA
    RTS

    
    ; Wait for 1.5 bits.
    ; This subroutine uses 8-bit registers.
    LONGA OFF
    LONGI OFF
waitfor1_5bits:

    LDX #202    ; 9600 baud on genuine 65c816 @ 10 Mhz 
    ; detect underlying hardware
    CLC
    XCE 
    BCC delay3      
    LDX #17     ; fine-tuned to give same speed on Bernd €12 Mhz
delay3:
    DEX        ; 2 cycles
    BNE delay2 ; 3 cycles
    
    RTS
