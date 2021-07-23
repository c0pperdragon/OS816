del *.obj
FOR %%G IN (*.c) DO WDC816CC -ML -MV -MU -MK -MT -SOP -WU -WW -I..\..\src %%G

