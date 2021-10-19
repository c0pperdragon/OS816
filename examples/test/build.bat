WDC816CC -ML -MV -MU -MK -MT -SO0S -WL -WP -I..\..\src test.c
WDCLN -HIE -D0,C00004 -CC01000 test.obj ..\..\bin\os816.obj -l..\..\bin\cl816
py ..\..\tools\addfiles.py test.hex y.txt x.txt
del *.obj
