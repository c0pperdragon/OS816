
    XREF ~~print
    XREF ~~printu16
    
JUMP SECTION        ; must be placed at $800000 by the linker
    JMP >~~print         ; $800000
    JMP >~~printu16      ; $800004
    ENDS
    
    CODE
	xdef	~~jump_10000
~~jump_10000:
    JMP >$10000
    ENDS
    
    END 
