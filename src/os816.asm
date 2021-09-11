
    ; external references
    XREF _ROM_BEG_DATA
    XREF _BEG_DATA
    XREF _END_DATA
    XREF _BEG_UDATA
    XREF _END_UDATA
    XREF ~~main
    
    ; linking to bootloader code
    xdef ~~softreset
    xdef ~~sleep
    xdef ~~send
    xdef ~~receive
	xdef ~~sendstr
    xdef ~~writeflash
    xdef ~~eraseflash
    xdef ~~topaddress_flash
    xdef ~~topaddress_ram
~~softreset        set $80F000
~~sleep            set $80F004
~~send             set $80F008
~~receive          set $80F00C
~~sendstr          set $80F010
~~writeflash       set $80F014
~~eraseflash       set $80F018
~~topaddress_flash set $80F01C
~~topaddress_ram   set $80F020

    ; start vector on fixed address        
LAUNCH SECTION
    ORG $810000
    JMP >startmain
    ENDS
        
    ; startup code already in CODE segment
    CODE
startmain:
    LONGA ON
    LONGI ON

    ; set D register to the bank of UDATA
    LDA #^_BEG_UDATA
    TCD

    ; clear UDATA segment
    LDA #_END_UDATA-_BEG_UDATA ; number of bytes to clear
    BEQ noclear
    LDX #<_BEG_UDATA  
    SEP #$20     ; for 8-bit memory access
    STZ <0,X     ; clear first byte
    REP #$20  
    DEA     
    BEQ noclear
    TXY   ; set up copy instruction for forward replication
    INY
    DEA  
    MVN #^_BEG_UDATA,#^_BEG_UDATA
noclear:

    ; Copy initial content into DATA segment. 
    ; Also the Data Bank Register will be correctly set to this bank  
    ; by the MVN instruction    
    LDA #_END_DATA-_BEG_DATA ;number of bytes to copy
    BEQ nocopy
    DEC A ;less one for MVN instruction
    LDX #<_ROM_BEG_DATA ;get source into X
    LDY #<_BEG_DATA ;get dest into Y
    MVN #^_ROM_BEG_DATA,#^_BEG_DATA ;copy bytes
nocopy:

    ; start the main function
    PEA #^argv
    PEA #<argv    
    PEA #1
    JSL >~~main
    
    ; do proper exit handling
    PHA
    JSL ~~_exit

progname:
    DB "OS816"
    DB 0
argv:
    DW #<progname
    DW #^progname
    DW 0
    DW 0
    ENDS 


    xdef ~~_exit    
    CODE
~~_exit:
; ------------------ RESTART or SHUTDOWN --------------------
; Let the machine restart or stop entirely 
; This depends on the exit-value. All negative values will cause a complete stop. 
    ; initial stack layout:  
    ;   SP+1, SP+2, SP+3    return address
    ;   SP+4, SP+5          return value from main() or exit()
    LDA <4,S
    BPL wantreset
    STP
wantreset:
    JMP >~~softreset    
    ENDS
    
    END
