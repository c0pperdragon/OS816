
ROMFILE SECTION
    ORG $810000
    DB 70,73,76,69   ; "FILE"
    DB 2,0,0,0       ; total file length
    DB 120,0         ; filename "x"    
    DB 97,98         ; filedata "ab"
    ENDS    
    END
