WDC816CC -ML -MV -MU -MK -MT -SO0S -WL -WP -I..\..\src test.c
WDC816AS romfiles.asm
WDCLN -HIE -D0,800000 -K808000 -C820000 test.obj romfiles.obj ..\..\bin\os816.obj -l..\..\bin\fixes -lcl -l..\..\bin\bios 
del *.obj
