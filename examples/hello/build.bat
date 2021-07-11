WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src hello.c
WDCLN -HIE -D0,F000 -C800000,0 hello.obj ..\..\bin\startup.obj -lcl -l..\..\bin\core 
