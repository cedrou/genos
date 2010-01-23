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

set BOCHSDIR="C:\Program Files\Bochs-2.4.1"
set BOCHS=%BOCHSDIR%\bochs.exe
set BOCHSOPT=
set BOCHSOPT=%BOCHSOPT% -q
set BOCHSOPT=%BOCHSOPT% -f %VMDIR%\Bochs\genos.bxrc
set BOCHSOPT=%BOCHSOPT% -log %VMDIR%\Bochs\log.txt
rem set BOCHSOPT=%BOCHSOPT% -dbglog %VMDIR%\Bochs\dbglog.txt

cd %VMDIR%\Bochs\

echo %GDB% %GDBOPT%
start %GDB% %GDBOPT%

echo %TELNET% %TELNETOPT%
start %TELNET% %TELNETOPT%

echo %BOCHS% %BOCHSOPT%
%BOCHS% %BOCHSOPT%


pause
