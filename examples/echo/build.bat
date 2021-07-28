WDC816CC -ML -MV -MU -MK -MT -SOP -WL -WP -I..\..\src echo.c
WDCLN -HIE -D0,800000 -C810000 echo.obj ..\..\bin\startup.obj -lcl -l..\..\bin\os816
