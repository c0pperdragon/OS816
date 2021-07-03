WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\src cli.c
WDC816AS jumptables.asm
WDCLN -HIE -Areset=80FFF8,FFF8 -Ajump=800000,0 -C800010,10 -U0 cli.obj jumptables.obj ..\bin\startup.obj -l..\bin\os816
