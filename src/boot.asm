
; --------------------- RAM layout ---------------------------- 
stacktop            set $00FBFF   ; stay 1K clear of top to work around a bug in sprintf
buffereddata        set $00FFE0   ; 30 byte
numbuffered         set $00FFFE   ; 8 bit
numconsumed         set $00FFFF   ; 8 bit

; The timeouts for waiting for incomming data after asserting RTS and then also after
; de-asserting the line again. 
; For true 65c816:
;    One timeout unit is 17 clocks, so a receive function call will
;    cause an idle delay of about 600 microseconds.
; For Bernd emulation:
timeout1             set 244
timeout2             set 102

BOOT SECTION 
    ORG $80F000     
    LONGA ON
    LONGI ON
    
; ------------ JUMP TABLE INTO THE BOOT LOADER ROUTINES ---------    
    JMP >~~softreset            ; 80F000
    JMP >~~sleep                ; 80F004
    JMP >~~send                 ; 80F008 
    JMP >~~receive              ; 80F00C
    JMP >~~sendstr              ; 80F010 
    JMP >~~writeflash           ; 80F014
    JMP >~~eraseflash           ; 80F018
    
; -------------------- STARTUP -------------------------------
~~softreset:
    ; 16-bit mode for accu and index registers
    ; clear all other status flags as well
    REP #$FF   
   
    ; set the output port to a defined state
    SEP #$20 
    LONGA OFF
    LDA #$FF
    STA >$7F0000
    REP #$20 
    LONGA ON
            
    ; initial stack pointer
    LDA #stacktop
    TCS 
    ; initial direct pointer  = 0
    LDA #0
    TCD
    ; initial data bank register = 0
    PHA
    PLB
    PLB

    ; initialize serial communication buffer
    LDA #0
    STA >numbuffered  ;and also clear numconsumed
    
    ; for genuine 65c816 add a small delay to stay clear of 
    ; potential double resets at power up
    CLC
    XCE 
    BCS skipstartupdelay
    PEA #100
    JSL ~~sleep
skipstartupdelay:
    
    ; start the user program
    JMP $810000
    
; ----------------- Tuned delay loop ----------------------
~~sleep:
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
    ; emulated by Bernd. use different delays for this (tuned by measurement)
    LDY #190
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
    DEC A            ; 2 cycles
    BNE continue     ; 2 or 3 (if taken) cycles
done:                ; SUM = 2+2+2 + (2+3)*1998 - 1 + 2 + 3 = 10000                     
    ; take down parameters, fix return address and return
    LDA <2,S
    STA <4,S
    PLA 
    STA <1,S
    RTL
    
; ----------------- Serial communication ------------------    
; Serial communication via the generic input and output ports.
; In order to not need any form of expernal uart receiver, hardware flow control
; via a CTS/RTS handshake is applied. With this the CPU only needs to handle input
; when it is actually prepared for it.
; Communication is done one start bit, one stop bit, no parity.
; When running on genuine 65C816, the speed is 115200 baud.
; On Bernd the speed is clocked down to 9600 baud.

; Pin assignments:
;   TX    Output bit 0
;   RX    Input bit 7
;   RTS   Output bit 1
;   CTS   Input bit 6
; All other output pins are just set high when the serial communcation is running, and
; are left high after that. 

    ; -- send one byte via serial
~~send:
    ; initial stack layout:  
    ;   SP+1, SP+2, SP+3    return address
    ;   SP+4, SP+5          data to send (in lower bits only)
    ; save data bank register
    PHB      
    ; switch to 8 bit accu/memory
    SEP #$20 
    longa off
    ; make IO address available via DBR
    LDA #$7F 
    PHA
    PLB      
    ; must wait until the receiver is ready to accept new data (incomming CTS must be low)
waitforready:
    BIT |0
    BVS waitforready
    ; prepare the other output bits for the io port
    LDA #$FF
    PHA
    ; send one byte bit by bit (1 start bit, 1 stop bit, no parity)
    LDA <4+2,S
    CLC         
    JSR sendreceivebit ; start bit    6 cycles
    ROR A              ;              2 cycles                
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
    SEC                ;             2 cycles
    JSR sendreceivebit ; stop bit 
    PLA
    ; switch to 16 bit accu/memory
    REP #$20 
    LONGA ON
    ; restore DBR
    PLB
    ; take down stack and return
    LDA <2,S
    STA <4,S
    PLA 
    STA <1,S
    RTL
  
    ; -- receive one byte via serial
