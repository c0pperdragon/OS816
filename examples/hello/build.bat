WDC816CC -ML -MV -MU -MK -MT -SO0S -WL -WP -I..\..\src hello.c
WDCLN -HIE -D0,800000 -C810000 hello.obj ..\..\bin\startup.obj -l..\..\bin\os816 -lcl 
