REM cmd.exe /c $(SolutionDir)\run-qemu.bat
@echo off
setlocal

set TRUNK=%~dp0

set TELNET=telnet.exe
set TELNETPORT=4445
set TELNETOPT=127.0.0.1 %TELNETPORT%

set GDBDIR=%TRUNK%\tpt\gdb
set GDB=%GDBDIR%\gdb.exe
set GDBPORT=4444
set QEMUPORT=1234
set GDBOPT=
set GDBOPT=%GDBOPT% -silent
set GDBOPT=%GDBOPT% -baud 33800
set GDBOPT=%GDBOPT% -ex "set debug remote 1"
set GDBOPT=%GDBOPT% -ex "target remote 127.0.0.1:%GDBPORT%"
rem set GDBOPT=%GDBOPT% -ex "target remote localhost:%QEMUPORT%"

set VMDIR=%TRUNK%\vm
set VM=%VMDIR%\floppy.ima

set QEMUDIR=%TRUNK%\tpt\qemu\11.0
set QEMU=%QEMUDIR%\qemu.exe
set QEMUOPT=
set QEMUOPT=%QEMUOPT% -L %QEMUDIR%\bios
set QEMUOPT=%QEMUOPT% -fda %VM%
rem -- COM1
rem set QEMUOPT=%QEMUOPT% -serial file:%TRUNK%\genos_serial.txt
set QEMUOPT=%QEMUOPT% -serial tcp:127.0.0.1:%GDBPORT%,server
rem -- COM2
set QEMUOPT=%QEMUOPT% -serial tcp:127.0.0.1:%TELNETPORT%,server


cd %QEMUDIR%

echo %GDB% %GDBOPT%
start %GDB% %GDBOPT%

echo %TELNET% %TELNETOPT%
start %TELNET% %TELNETOPT%

echo %QEMU% %QEMUOPT%
%QEMU% %QEMUOPT%


pause
