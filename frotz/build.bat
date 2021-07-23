
del frotz.lib
FOR %%G IN (blorb\*.obj) DO WDCLIB -A frotz.lib %%G
FOR %%G IN (dumb\*.obj) DO WDCLIB -A frotz.lib %%G
FOR %%G IN (common\*.obj) DO WDCLIB -A frotz.lib %%G

WDCLN -HIE -D0,800000 -ZCODE -C810000 -O frotz.hex ..\bin\startup.obj -lfrotz -lml -lcl -l..\bin\os816 
