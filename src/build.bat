del ..\bin\*.lib
del ..\bin\*.obj

WDC816AS startup.asm -O..\bin\startup.obj
WDC816AS sleep.asm
WDC816AS serial.asm
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP portio.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP fcntl.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP time.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP burstmemcpy.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP romfile.c
WDCLIB -A ..\bin\os816.lib sleep.obj serial.obj portio.obj fcntl.obj time.obj burstmemcpy.obj romfile.obj

WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP malloc.c
WDCLIB -A ..\bin\malloc.lib malloc.obj

del *.obj
