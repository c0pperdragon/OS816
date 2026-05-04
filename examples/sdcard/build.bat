WDC816CC -ML -MV -MU -MK -MT -SO0S -WL -WP -I..\..\src sdcard.c
WDCLN -HIE -D0,C00004 -CC01000 sdcard.obj ..\..\bin\os816.obj -l..\..\bin\cl816
del *.obj
