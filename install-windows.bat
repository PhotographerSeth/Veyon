@echo off
REM Veyon Chat Plugin - Windows Installation Script
REM This script automates the installation of the Veyon Chat Plugin on Windows

echo ========================================
echo Veyon Chat Plugin Installation Script
echo ========================================
echo.

REM Check if running as administrator
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo ERROR: This script must be run as Administrator.
    echo Please right-click and select "Run as administrator"
    pause
    exit /b 1
)

echo Checking for Veyon installation...

REM Check for Veyon installation in common locations
set VEYON_PATH=""
if exist "C:\Program Files\Veyon\veyon-master.exe" (
    set VEYON_PATH=C:\Program Files\Veyon
    echo Found Veyon at: C:\Program Files\Veyon
) else if exist "C:\Program Files (x86)\Veyon\veyon-master.exe" (
    set VEYON_PATH=C:\Program Files (x86)\Veyon
    echo Found Veyon at: C:\Program Files (x86)\Veyon
) else (
    echo ERROR: Veyon installation not found.
    echo Please ensure Veyon 4.9.7 is installed before running this script.
    pause
    exit /b 1
)

REM Create plugins directory if it doesn't exist
set PLUGIN_DIR=%VEYON_PATH%\plugins
if not exist "%PLUGIN_DIR%" (
    echo Creating plugins directory...
    mkdir "%PLUGIN_DIR%"
)

REM Check if plugin DLL exists in current directory
if not exist "veyon-chat-plugin.dll" (
    echo ERROR: veyon-chat-plugin.dll not found in current directory.
    echo Please ensure the plugin DLL is in the same folder as this script.
    pause
    exit /b 1
)

echo Installing plugin...
copy "veyon-chat-plugin.dll" "%PLUGIN_DIR%\" >nul
if %errorLevel% neq 0 (
    echo ERROR: Failed to copy plugin file.
    echo Please check file permissions and try again.
    pause
    exit /b 1
)

echo Plugin installed successfully!
echo.

REM Stop and start Veyon Service
echo Restarting Veyon Service...
net stop VeyonService >nul 2>&1
if %errorLevel% equ 0 (
    echo Veyon Service stopped.
    timeout /t 2 >nul
    net start VeyonService >nul 2>&1
    if %errorLevel% equ 0 (
        echo Veyon Service started.
    ) else (
        echo WARNING: Failed to start Veyon Service. Please start it manually.
    )
) else (
    echo WARNING: Veyon Service not found or already stopped.
    echo Please restart the service manually if it's running.
)

echo.
echo ========================================
echo Installation Complete!
echo ========================================
echo.
echo The Veyon Chat Plugin has been installed successfully.
echo.
echo Next steps:
echo 1. Restart Veyon Master application
echo 2. Look for the Chat button in the toolbar
echo 3. Press F10 to open the chat window
echo 4. Install the plugin on all client computers
echo.
echo For detailed usage instructions, see the README.md file.
echo.
pause
