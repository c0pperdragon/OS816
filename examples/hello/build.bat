WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src hello.c
WDCLN -HIE -Areset=80FFF8,0FFF8 -D0,FF00 -C800000,0 hello.obj ..\..\bin\startup.obj -l..\..\bin\core
WDCLN -HB -Aheader=10000,0 -C10010,10 hello.obj ..\..\bin\header.obj -l..\..\bin\header
