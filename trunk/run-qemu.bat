@echo off
setlocal

set TRUNK=%~dp0
set QEMU=%TRUNK%\tpt\qemu
set VM=%TRUNK%\vm


%QEMU%\qemu.exe -L %QEMU%\ -fda %VM%\floppy.ima

rem pause
