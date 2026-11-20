WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src data.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src io.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src main.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src evaluate.c
WDCLN -HIE -D0,C00004 -CC01000 main.obj data.obj io.obj evaluate.obj ..\..\bin\os816.obj -l..\..\bin\cl816 
del *.obj