~~receive:
    ; save data bank register 
    PHB      
    ; switch to 8 bit accu/memory
    LDA #0  ; make sure high byte is 0 from now on, so TAX and TAY work
    SEP #$20 
    longa off
    ; when there is unconsumed data left in buffer, no need to get more
    LDA >numconsumed
    CMP >numbuffered
    BMI donereceive
    ; completely reset buffer
    LDA #0
    STA >numbuffered 
    STA >numconsumed 
    ; prepare accessing the io addresses
    LDA #$7F
    PHA
    PLB
    ; notify the sender that we are ready to accept data
    LDA #$FD      ; set RTS low
    STA |0
    ; Implement an edge-detector with latency jitter as low as possible.
    ; On real CPU the uncertainty is about 9 clocks which is just 0.1 bits
    ; On Bernd the uncertainty is about 150 clocks (12.5us) which is nearly the
    ; same relative jitter for a 9600 bit/s signal.
state_RTS_active:
    CLC           ; carry flag for timeout   
    BIT |0                                   
    BPL startbitdetected                       
    LDA #256-timeout1                        
waitforstartbit:
    BIT |0                                   ; 4      (55) 
    BPL startbitdetected                     ; 2      (21)
    ADC #1        ; progress timeout         ; 2      (71)
    BIT |0                                   ; 4      (55)
    BPL startbitdetected                     ; 2  2   (21)
    BCC waitforstartbit                      ; 3      (43)
timeoutreached:
    BRA set_RTS_inactive
startbitdetected:
    LDA #$FE                                 ; 2  4
    PHA                                      ; 3  7
    JSR receiveandstorebyte                  ; 6  13
    PLA
    ; when buffer is not too full, keep RTS active
    LDA >numbuffered
    CMP #25
    BMI state_RTS_active
    ; set RTS high
set_RTS_inactive:
    LDA #$FF      
    STA |0
    ; second implementation of the edge-detector. this is used
    ; in the state state when RTS is already de-asserted.
    ; Then the sender should stop as soon as possible. but may
    ; send a few more bytes.
state_RTS_inactive:
    CLC           ; carry flag for timeout   
    BIT |0                                   
    BPL startbitdetected2                    
    LDA #256-timeout2                          
waitforstartbit2:                            
    BIT |0                                   
    BPL startbitdetected2                    
    ADC #1        ; progress timeout         
    BIT |0                                    
    BPL startbitdetected2                    
    BCC waitforstartbit2                     
timeoutreached2:
    BRA donereceive
startbitdetected2:
    LDA #$FF
    PHA
    JSR receiveandstorebyte                  
    PLA
    BRA state_RTS_inactive
    ; after operation, may or may not have data in buffer
    ; when numbuffered > 0, there is something usable
donereceive:
    LDY #$FFFF             ; default value for no data
    LDA >numbuffered
    BEQ returnfromreceive  ; when nothing here
    LDA >numconsumed
    TAX
    INC A
    STA >numconsumed       ; progress numconsumed counter
    LDA >buffereddata,X
    LDY #0
    TAY                    ; 0-expand to 16 bit
returnfromreceive:
    REP #$20 
    LONGA ON
    TYA
    PLB
    RTL    

    ; Subroutine to fetch a byte of data and store in input buffer
    ; routine will terminate as quickly as possible but only
    ; after input level is inactive (high) again.
    ; With 0 - 9 clocks jitter, need to do the first sampling at
    ; 130 clocks after the flank of the start bit
receiveandstorebyte:  
    LONGA OFF                                            ;    13
    ; add some delay to get the first bit sample point correct
    ; detect underlying hardware
    CLC                                                  ; 2  15
    XCE                                                  ; 2  17
    BCC true65c816_2                       ; branch taken: 3  20
berndemulation_2:
    LDX #6                                              
delay4:
    DEX                                                  
    BNE delay4
    BRA startreceivebyte
true65c816_2:
    LDX #7                                               ; 3  23
delay5: 
    DEX                                                  ; 2  25 30 35 40 45   
    BNE delay5                                          ; 2/3 28 33 38 43 47  
