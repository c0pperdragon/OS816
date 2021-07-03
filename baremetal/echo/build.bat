WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src echo.c
WDCLN -HIE -Areset=80FFF8,0FFF8 -C800000,0 -U0 echo.obj ..\..\bin\startup.obj -l..\..\bin\os816

