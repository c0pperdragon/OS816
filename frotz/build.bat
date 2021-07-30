
del frotz.lib
FOR %%G IN (src\*.obj) DO WDCLIB -A frotz.lib %%G

WDC816AS romfiles.asm

WDCLN -HIE -D0,800000 -ZCODE -C850000 -O frotz.hex ..\bin\startup.obj romfiles.obj -lfrotz -l..\bin\malloc -lml -lcl -l..\bin\os816 

del frotz.lib
del romfiles.obj