startreceivebyte:
    NOP                                                  ; 2  49
    ; pass down the pattern for other output bits 
    LDA <3,S                                             ; 4  53
    PHA                                                  ; 3  56
    ; prepare outgoing TX bits to be all idle
    LDA #$FF                                             ; 2  58
    SEC                                                  ; 2  60    
    ; transfer
    JSR sendreceivebit ; bit 0                           ; 70 130
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
    ; temporarily keep data here
    TAY 
    ; remove parameter from stack
    PLA
    ; store the data if there is space left
    LDA >numbuffered
    CMP #30
    BPL receiveandstoredone
    TAX
    INC A
    STA >numbuffered
    TYA
    STA >buffereddata,X
receiveandstoredone:
    ; wait until input reaches idle level
    ; (either from stop bit, or when last data bit was high)
    BIT |0                                  
    BPL receiveandstoredone                   
    RTS
    LONGA ON

    ; Send the bit that is given in the carry flag while at the
    ; same time receive the current input into the carry flag. 
    ; The A register needs to be conserved during this.
    ; The caller prepares DBR to point to the IO range.
    ; This subroutine uses 8-bit accu/memory. Calling is done with near JSR.
    ; The other bits for output port are taken from the stake (just above return address).
    ; For real 86c816:
    ;   To get the 115200 baud with 10 Mhz clock, each bits needs to take about
    ;   87 clocks. With 8 clocks used by the caller, this function is tuned to 
    ;   consume exactly 79 clocks.
    ;   Sampling the input bit is done at about 70 clocks after the call.
    ; For Bernd emulation:
    LONGA OFF
sendreceivebit:
    TAY                                                  ; 2   2 
    ; bild output data with carry flag in bit 0,
    ; the rest is taken from stack parameter
    LDA <3,S                                             ; 4   6
    ROL                                                  ; 2   8
    STA |0         ; write to output port                ; 4  12
    ; detect underlying hardware
    CLC                                                  ; 2  14
    XCE                                                  ; 2  16
    BCC true65c816                         ; branch taken: 3  19
berndemulation:
    LDX #13                                              
delay1:
    DEX                                                  
    BNE delay1
    BRA donedelay
true65c816:
    LDX #4                                               ; 3  22
delay2:
    NOP                                                  ; 2  24 34 44 54
    BRA delay3cycles                                     ; 3  27 37 47 57
delay3cycles:
    DEX                                                  ; 2  29 39 49 59 
    BNE delay2                                          ; 3/2 32 42 52 61
donedelay:
    NOP                                                  ; 2  63
    NOP                                                  ; 2  65
    LDA |0         ; fetch input                         ; 4  69
    ASL A  ; put input bit into carry flag               ; 2  71
    TYA    ; repair A and return                         ; 2  73
    RTS                                                  ; 6  79
    LONGA ON

; ----------- Simple convenience function --------------------    
    ; stack layout:
    ;   SP+1, SP+2, SP+3        return address
    ;   SP+4, SP+5, SP+6, SP+7  pointer to string
~~sendstr:
    TSC
    PHD
    TCD
sendstrloop:
	LDA	[<4]
    AND #$00FF
    BEQ sendstrend
    PHA
    JSL ~~send
    INC <4
    BNE sendstrloop
    INC <6
    BNE sendstrloop
sendstrend: 
    LDA <1
    STA <5
    LDA <2
    STA <6
    PLD
    PLA
    PLA
    RTL 
        
; ---------------- Write to FLASH -------------------------
~~writeflash:
; stack frame (accessed via D):  
;   D+1 - D+50                mirrored code    
;   D+51, D+52                previous D 
;   D+53, D+54, D+55          return address
;   D+56, D+57, D+58, D+59    destination address
;   D+60, D+61, D+62, D+63    source address
;   D+64, D+65                length    
    ; set up stack frame
    PHD
    TSC
    SEC
    SBC #50
    TCS
    TCD
    ; transfer program to RAM
    LDX #32
transferaccesscode:
    LDA >writebytetoflash,X
    STA <1,X
    DEX
    DEX
    BPL transferaccesscode    
    ; init counters
    LDY <64
    BEQ skipcopyloop
    ; use 8-bit accu/memory access during loop
    SEP #$20
    LONGA OFF
writeflashloop:    
    DEY
    JSL executefromstackframe  
    TYX
    BNE writeflashloop
    ; revert to 16 bit
    REP #$20
    LONGA ON
skipcopyloop:
    ; transfer return address to where it is needed
    LDA <53
    STA <63
    LDA <54
    STA <64
    ; repair D, take down stack frame and return    
    LDA <51
    TCD
    TSC 
    CLC
    ADC #62
    TCS
    RTL
