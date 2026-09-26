@echo off
setlocal
set SCRIPT_DIR=%~dp0
cd /d "%SCRIPT_DIR%"

if exist "%SCRIPT_DIR%build\AvastSmartCleaner.exe" (
    echo Launching Secure Shield Cleaner...
    start "" "%SCRIPT_DIR%build\AvastSmartCleaner.exe"
    exit /b 0
)

if exist "%SCRIPT_DIR%build\Release\AvastSmartCleaner.exe" (
    echo Launching Secure Shield Cleaner...
    start "" "%SCRIPT_DIR%build\Release\AvastSmartCleaner.exe"
    exit /b 0
)

echo Desktop app has not been built yet. Run the PowerShell launcher or build with CMake first.
exit /b 1
