
del frotz.lib
FOR %%G IN (src\*.obj) DO WDCLIB -A frotz.lib %%G

WDC816AS files.asm

WDCLN -HIE -D0,800000 -ZCODE -C810000 -O frotz.hex ..\bin\startup.obj files.obj -lfrotz -l..\bin\malloc -lml -lcl -l..\bin\os816 

del frotz.lib
del files.obj
