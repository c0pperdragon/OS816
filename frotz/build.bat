del frotz.lib
FOR %%G IN (src\*.obj) DO WDCLIB -A frotz.lib %%G

WDC816AS romfiles.asm

WDCLN -HIE -ZCODE -D0,800000 -K808000 -C860000 -O frotz.hex romfiles.obj ..\bin\os816.obj -lfrotz -l..\bin\fixes -lml -lcl -l..\bin\bios

del frotz.lib
del romfiles.obj
