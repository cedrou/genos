@echo off
setlocal

set TRUNK=%~dp0
set BIN=%TRUNK%\bin
set SRC=%TRUNK%\src
set TPT=%TRUNK%\tpt
set VFD=%TPT%\vfd
set G4D=%TPT%\grub4dos
set VM=%TRUNK%\vm

rem Install the VFD driver
rem ---------------------------------------
%VFD%\vfd install /a

rem Create and mount a new floppy image
rem ---------------------------------------
%VFD%\vfd open 0: %VM%\floppy.ima /144 /W /NEW /F
%VFD%\vfd format 0:

rem Copy distrib files
rem ---------------------------------------
copy /b %G4D%\grldr A:\
copy /b %SRC%\grub\menu.lst A:\
copy /b %BIN%\kldr.exe A:\
copy /b %BIN%\kernel.exe A:\
copy /b %BIN%\kernel.pdb A:\

rem Unmount the image
rem ---------------------------------------
%VFD%\vfd close 0:

rem Install the bootloader
rem ---------------------------------------
%G4D%\bootlace --floppy %VM%\floppy.ima

