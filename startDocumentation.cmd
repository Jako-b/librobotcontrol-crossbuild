@ECHO OFF

REM using batch parameters: 
REM https://www.microsoft.com/resources/documentation/windows/xp/all/proddocs/en-us/percent.mspx
REM https://stackoverflow.com/questions/112055/what-does-d0-mean-in-a-windows-batch-file

REM Setting the environment variable to absolute path of the installation
set INSTALL_DIR=%~d0%~p0

REM OpenOffice Folder
set DOC_DIR=%INSTALL_DIR%POSIX.1-2008

REM Switch to installation drive
%~d0

REM Storing the current directory in the stack
PUSHD . 

REM Switch to bin folder
cd %DOC_DIR%

REM Start Edge in Kiosk Mode
"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe" --no-first-run --kiosk --app=file:///%DOC_DIR%/index.html --edge-kiosk-type=public-browsing

REM Restoring to the original folder
POPD
