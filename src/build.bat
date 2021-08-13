del ..\bin\*.lib
del ..\bin\*.obj

WDC816AS os816.asm -O..\bin\os816.obj

WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP fixes.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP malloc.c
WDCLIB -A ..\bin\fixes.lib fixes.obj malloc.obj

WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP fcntl.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP time.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP romfile.c
WDCLIB -A ..\bin\bios.lib fcntl.obj time.obj romfile.obj

del *.obj
