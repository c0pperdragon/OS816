WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src testint.c
WDCLN -HIE -D0,FF00 -C800000,0  testint.obj ..\..\bin\startup.obj -l..\..\bin\core

