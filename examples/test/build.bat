WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src test.c
WDCLN -HIE -D0,800000 -C810000 test.obj ..\..\bin\startup.obj -l..\..\bin\malloc -lcl -l..\..\bin\os816
del *.obj
