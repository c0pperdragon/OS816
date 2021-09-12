del ..\bin\*.obj
del ..\bin\*.lib
del ..\bin\*.hex

WDC816AS boot.asm
WDC816CC -ML -MV -MU -MK -MT -SO0S -WL -WP monitor.c
WDCLN -HIE -C80F500 boot.obj monitor.obj -O ..\bin\boot.hex

WDC816AS os816.asm -O..\bin\os816.obj

WDC816CC -ML -MV -MU -MK -MT -SO0S -WL -WP fixes.c
WDC816CC -ML -MV -MU -MK -MT -SO0S -WL -WP malloc.c
WDC816CC -ML -MV -MU -MK -MT -SO0S -WL -WP fcntl.c
WDC816CC -ML -MV -MU -MK -MT -SO0S -WL -WP time.c
WDC816CC -ML -MV -MU -MK -MT -SO0S -WL -WP romfile.c

COPY %WDC_LIB%\cl.lib ..\bin\cl816.lib
py ..\tools\erasesymbols.py ..\bin\cl816.lib malloc free calloc realloc strdup fread memcpy
WDCLIB -A ..\bin\cl816.lib fixes.obj malloc.obj fcntl.obj romfile.obj

del *.obj
