WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\src cli.c
WDC816CC -A -ML -MV -MU -MK -MT -SOP -WL -WP -I..\src cli.c
WDCLN -HIE -Areset=80FFF8,0FFF8 -D0,F000 -C800000,0 cli.obj ..\bin\startup.obj -l..\bin\core -lml -lcl 
