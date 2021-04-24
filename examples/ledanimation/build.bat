WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src ledanimation.c
WDCLN -HIE -Areset=80FFF8,0FFF8 -D10000,00000 -U, -C810000,10000 -K, ledanimation.obj ..\..\bin\startup.obj -l..\..\bin\os816

