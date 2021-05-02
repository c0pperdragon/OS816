WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src hello.c
WDCLN -HIE -Areset=80FFF8,00FFF8 -D000000,00000 -U, -C801000,01000 -K, hello.obj ..\..\bin\startup.obj -l..\..\bin\os816

