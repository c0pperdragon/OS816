WDC816AS startup.asm -O..\bin\startup.obj
WDC816AS portio.asm
WDC816AS sleep.asm
WDC816AS serial.asm
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP textio.c
WDC816CC -A -ML -MV -MU -MK -MT -SOP -WL -WP string.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP string.c
del ..\bin\OS816.lib
WDCLIB -A ..\bin\OS816.lib portio.obj sleep.obj serial.obj textio.obj string.obj
del *.obj