; This code needs to be mirrored to RAM before executing. 
; parameters:
;     Y  offset in source and the target buffer
; D must point to the stack frame as specified by writeflash. 
; Register widths A/M 8 bit, X/Y 16 bit
; Invocation by far subroutine call
writebytetoflash:
    LONGA OFF
    LDA #$AA                 ; 2
    STA >$805555             ; 4
    LDA #$55                 ; 2
    STA >$802AAA             ; 4
    LDA #$A0                 ; 2
    STA >$805555             ; 4
    LDA [<60],Y              ; 2
    STA [<56],Y              ; 2
waitflashstable:
    LDA [<56],Y              ; 2
    CMP [<56],Y              ; 2
    BNE waitflashstable      ; 2
    CMP [<56],Y              ; 2
    BNE waitflashstable      ; 2
    RTL                      ; 1
    LONGA ON                 ; 33 bytes total 

; ---------------- Erase to FLASH -------------------------
~~eraseflash:
; stack frame (accessed via D):  
;   D+1 - D+50                mirrored code    
;   D+51, D+52                previous D 
;   D+53, D+54, D+55          return address
;   D+56, D+57, D+58, D+59    sector address
    ; set up stack frame
    PHD
    TSC
    SEC
    SBC #50
    TCS
    TCD
    ; transfer program to RAM
    LDX #44
transfererasecode:
    LDA >erasesector,X
    STA <1,X
    DEX
    DEX
    BPL transfererasecode    
    ; use 8-bit accu/memory access for call
    SEP #$20
    LONGA OFF
    ; execute erase code from RAM
    JSL executefromstackframe  
    ; revert to 16 bit
    REP #$20
    LONGA ON
    ; transfer return address to where it is needed
    LDA <53
    STA <57
    LDA <54
    STA <58
    ; repair D, take down stack frame and return    
    LDA <51
    TCD
    TSC 
    CLC
    ADC #56
    TCS
    RTL
; This code needs to be mirrored to RAM before executing. 
; D must point to the stack frame as specified by eraseflash. 
; Register widths A/M 8 bit, X/Y 16 bit
; Invocation by far subroutine call
erasesector:
    LONGA OFF
    LDA #$AA                 ; 2
    STA >$805555             ; 4
    LDA #$55                 ; 2
    STA >$802AAA             ; 4
    LDA #$80                 ; 2
    STA >$805555             ; 4
    LDA #$AA                 ; 2
    STA >$805555             ; 4
    LDA #$55                 ; 2
    STA >$802AAA             ; 4
    LDA #$30                 ; 2
    STA [<56]                ; 2
waiterasestable:
    LDA [<56]                ; 2
    CMP [<56]                ; 2
    BNE waiterasestable      ; 2
    CMP [<56]                ; 2
    BNE waiterasestable      ; 2
    RTL                      ; 1
    LONGA ON                 ; 45 bytes total 


; ------------------ JUMP TO CODE IN STACK ------------------------------------
    ; Extremely tricky construction to jump to a code that is in the stack frame
    ; at the location D+1 (in bank 0). We prepare a 24-bit pointer
    ; on the stack to be jumped to with use of RTL. Due to a strange quirk in the 
    ; JSL/RTL implementation, the resultant address is actually one higher than what
    ; is taken from the stack, so we will hit D+1 perfectly.
    ; To work correctly, the accu/memory length must be set to 8 bit, and
    ; the accu will also be overwritten.
    LONGA OFF
executefromstackframe:
    LDA #0
    PHA
    PHD
    RTL
    LONGA ON              

    ENDS
    
; ----------------- Reset code ----------------------------    
RESET SECTION
    ; This is a very tricky startup code that needs to work with both
    ; the original 65c816 as well as the Bernd emulator.
   ORG $80FFF0
    ; The original CPU will come up in emulation mode which has a different
    ; memory map so it can access the ROM via bank 0. 
    ; In this case we must switch to a true ROM bank and turn off emulation.
ORIGINAL65C816:           ; 80FFF0
    JMP >TOHIGHBANK  
TOHIGHBANK:               ; 80FFF4
    CLC 
    XCE                
    REP #$30 
    ; Bernd emulation will not use the reset vectors, but will directly jump to this
    ; location with emulation already turned off and all registers in 16bit mode
BERND:                    ; 80FFF8
    JMP >~~softreset
    ; The reset vector for the 65C816
RESETVECTOR:              ; 80FFFC
    DW $FFF0
    DW 0                  ; padding
    
    ENDS 
    END
    