@echo off

rem Root of Visual C++ installed files.
set MSVC_ROOT=e:\b\Compillers\MSVC98
echo MSVC Root: %MSVC_ROOT%

rem Root of Visual Developer Studio installed files.
set MSVC_BIN=%MSVC_ROOT%\bin
echo MSVC BIN: %MSVC_BIN%

set PSDK=e:\b\Compillers\PSDK2003
echo PSDK2003 Root: %PSDK%

set ICL_VER=12
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
set LIB=%ICL%\Lib
echo INCLUDE = %INCLUDE%

set CC=%ICL_BIN%\icl.exe
set CFLAGS=/Os /MD
rem %PERL% Configure 386 -D_WIN32_WINNT=0x0501 VC-WIN32 --release -static no-shared no-asm no-hw no-egd no-blake2 no-ec no-siphash --api=1.0.0
%PERL% Configure 386 -D_WIN32_WINNT=0x0501 VC-WIN32 --release -static no-shared no-threads no-asm enable-ssl2 enable-ssl3
rem --api=1.0.0

rem cd ms
rem do_nt.bat
echo Press key for start compiling
pause

nmake clean > nul
nmake /nologo -f makefile  build_libs
rem nmake -f makefile

echo End compiling
pause

rem crypto\aes\aes_local.h 
rem //typedef unsigned long long u64;
rem typedef unsigned __int64 u64;
rem crypto\sha\keccak1600.c 
rem ������ ULL

rem Since version 1.1.0 OpenSSL have changed their library names from: libeay32.dll -> libcrypto.dll ssleay32.dll -> libssl.dll