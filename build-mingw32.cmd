@echo off

REM Install Mingw32-GCC (x64 for 64-bit OS) and make sure it's on the PATH

@echo on
%~d0
cd %~dp0
md Release 2>nul
gcc -O3 -s -mwindows -o Release/proxylock proxylock.c
