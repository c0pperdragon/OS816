WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src primes.c
WDC816CC -A -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src primes.c
WDCLN -HIE -Areset=80FFF8,00FFF8 -U10000, -C801000,01000 -K, primes.obj ..\..\bin\startup.obj -l..\..\bin\os816

