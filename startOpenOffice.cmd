@ECHO OFF

REM using batch parameters: 
REM https://www.microsoft.com/resources/documentation/windows/xp/all/proddocs/en-us/percent.mspx
REM https://stackoverflow.com/questions/112055/what-does-d0-mean-in-a-windows-batch-file

REM Setting the environment variable to absolute path of the installation
set INSTALL_DIR=%~d0%~p0

REM OpenOffice Folder
set OPENOFFICE_BINARY_DIR=%INSTALL_DIR%OpenOfficePortable

REM Switch to installation drive
%~d0

REM Storing the current directory in the stack
PUSHD . 

REM Switch to bin folder
cd %OPENOFFICE_BINARY_DIR%

REM Start OpenOffice Writer
start OpenOfficeWriterPortable.exe

REM Restoring to the original folder
POPD
