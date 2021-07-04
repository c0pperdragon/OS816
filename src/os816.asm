
    XREF ~~main

; For an OS816 exectuable, this header section must be present directly at the start
; of the file. For executing, this file will be loaded to location $10000 and 
; after sanity check and patching of the jump tables it will be called.

HEADER SECTION
; This is the magic code in the first 4 bytes of any OS816 executable. 
    DB 79,83,8,16

; This is the entry point of the executable 
; The actual main method must have the usual signature: u16 argc, char** argv
    JMP >~~main

; This table is the interface from the exectuable to the OS. 
; After loading this is patched to point to the correct OS routines.
; It is intended that the collection of routines can change over time, so
; each routine is given a number that will never change. If such a routine
; is discontinued, the number will be left unused from then on. By this
; mechanism binary combatiblity can be preserved for all executables
; that do not actually use one of the discontinued routines.

    xdef ~~sleep
~~sleep:
    JMP >$000000
    xdef ~~portout
~~portout:
    JMP >$000001
    xdef ~~portin
~~portin:
    JMP >$000002
    xdef ~~send
~~send:
    JMP >$000003
    xdef ~~receive
~~receive:
    JMP >$000004
    xdef ~~print
~~print:
    JMP >$000005
    xdef ~~printu16
~~printu16:
    JMP >$000006
    xdef ~~readline
~~readline:
    JMP >$000007
    xdef ~~tokenize
~~tokenize:
    JMP >$000008
    xdef ~~strequal
~~strequal:
    JMP >$000009
    
    ENDS

