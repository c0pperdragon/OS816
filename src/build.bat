WDC816AS startup.asm -O..\bin\startup.obj
WDC816AS portio.asm
WDC816AS sleep.asm
WDC816AS uart.asm
del ..\bin\OS816.lib
WDCLIB -A ..\bin\OS816.lib portio.obj sleep.obj uart.obj
del *.obj
