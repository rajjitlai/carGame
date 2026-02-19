@echo off
title Car Game – Build & Run
echo =============================================
echo   Car Game – Build Script
echo   Author: Rajjit Laishram
echo =============================================
echo.

REM ── Check if g++ is available ─────────────────────────────────────────────
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] g++ not found in PATH.
    echo.
    echo Please install MinGW-w64 and add it to your PATH:
    echo   https://www.mingw-w64.org/downloads/
    echo.
    echo Alternatively, open the project in Visual Studio and press Ctrl+F5.
    echo.
    pause
    exit /b 1
)

REM ── Compile ───────────────────────────────────────────────────────────────
echo [1/2] Compiling CarGame.cpp ...
g++ -std=c++17 -Wall -O2 -o CarGame.exe CarGame.cpp

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Build failed. See errors above.
    pause
    exit /b 1
)

echo [2/2] Build successful!
echo.

REM ── Launch ────────────────────────────────────────────────────────────────
echo Starting Car Game...
echo.
CarGame.exe

REM ── Keep window open after game exits ─────────────────────────────────────
echo.
echo Game exited. Press any key to close this window.
pause >nul
