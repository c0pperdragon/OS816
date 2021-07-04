WDC816CC -A -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src testint.c
WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src testint.c
WDCLN -HIE -Areset=80FFF8,0FFF8 -C800000,0 -U0 testint.obj ..\..\bin\startup.obj -l..\..\bin\os816

