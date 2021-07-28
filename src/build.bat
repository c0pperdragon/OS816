del ..\bin\*.lib
del ..\bin\*.obj

WDC816AS startup.asm -O..\bin\startup.obj
WDC816AS sleep.asm
WDC816AS serial.asm
WDC816AS MVN.asm
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP portio.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP fctnl.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP time.c
WDCLIB -A ..\bin\os816.lib sleep.obj serial.obj portio.obj fctnl.obj time.obj MVN.obj

WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP malloc.c
WDCLIB -A ..\bin\malloc.lib malloc.obj

del *.obj
