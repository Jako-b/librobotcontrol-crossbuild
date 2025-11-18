@echo off
setlocal ENABLEDELAYEDEXPANSION

REM -----------------------------------------
REM USER CONFIG SECTION — MUST BE ADJUSTED
REM -----------------------------------------
set "TOOLCHAIN_DIR=C:/Users/pille/ECSW/arm-gnu-toolchain"
set "ECLIPSE_EXE=C:/Users/pille/ECSW/eclipse-cpp/eclipse.exe"

REM -----------------------------------------
REM Validate paths
REM -----------------------------------------
if not exist "%TOOLCHAIN_DIR%\bin\arm-none-linux-gnueabihf-gcc.exe" (
    echo ERROR: Toolchain not found:
    echo %TOOLCHAIN_DIR%
    pause
    exit /b 1
)

if not exist "%ECLIPSE_EXE%" (
    echo ERROR: Eclipse EXE not found:
    echo %ECLIPSE_EXE%
    pause
    exit /b 1
)

REM -----------------------------------------
REM Determine current folder
REM -----------------------------------------
set "SCRIPT_DIR=%~dp0"
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

echo SCRIPT_DIR detected as:
echo %SCRIPT_DIR%
echo.

REM -----------------------------------------
REM Build MSYS2 PATH manually (critical fix!)
REM Ensures: cygpath, dirname, readlink, bash tools available
REM -----------------------------------------
set "MSYS_PATH=C:\msys64\usr\bin"

if not exist "%MSYS_PATH%\bash.exe" (
    echo ERROR: MSYS2 not found at %MSYS_PATH%
    pause
    exit /b 1
)

REM -----------------------------------------
REM Launch bash with correct PATH and working directory
REM -----------------------------------------
C:\msys64\usr\bin\bash.exe --noprofile --norc -c ^
"export PATH=\"%MSYS_PATH%:$PATH\"; cd \"$(cygpath -u \"%SCRIPT_DIR%\")\"; ./startEclipse.sh \"%TOOLCHAIN_DIR%\" \"%ECLIPSE_EXE%\""

endlocal
exit /b 0