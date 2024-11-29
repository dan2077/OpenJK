@echo off
SETLOCAL EnableDelayedExpansion

:: Initialize configuration options with default values
SET "BuildPortableVersion=ON"
SET "BuildMPEngine=ON"
SET "BuildMPRdVanilla=ON"
SET "BuildMPRdVulkan=ON"
SET "BuildMPDed=ON"
SET "BuildMPGame=ON"
SET "BuildMPCGame=ON"
SET "BuildMPUI=ON"
SET "BuildDiscordRichPresence=ON"
SET "BuildTests=OFF"
SET "INSTALL_PATH="
SET "AdditionalCMakeOptions="

:: Check if CMake is installed
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo CMake not found. Please install CMake and add it to your PATH.
    pause
    exit /b
)

echo Found CMake!

:: Prompt user for Visual Studio version with default
echo Please select the Visual Studio version:
echo [1] Visual Studio 2015 (msvc14)
echo [2] Visual Studio 2017 (msvc15)
echo [3] Visual Studio 2019 (msvc16)
echo [4] Visual Studio 2022 (msvc17)
echo.

set /p VS_CHOICE="Enter your choice (1-4), or press Enter for the default [Visual Studio 2022 (msvc17)], or type 'EXIT' to quit: "
if /i "%VS_CHOICE%"=="EXIT" (
    exit /b
)
echo.

IF "%VS_CHOICE%"=="1" (
    SET "VS_VERSION=Visual Studio 14 2015"
    SET "VS_FOLDER=msvc14"
    echo You have selected Visual Studio 2015 ^(msvc14^).
) ELSE IF "%VS_CHOICE%"=="2" (
    SET "VS_VERSION=Visual Studio 15 2017"
    SET "VS_FOLDER=msvc15"
    echo You have selected Visual Studio 2017 ^(msvc15^).
) ELSE IF "%VS_CHOICE%"=="3" (
    SET "VS_VERSION=Visual Studio 16 2019"
    SET "VS_FOLDER=msvc16"
    echo You have selected Visual Studio 2019 ^(msvc16^).
) ELSE (
    SET "VS_VERSION=Visual Studio 17 2022"
    SET "VS_FOLDER=msvc17"
    echo You have selected Visual Studio 2022 ^(msvc17^).
)

:: Prompt user for architecture with default
echo.
echo Please select the target architecture:
echo [1] 32-bit (x86)
echo [2] 64-bit (x64)
echo.

set /p ARCH_CHOICE="Enter your choice (1-2), or press Enter for the default [32-bit (x86)], or type 'EXIT' to quit: "
if /i "%ARCH_CHOICE%"=="EXIT" (
    exit /b
)
echo.

IF "%ARCH_CHOICE%"=="2" (
    SET "ARCH=x64"
    SET "ARCH_FOLDER=x64"
    echo You have selected 64-bit ^(x64^).
) ELSE (
    SET "ARCH=Win32"
    SET "ARCH_FOLDER=x86"
    echo You have selected 32-bit ^(x86^).
)

:: Configuration options prompt
:CONFIG_OPTIONS
echo.
echo Configuration will proceed with default cmake build options.
echo Advanced users can enter 'C' to configure options.
set /p ADVANCED_CONFIG="Press Enter to continue with the default options, or type 'C' to configure cmake build options, or type 'EXIT' to quit: "
if /i "%ADVANCED_CONFIG%"=="EXIT" (
    exit /b
)
echo.

REM Set default installation path for both standard and advanced users
SET "INSTALL_PATH=install-%VS_FOLDER%_%ARCH_FOLDER%"

IF /I "%ADVANCED_CONFIG%"=="C" (
    GOTO CONFIG_MENU
) ELSE (
    GOTO CONFIG_DONE
)

:CONFIG_MENU
cls
echo Advanced CMake Configuration Menu
echo [1] Build portable version - !BuildPortableVersion!
echo [2] Build MP Engine - !BuildMPEngine!
echo [3] Build MP default renderer - !BuildMPRdVanilla!
echo [4] Build MP vulkan renderer - !BuildMPRdVulkan!
echo [5] Build MP dedicated server - !BuildMPDed!
echo [6] Build MP server-side gamecode - !BuildMPGame!
echo [7] Build MP clientside gamecode - !BuildMPCGame!
echo [8] Build MP UI code - !BuildMPUI!
echo [A] Build with Discord Rich Presence - !BuildDiscordRichPresence!
echo [B] Build automatic unit tests (requires Boost) - !BuildTests!
echo [C] Set custom install path - [!INSTALL_PATH!]
echo [D] Add custom CMake options - [!AdditionalCMakeOptions!]
echo [E] Done
echo [F] Quit
echo.
set /p CONFIG_CHOICE="Select an option to toggle or configure (1-9, A-E) or F when done, F to exit: "

