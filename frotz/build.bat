FOR %%G IN (src\*.obj) DO WDCLIB -A frotz.lib %%G

WDCLN -HIE -ZCODE -D0,810004 -C811000 -O frotz.hex ..\bin\os816.obj -lfrotz -lml -l..\bin\cl816
py ..\tools\addfiles.py frotz.hex games/zork1.z3 games/wishbringer.z3 

del frotz.lib

