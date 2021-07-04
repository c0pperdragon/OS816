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

    ; one additional byte as input buffer
    DATA
hasbuffered:  db 0
buffereddata: db 0
    ENDS

    ; send on byte via serial
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


    ; receive one byte via serial
    xdef ~~receive
~~receive:
    SEP #$30 ; switch to 8 bit registers
    longa off
    longi off

    ; when there is a byte already in buffer, consume this first
    LDA |hasbuffered
    BEQ bufferempty
    STZ |hasbuffered
    LDY |buffereddata
    BRL donereceive    
bufferempty:

    ; must notify the sender that we are read to accept data
    LDA #$FD      ; set RTS low
    STA >$40FD00  ; sent value on data bus and high address lines

    ; wait for incomming start bit
waitforincommingdata:
    LDA >$400000
    LSR
    BCS waitforincommingdata

    ; prevent more data from coming in
    LDA #$FF      ; set RTS to high
    STA >$40FF00  ; sent value on data bus and high address lines
    
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
    TAY
    
    ; wait some additional time in case one more byte arrives
    LDX #255   ; loops to wait on genuine 65c816
    CLC
    XCE 
    BCC waitformoreincommingdata  
    LDX #35    ; loops to wait on Bernd
waitformoreincommingdata:
    DEX
    BEQ donereceive
    LDA >$400000
    LSR
    BCS waitformoreincommingdata
    
    ; keep current byte here temporarily
    STY |hasbuffered
    
    ; read the second byte
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
    
    ; set up buffer and return correct value here
    STA |buffereddata
    LDY |hasbuffered
    LDA #1
    STA |hasbuffered
    
donereceive:
    ; the return value is provided in Y
    TYA
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
   
    ; bild output data with carry flag in bit 0, the rest is high bits
    ROL
    ORA #$FE
    STA >$400000  ; write to output port

    ; fetch input
    LDA >$400000
    
    ; tuned delay loop 
    LDX #200    ; 9600 baud on genuine 65c816 @ 10 Mhz 
    ; detect underlying hardware
    CLC
    XCE 
    BCC delay2  
    LDX #18     ; fine-tuned to give same speed on Bernd €16 Mhz
delay2:
    DEX        ; 2 cycles
    BNE delay2 ; 3 cycles
    
    ; put input bit into carry flag
    LSR
    
    ; repair A and return
    TYA
    RTS

    
    ; Wait for about 1.5 bits.
    ; This subroutine uses 8-bit registers.
    LONGA OFF
    LONGI OFF
waitfor1_5bits:

    LDX #21     ; fine-tuned to wait 1.5 bits on Bernd €16 Mhz
    ; detect underlying hardware
    CLC
    XCE 
    BCS delay3      
    LDX #100   ; delay for one extra half bit 
halfdelay:
    DEX        ; 2 cycles
    BNE halfdelay ; 3 cycles
    LDX #250   ; 9600 baud on genuine 65c816 @ 10 Mhz 
delay3:
    DEX        ; 2 cycles
    BNE delay2 ; 3 cycles
    
    RTS

    ENDS
    END 

    