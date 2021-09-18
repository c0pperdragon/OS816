WDC816CC -ML -MV -MU -MK -MT -SO0S -WL -WP -I..\..\src test.c
WDCLN -HIE -D0,800004 -C801000 test.obj ..\..\bin\os816.obj -l..\..\bin\cl816
py ..\..\tools\addfiles.py test.hex x.txt
del *.obj
