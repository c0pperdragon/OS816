WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src data.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src io.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src main.c
WDCLN -HIE -D0,C00004 -CC01000 data.obj io.obj main.obj ..\..\bin\os816.obj -l..\..\bin\cl816 
del *.obj

