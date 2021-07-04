del ..\bin\*.lib
del ..\bin\*.obj

WDC816AS startup.asm -O..\bin\startup.obj
WDC816AS portio.asm
WDC816AS sleep.asm
WDC816AS serial.asm
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP textio.c
WDC816CC -A -ML -MV -MU -MK -MT -SOP -WL -WP string.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP string.c
WDCLIB -A ..\bin\core.lib portio.obj sleep.obj serial.obj textio.obj string.obj

WDC816AS os816.asm -O..\bin\os816.obj

del *.obj
