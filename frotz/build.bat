del frotz.lib
FOR %%G IN (src\*.obj) DO WDCLIB -A frotz.lib %%G

WDC816AS romfiles.asm

WDCLN -HIE -ZCODE -D0,850000 -C851000  -O frotz.hex ..\bin\startup.obj romfiles.obj -lfrotz -l..\bin\os816 -lml -lcl  

del frotz.lib
del romfiles.obj
