@echo off
rem Compilation...

if exist qvga.pio.h del qvga.pio.h
..\_exe\pioasm.exe -o c-sdk ..\_qvga\qvga.pio qvga.pio.h
if exist qvga.pio.h goto start
pause
goto stop

:start
call ..\_c1.bat

:stop
