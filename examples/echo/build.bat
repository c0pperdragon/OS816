WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src echo.c
WDCLN -HIE -Areset=80FFF8,0FFF8 -D0,FF00 -C800000,0 echo.obj ..\..\bin\startup.obj -l..\..\bin\core
WDCLN -HB -Aheader=10000,0 -C10030,30 echo.obj ..\..\bin\os816.obj

