del *.obj
FOR %%G IN (*.c) DO WDC816CC -ML -MV -MU -MK -MT -SO0S -WU -WW -I..\..\src %%G

