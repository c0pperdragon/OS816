cd src
del *.obj
FOR %%G IN (*.c) DO WDC816CC -ML -MV -MU -MK -MT -SO0S -WU -WW -I..\..\..\src %%G
cd ..

FOR %%G IN (src\*.obj) DO WDCLIB -A frotz.lib %%G

WDCLN -HIE -ZCODE -D0,C00004 -CC01000 -O frotz.hex ..\..\bin\os816.obj -lfrotz -lml -l..\..\bin\cl816
python ..\..\tools\addfiles.py frotz.hex games/zork1.z3 games/zork2.z3 games/zork3.z3

del frotz.lib
