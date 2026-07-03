@echo off

rem Root of Visual C++ installed files.
set MSVC_ROOT=e:\b\Compillers\MSVC98
echo MSVC Root: %MSVC_ROOT%

rem Root of Visual Developer Studio installed files.
set MSVC_BIN=%MSVC_ROOT%\bin
echo MSVC BIN: %MSVC_BIN%

set PSDK=e:\b\Compillers\PSDK2003
echo PSDK2003 Root: %PSDK%

set MSVC98=e:\b\Compillers\MSVC98
echo MSVC98 Root: %MSVC98%

set ICL_VER=11
set ICL=e:\b\Compillers\Intel%ICL_VER%
set ICL_BIN=%ICL%\bin
echo ICL Root: %ICL%

rem e:\b\Compillers\Intel10\Bin\iclvars.bat 

set PERL_BIN=e:\b\Compillers\Perl\perl\bin
set PERL=%PERL_BIN%\perl.exe
echo Root of PERL: %PERL%

set PATH=%PERL_BIN%;%MSVC_BIN%;%ICL_BIN%;%PATH% 

rem set INCLUDE=%PSDK2003%\Include;%PSDK2003%\INCLUDE\CRT;%PSDK2003%\INCLUDE\MFC;%PSDK2003%\INCLUDE\ATL
set INCLUDE=%PSDK%\Include;%PSDK%\INCLUDE\CRT;%PSDK%\INCLUDE\MFC;%PSDK%\INCLUDE\ATL
set LIB=%ICL%\Lib;%MSVC98%\Lib
echo INCLUDE = %INCLUDE%

set CC=%ICL_BIN%\icl.exe

copy /Y .\win32\Makefile.msc .\Makefile.msc

nmake -f Makefile.msc clean
nmake -f Makefile.msc

echo End compiling
pause