IF "!CONFIG_CHOICE!"=="1" CALL :TOGGLE_OPTION BuildPortableVersion
IF "!CONFIG_CHOICE!"=="2" CALL :TOGGLE_OPTION BuildMPEngine
IF "!CONFIG_CHOICE!"=="3" CALL :TOGGLE_OPTION BuildMPRdVanilla
IF "!CONFIG_CHOICE!"=="4" CALL :TOGGLE_OPTION BuildMPRdVulkan
IF "!CONFIG_CHOICE!"=="5" CALL :TOGGLE_OPTION BuildMPDed
IF "!CONFIG_CHOICE!"=="6" CALL :TOGGLE_OPTION BuildMPGame
IF "!CONFIG_CHOICE!"=="7" CALL :TOGGLE_OPTION BuildMPCGame
IF "!CONFIG_CHOICE!"=="8" CALL :TOGGLE_OPTION BuildMPUI
IF "!CONFIG_CHOICE!"=="9" CALL :TOGGLE_OPTION BuildMPRend2
IF /I "!CONFIG_CHOICE!"=="A" CALL :TOGGLE_OPTION BuildDiscordRichPresence
IF /I "!CONFIG_CHOICE!"=="B" CALL :TOGGLE_OPTION BuildTests
IF /I "!CONFIG_CHOICE!"=="C" (
    echo Enter custom install path or leave empty for default:
    set /p INSTALL_PATH="Custom install path: "
    if "!INSTALL_PATH!"=="" SET "INSTALL_PATH=install-%VS_FOLDER%_%ARCH_FOLDER%"
    echo.
)
IF /I "!CONFIG_CHOICE!"=="D" (
    echo Enter additional CMake options or leave empty for none:
    set /p AdditionalCMakeOptions="Custom CMake options: "
    echo.
)
IF /I "!CONFIG_CHOICE!"=="E" GOTO CONFIG_DONE
IF /I "!CONFIG_CHOICE!"=="F" exit /b
GOTO CONFIG_MENU

:CONFIG_DONE
SET "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DBuildPortableVersion=!BuildPortableVersion!"
SET "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DBuildMPEngine=!BuildMPEngine!"
SET "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DBuildMPRdVanilla=!BuildMPRdVanilla!"
SET "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DBuildMPRdVulkan=!BuildMPRdVulkan!"
SET "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DBuildMPDed=!BuildMPDed!"
SET "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DBuildMPGame=!BuildMPGame!"
SET "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DBuildMPCGame=!BuildMPCGame!"
SET "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DBuildMPUI=!BuildMPUI!"
SET "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DBuildDiscordRichPresence=!BuildDiscordRichPresence!"
SET "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DBuildTests=!BuildTests!"

IF NOT "!INSTALL_PATH!"=="" (
    SET "CMAKE_OPTIONS=!CMAKE_OPTIONS! -DCMAKE_INSTALL_PREFIX=!INSTALL_PATH!"
)

IF NOT "!AdditionalCMakeOptions!"=="" (
    SET "CMAKE_OPTIONS=!CMAKE_OPTIONS! !AdditionalCMakeOptions!"
)

:: Generate the build folder name using the selected Visual Studio and Architecture
SET "BUILD_FOLDER=%VS_FOLDER%_%ARCH_FOLDER%"

:: Display the selected install path
echo Install path: !INSTALL_PATH!
echo.

:: Before configuring a new build, clear previous CMake configuration files if they exist.
IF EXIST "%BUILD_FOLDER%\CMakeCache.txt" DEL /F /Q "%BUILD_FOLDER%\CMakeCache.txt"
IF EXIST "%BUILD_FOLDER%\CMakeFiles" RMDIR /S /Q "%BUILD_FOLDER%\CMakeFiles"

:: Configure and generate CMake project
echo Configuring CMake for %VS_VERSION% %ARCH% with options: !CMAKE_OPTIONS!
cmake -G "%VS_VERSION%" -A %ARCH% !CMAKE_OPTIONS! -B "%BUILD_FOLDER%" -S ".." || (
    echo Failed to configure CMake project.
    pause
    exit /b
)

echo.
echo CMake project configured successfully in folder "%BUILD_FOLDER%".
pause
exit /b

:: Function to toggle configuration options
:TOGGLE_OPTION
SET "optionName=%1"
IF "!%optionName%!"=="ON" (
    SET "!optionName!=OFF"
) ELSE (
    SET "!optionName!=ON"
)
GOTO CONFIG_MENU