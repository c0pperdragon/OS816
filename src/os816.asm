
    ; external references
    XREF _ROM_BEG_DATA
    XREF _BEG_DATA
    XREF _END_DATA
    XREF _BEG_UDATA
    XREF _END_UDATA
    XREF ~~main

; --------------------- RAM layout ----------------------- 
writeflash_mirrored set $00FF00
stacktop            set $00FBFF

    
; -------------- Persistent data--------------------------
    DATA
hasbuffered:  db 0
buffereddata: db 0
    ENDS
        
    
OS816 SECTION


    ORG $80EF00       ; This area will be replaced when flashing 
    ; -------------- start of the program initialization --------
STARTMAIN:
    ; set D register to the bank of UDATA
    LDA #^_BEG_UDATA
    TCD

    ; clear UDATA segment
    LDA #_END_UDATA-_BEG_UDATA ; number of bytes to clear
    BEQ NOCLEAR
    LDX #<_BEG_UDATA  
    SEP #$20     ; for 8-bit memory access
    STZ <0,X     ; clear first byte
    REP #$20  
    DEA     
    BEQ NOCLEAR
    TXY   ; set up copy instruction for forward replication
    INY
    DEA  
    MVN #^_BEG_UDATA,#^_BEG_UDATA
NOCLEAR:

    ; Copy initial content into DATA segment. 
    ; Also the Data Bank Register will be correctly set to this bank  
    ; by the MVN instruction    
    LDA #_END_DATA-_BEG_DATA ;number of bytes to copy
    DEC A ;less one for MVN instruction
    LDX #<_ROM_BEG_DATA ;get source into X
    LDY #<_BEG_DATA ;get dest into Y
    MVN #^_ROM_BEG_DATA,#^_BEG_DATA ;copy bytes

    ; start the main function
    PEA #^argv
    PEA #<argv    
    PEA #1
    JSL >~~main
    
    ; do proper exit handling
    PHA
    JSL ~~_exit

progname:
    DB 79,83,56,49,54,0  ; "OS816"
argv:
    DW #<progname
    DW #^progname
    DW 0
    DW 0

    

    ORG $80F000       ; Start of resident boot loader area     
; ------------ EXPORTS FROM THE BOOT LOADER CODE --------------    
    xdef ~~_exit
    xdef ~~send
    xdef ~~receive
	xdef ~~sendstr
    xdef ~~writeflash
~~_exit:
    JMP >restart
~~send:
    JMP >send
~~receive:
    JMP >receive
~~sendstr:
    JMP >sendstr
~~writeflash:
    JMP >writeflash_mirrored
    
; -------------------- STARTUP -------------------------------
START:
    LONGI OFF
    LONGA OFF

    ;  ; set the output port to a defined state
    LDA #$FF
    STA >$400000
    
    ; set up 16 bit mode for all registers and memory access
    REP #$30 ;16 bit registers
    LONGI ON
    LONGA ON

    ; copy flash programming routines to the very top of RAM for later use   
    LDA #writeflashend-writeflash-1  ; length-1
    LDX #<writeflash            ; get source into X
    LDY #<writeflash_mirrored   ; get dest into Y
    MVN #^writeflash,#^writeflash_mirrored   ;copy bytes

    ; to work around a bug in sprintf start stack
    ; 1K below end of bank
    LDA #stacktop
    TCS 

    JMP STARTMAIN

; ------------------ RESTART or SHUTDOWN --------------------
; Let the machine restart or stop entirely 
; This depends on the exit-value. All negative values will cause a complete stop. 
restart:
    ; initial stack layout:  
    ;   SP+1, SP+2, SP+3    return address
    ;   SP+4, SP+5          return value from main() or exit()
    LDA <4,S
    BPL START
    STP
    
; ----------------- Serial communication ------------------    
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

    ; send on byte via serial
send:
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
receive:
    ; initial stack layout:  
    ;   SP+1, SP+2, SP+3    return address
    ;   SP+4, SP+5          timeout (in milliseconds)

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
    ; switch to 16 bit registers
    REP #$30 
    LONGI ON
    LONGA ON
    
    ; take down stack
    ; take down stack and return
    LDA <2,S
    STA <4,S
    PLA 
    STA <1,S    
    ; the return value is provided in Y
    TYA
    AND #$00FF
    RTL
    
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
    NOP
    NOP
    LDX #12; #18     ; fine-tuned to give same speed on Bernd €12 Mhz
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

    LDX #13     ; fine-tuned to wait 1.5 bits on Bernd €12 Mhz
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

; ----------- Simple convenience function --------------------    

sendstr:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L42
	tcs
	phd
	tcd
str_0	set	4
i_1	set	0
	stz	<L43+i_1
L10044:
c_2	set	2
	ldy	<L43+i_1
	lda	[<L42+str_0],Y
	and	#$ff
	sta	<L43+c_2
	lda	<L43+c_2
	bne	L10045
	lda	<L42+2
	sta	<L42+2+4
	lda	<L42+1
	sta	<L42+1+4
	pld
	tsc
	clc
	adc	#L42+4
	tcs
	rtl
L10045:
	pei	<L43+c_2
	jsl	send
	inc	<L43+i_1
	bra	L10044
L42	equ	4
L43	equ	1
    
        
; ---------------- Write to FLASH -------------------------
; This code must be mirrored to RAM before using it, because
; ROM can not be accessed while programming is active
writeflash:
    ; stack layout (accessed via D):  
    ;   SP+1, SP+2, SP+3          return address
    ;   SP+4, SP+5, SP+6, SP+7    destination address
    ;   SP+8, SP+9, SP+10, SP+11  source address
    ;   SP+12, SP+13              length    
	longa	on
	longi	on
    TSC
    PHD
    TCD
        
    LDX <12
    BEQ writeflashdone
    LDY #0
writeflashloop:
    SEP #$20
	longa	off
    LDA #$AA
    STA >$805555
    LDA #$55
    STA >$802AAA
    LDA #$A0
    STA >$805555
    LDA [<8],Y
    STA [<4],Y
waitflashstable:
    LDA [<4],Y
    CMP [<4],Y
    BNE waitflashstable
    CMP [<4],Y
    BNE waitflashstable
    REP #$20
	longa	on
    INY
    DEX
    BNE writeflashloop   
    
writeflashdone:
	LDA	<1
    STA <11 
	LDA	<2
    STA <12 
    PLD
    PLA
    PLA
    PLA
    PLA
    PLA
    RTL	
writeflashend:

; ----------------- Reset code ----------------------------    
    ; This is a very tricky startup code that needs to work with both
    ; the original 65c816 as well as the Bernd emulator.
   ORG $80FFF2
    ; The original CPU will come up in emulation mode which has a different
    ; memory map so it can access the ROM via bank 0. 
    ; In this case we must switch to a true ROM bank and turn off emulation.
ORIGINAL65C816:           ; 80FFF2
    JMP >TOHIGHBANK  
TOHIGHBANK:               ; 80FFF6
    CLC 
    XCE                
    ; Bernd emulation will not use the reset vectors, but will directly jump to this
    ; location with emulation already turned off
BERND:                    ; 80FFF8
    JMP >START
    ; The reset vector for the 65C816
RESETVECTOR:              ; 80FFFC
    DW $FFF2
   
    
    ENDS 
    END
    