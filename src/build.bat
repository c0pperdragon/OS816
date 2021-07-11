del ..\bin\*.lib
del ..\bin\*.obj

WDC816AS startup.asm -O..\bin\startup.obj
WDC816AS sleep.asm
WDC816AS serial.asm
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP portio.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP fctnl.c
WDCLIB -A ..\bin\core.lib sleep.obj serial.obj portio.obj fctnl.obj

del *.obj
