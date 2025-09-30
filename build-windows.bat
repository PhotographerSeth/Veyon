@echo off
REM Veyon Chat Plugin - Windows Build Script
REM This script automates the compilation of the Veyon Chat Plugin

echo ========================================
echo Veyon Chat Plugin Build Script
echo ========================================
echo.

REM Check for Visual Studio installation
where cl >nul 2>&1
if %errorLevel% neq 0 (
    echo Setting up Visual Studio environment...
    if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else (
        echo ERROR: Visual Studio not found or not properly installed.
        echo Please install Visual Studio 2019 or 2022 with C++ development tools.
        pause
        exit /b 1
    )
)

REM Check for Qt installation
set QT_FOUND=0
if exist "C:\Qt\5.15.2\msvc2019_64\bin\qmake.exe" (
    set Qt5_DIR=C:\Qt\5.15.2\msvc2019_64\lib\cmake\Qt5
    set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%
    set QT_FOUND=1
    echo Found Qt 5.15.2 at C:\Qt\5.15.2\msvc2019_64
) else if exist "C:\Qt\6.0.0\msvc2019_64\bin\qmake.exe" (
    set Qt5_DIR=C:\Qt\6.0.0\msvc2019_64\lib\cmake\Qt6
    set PATH=C:\Qt\6.0.0\msvc2019_64\bin;%PATH%
    set QT_FOUND=1
    echo Found Qt 6.0.0 at C:\Qt\6.0.0\msvc2019_64
) else (
    echo ERROR: Qt installation not found.
    echo Please install Qt 5.15 or later from https://www.qt.io
    echo Expected locations:
    echo   C:\Qt\5.15.2\msvc2019_64\
    echo   C:\Qt\6.0.0\msvc2019_64\
    pause
    exit /b 1
)

REM Check for CMake
where cmake >nul 2>&1
if %errorLevel% neq 0 (
    echo ERROR: CMake not found.
    echo Please install CMake from https://cmake.org and add it to PATH.
    pause
    exit /b 1
)

echo Found CMake: 
cmake --version | findstr /C:"cmake version"

REM Check for source files
if not exist "CMakeLists.txt" (
    echo ERROR: CMakeLists.txt not found.
    echo Please ensure you're running this script from the plugin source directory.
    echo Current directory: %cd%
    pause
    exit /b 1
)

if not exist "src\ChatFeaturePlugin.h" (
    echo ERROR: Plugin source files not found.
    echo Please ensure all source files are present in the src\ directory.
    pause
    exit /b 1
)

echo All dependencies found. Starting build...
echo.

REM Create build directory
if exist build (
    echo Cleaning previous build...
    rmdir /s /q build
)
mkdir build
cd build

REM Determine Visual Studio version
set VS_GENERATOR="Visual Studio 16 2019"
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set VS_GENERATOR="Visual Studio 17 2022"
    echo Using Visual Studio 2022
) else (
    echo Using Visual Studio 2019
)

REM Configure the build
echo Configuring build with CMake...
cmake -G %VS_GENERATOR% -A x64 -DQt5_DIR="%Qt5_DIR%" ..
if %errorLevel% neq 0 (
    echo ERROR: CMake configuration failed.
    echo This might be due to missing Veyon development headers.
    echo Please ensure Veyon SDK is installed or Veyon was built from source.
    pause
    exit /b 1
)

echo Configuration successful!
echo.

REM Build the project
echo Building plugin (Release mode)...
cmake --build . --config Release --parallel
if %errorLevel% neq 0 (
    echo ERROR: Build failed.
    echo Check the error messages above for details.
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build Completed Successfully!
echo ========================================
echo.

REM Check if DLL was created
if exist "Release\veyon-chat-plugin.dll" (
    echo Plugin DLL created: %cd%\Release\veyon-chat-plugin.dll
    echo File size: 
    dir "Release\veyon-chat-plugin.dll" | findstr "veyon-chat-plugin.dll"
    echo.
    echo The plugin is ready for installation!
    echo.
    echo Next steps:
    echo 1. Copy veyon-chat-plugin.dll to your installation directory
    echo 2. Run install-windows.bat as Administrator
    echo.
    
    REM Offer to copy DLL to parent directory for easy access
    set /p COPY_DLL="Copy DLL to parent directory for easy installation? (y/N): "
    if /i "%COPY_DLL%"=="y" (
        copy "Release\veyon-chat-plugin.dll" "..\veyon-chat-plugin.dll"
        echo DLL copied to: %cd%\..\veyon-chat-plugin.dll
    )
) else (
    echo ERROR: Plugin DLL was not created.
    echo Build may have completed but DLL is missing.
    echo Check the build output above for errors.
)

echo.
pause
