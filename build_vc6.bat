@echo off
rem =====================================================================
rem  Reproducible VC6 build of curl1c.dll (1C 7.7 external component).
rem
rem  Why this exists: the .cbp targets a PSDK2003 + Intel C++ toolchain
rem  that is not present. This script builds with a portable VC6 instead:
rem   - VC98 headers substitute for PSDK2003 (winsock2/ws2tcpip/MFC present);
rem   - curl1c/intel_stub.c covers the few Intel runtime helpers the
rem     icl9-built dependency libs reference (forwarded to the CRT);
rem   - a modern kernel32.lib supplies GetModuleHandleExW/ConvertFiberToThread
rem     (missing from the old VC98 kernel32.lib).
rem  Output: Release\curl1c.dll. See memory note curl1c-build-recipe.
rem
rem  Adjust these two paths for your machine if they move:
set MSVC=D:\B6E3~1\1236A~1\SQLite\tools\MSVC600
set WINK32=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x86\kernel32.Lib
rem =====================================================================
setlocal
rem %~sdp0 = short (8.3, ASCII) path of this script's dir -> avoids Cyrillic
set ROOT=%~sdp0
if "%ROOT:~-1%"=="\" set ROOT=%ROOT:~0,-1%
set SRC=%ROOT%\curl1c
set OUT=%ROOT%\Build\curl1c
if not exist "%OUT%" mkdir "%OUT%"

set PATH=%MSVC%\VC98\Bin;%MSVC%\Common\MSDev98\Bin;%PATH%
set INCLUDE=%MSVC%\VC98\MFC\Include;%MSVC%\VC98\Include;%MSVC%\VC98\ATL\Include;%ROOT%\curl-8.6.0\include\curl
set LIB=%MSVC%\VC98\MFC\Lib;%MSVC%\VC98\Lib;%ROOT%\Release\icl9

set DEFS=/DWIN32 /DNDEBUG /D_WINDOWS /D_WINDLL /D_AFXDLL /D_MBCS /D_AFXEXT /DUSE_SYNC_DNS /DHAVE_GZIP /DHAVE_LIBZ /DHAVE_ZLIB_H /DHAVE_BROTLI /D_WSPIAPI_COUNTOF /DBUILDING_LIBCURL /DCURL_STATICLIB /DUSE_OPENSSL
set CFLAGS=/nologo /c /MD /O1 /W0 /GX /Oy /Gy %DEFS%

cd /d "%OUT%"
echo === RC ===
rc /l 0x419 /d NDEBUG /d _AFXDLL /fo 1ScURL.res "%SRC%\1ScURL.rc"        || goto :fail
echo === CL ===
cl %CFLAGS% "%SRC%\StdAfx.cpp"                                           || goto :fail
cl %CFLAGS% "%SRC%\Common.cpp"                                           || goto :fail
cl %CFLAGS% "%SRC%\Context.cpp"                                          || goto :fail
cl %CFLAGS% "%SRC%\1ScURL.cpp"                                           || goto :fail
cl %CFLAGS% "%SRC%\intel_stub.c"                                         || goto :fail

echo === LINK ===
rem link from curl1c so #pragma comment(lib,"../_1Common/...") resolves
cd /d "%SRC%"
link /nologo /dll /NODEFAULTLIB:LIBCMT "/OPT:REF,ICF,NOWIN98" /INCREMENTAL:NO ^
  /NODEFAULTLIB:libmmd /NODEFAULTLIB:libirc /NODEFAULTLIB:libdecimal ^
  /NODEFAULTLIB:svml_dispmd /NODEFAULTLIB:svml_disp /NODEFAULTLIB:libircmt ^
  /def:"%SRC%\1ScURL.def" /out:"%ROOT%\Release\curl1c.dll" ^
  "%OUT%\StdAfx.obj" "%OUT%\Common.obj" "%OUT%\Context.obj" "%OUT%\1ScURL.obj" ^
  "%OUT%\intel_stub.obj" "%OUT%\1ScURL.res" ^
  libcurl.lib zlib.lib brotli.lib libssh2.lib libcrypto.lib libssl.lib ^
  WS2_32.lib Crypt32.lib "%WINK32%"                                      || goto :fail

echo === DONE: Release\curl1c.dll ===
goto :eof
:fail
echo *** BUILD FAILED (errorlevel %errorlevel%) ***
exit /b 1
