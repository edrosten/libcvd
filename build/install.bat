rem @echo off

set headersdir=%1
set headersname=%2
set libs=%3
set bins=%4

rem ************************************************************************
rem * check environment variables

if not "%INCLUDEDIR%"=="" goto includedirset
echo The INCLUDEDIR environment variable must be set to point to a directory
echo to be able to perform the installation procedure.
exit

:includedirset
if exist "%INCLUDEDIR%\*.*" goto includedirexists
echo The INCLUDEDIR environment variable must point to an existing directory
echo to be able to perform the installation procedure.
exit

:includedirexists

if not "%LIBDIR%"=="" goto libdirset
echo The LIBDIR environment variable must be set to point to a directory
echo to be able to perform the installation procedure.
exit

:libdirset
if exist "%LIBDIR%\*.*" goto libdirexists
echo The LIBDIR environment variable must point to an existing directory
echo to be able to perform the installation procedure.
exit

:libdirexists
if not "%BINDIR%"=="" goto bindirset
echo The BINDIR environment variable must be set to point to a directory
echo to be able to perform the installation procedure.
exit

:bindirset
if exist "%BINDIR%\*.*" goto bindirexists
echo The BINDIR environment variable must point to an existing directory
echo to be able to perform the installation procedure.
exit

:bindirexists

echo All is well installing...

rem ************************************************************************
rem * check input variables and install if necessary

if "%headersdir%"=="" goto skipheaders
if "%headersname%"=="" goto skipheaders

xcopy /S /I /Y %headersdir% "%INCLUDEDIR%\%headersname%"

:skipheaders

if "%libs%"=="" goto skiplibs
xcopy /S /I /Y %libs% "%LIBDIR%"

:skiplibs

if "%bins%"=="" goto skipbins
xcopy /S /I /Y %bins% "%BINDIR%"

:skipbins

echo Installation finished.
