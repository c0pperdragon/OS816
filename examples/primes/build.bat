WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src primes.c
WDCLN -HIE -D0,F000 -C800000,0 primes.obj ..\..\bin\startup.obj -lcl -l..\..\bin\core

