@echo off
setlocal ENABLEDELAYEDEXPANSION

REM =========================================================
REM  USER CONFIGURATION - PLEASE ADJUST THESE PATHS!
REM =========================================================
REM Path to your ARM Toolchain (root folder containing bin/)
set "TOOLCHAIN_DIR=path\to\your\toolchain"

REM Path to your Eclipse executable (e.g. C:/eclipse/eclipse.exe)
set "ECLIPSE_EXE=path\to\your\eclipse.exe"

REM Example:
REM set "TOOLCHAIN_DIR=C:/Toolchains/arm-gnu-toolchain"
REM set "ECLIPSE_EXE=C:/Eclipse/eclipse-cpp/eclipse.exe"


REM =========================================================
REM  Validation Checks
REM =========================================================

if "%TOOLCHAIN_DIR%"=="" (
    echo [ERROR] TOOLCHAIN_DIR is not set!
    echo Please edit startEclipse.cmd and set the path to your ARM Toolchain.
    pause
    exit /b 1
)

if "%ECLIPSE_EXE%"=="" (
    echo [ERROR] ECLIPSE_EXE is not set!
    echo Please edit startEclipse.cmd and set the path to eclipse.exe.
    pause
    exit /b 1
)

if not exist "%TOOLCHAIN_DIR%\bin\arm-none-linux-gnueabihf-gcc.exe" (
    echo [ERROR] Toolchain not found at:
    echo %TOOLCHAIN_DIR%
    echo Please check the path. It should point to the folder containing 'bin'.
    pause
    exit /b 1
)

if not exist "%ECLIPSE_EXE%" (
    echo [ERROR] Eclipse EXE not found at:
    echo %ECLIPSE_EXE%
    pause
    exit /b 1
)

REM =========================================================
REM  Environment Setup
REM =========================================================
set "SCRIPT_DIR=%~dp0"
REM Remove trailing backslash if present
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

echo Detected Project Root: %SCRIPT_DIR%
echo.

REM Path to MSYS2 (adjust if installed in non-standard location)
set "MSYS_PATH=C:\msys64\usr\bin"

if not exist "%MSYS_PATH%\bash.exe" (
    echo [ERROR] MSYS2 bash.exe not found at %MSYS_PATH%
    echo Please install MSYS2 or update the path in this script.
    pause
    exit /b 1
)

REM =========================================================
REM  Launch Eclipse via MSYS2 Wrapper
REM =========================================================
echo Launching Eclipse in Cross-Compile Environment...
"%MSYS_PATH%\bash.exe" --noprofile --norc -c ^
"export PATH=\"%MSYS_PATH%:$PATH\"; cd \"$(cygpath -u \"%SCRIPT_DIR%\")\"; ./startEclipse.sh \"%TOOLCHAIN_DIR%\" \"%ECLIPSE_EXE%\""

endlocal
exit /b 0