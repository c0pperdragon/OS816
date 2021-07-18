WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src test.c
WDCLN -HIE -D0,F000 -C800000,0  test.obj ..\..\bin\startup.obj -lcl -l..\..\bin\os816
del *.obj
