del ..\bin\*.lib
del ..\bin\*.obj

WDC816AS startup.asm -O..\bin\startup.obj
WDC816AS portio.asm
WDC816AS sleep.asm
WDC816AS serial.asm
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP console.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP string.c
WDCLIB -A ..\bin\core.lib portio.obj sleep.obj serial.obj console.obj string.obj

WDC816AS header.asm -O..\bin\header.obj
FOR /R "header\" %%F IN (*.asm) DO WDC816AS "%%F"
FOR /R "header\" %%F IN (*.obj) DO WDCLIB -A ..\bin\header.lib "%%F"

del *.obj
del header\*.obj
