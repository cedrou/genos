//------------------------------------------------------------------------------
// register.cpp
//	
//------------------------------------------------------------------------------
// Copyright (c) 2008, Cedric Rousseau
// All rights reserved.
// 
// This source code is released under the new BSD License.
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions, and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions, and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include "registers.h"
#include "screen.h"

using namespace GenOS;

void Registers::Print() const
{
  Screen::cout << "DS  " << ds << "\t";
  Screen::cout << "EDI " << edi << "\t";
  Screen::cout << "ESI " << esi << "\t";
  Screen::cout << "EBP " << ebp << "\t";
  Screen::cout << "ESP " << esp << "\n";
  Screen::cout << "EBX " << ebx << "\t";
  Screen::cout << "EDX " << edx << "\t";
  Screen::cout << "ECX " << ecx << "\t";
  Screen::cout << "EAX " << eax << "\t";
  Screen::cout << "INT " << int_no << "\n";
  Screen::cout << "ERR " << err_code << "\t";
  Screen::cout << "EIP " << eip << "\t";
  Screen::cout << "CS  " << cs << "\t";
  Screen::cout << "EFL " << eflags << "\t";
  Screen::cout << "USP " << useresp << "\n";
  Screen::cout << "SS  " << ss << "\n";
}

void Registers::PrintRegs()
{
  struct
  {
      uint16 gs, fs, es, ds, cs, ss;
      uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
  } regs;
  __asm
  {
    mov regs.eax, eax
    mov regs.ebx, ebx
    mov regs.ecx, ecx
    mov regs.edx, edx
    mov regs.edi, edi
    mov regs.esi, esi
    mov regs.ebp, ebp
    mov regs.esp, esp
    mov word ptr regs.cs, cs
    mov word ptr regs.ds, ds
    mov word ptr regs.ss, ss
    mov word ptr regs.es, es
    mov word ptr regs.fs, fs
    mov word ptr regs.gs, gs
  }

  Screen::cout << "EAX " << regs.eax << "\t";
  Screen::cout << "EBX " << regs.ebx << "\t";
  Screen::cout << "ECX " << regs.ecx << "\t";
  Screen::cout << "EDX " << regs.edx << "\n";
  Screen::cout << "ESP " << regs.esp << "\t";
  Screen::cout << "EBP " << regs.ebp << "\t";
  Screen::cout << "EDI " << regs.edi << "\t";
  Screen::cout << "ESI " << regs.esi << "\n";
  Screen::cout << " CS " << regs.cs << "\t";
  Screen::cout << " DS " << regs.ds << "\t";
  Screen::cout << " SS " << regs.ss << "\n";
  Screen::cout << " ES " << regs.es << "\t";
  Screen::cout << " FS " << regs.fs << "\t";
  Screen::cout << " GS " << regs.gs << "\n";
}

uint32 Registers::CurrentEIP()
{
  _asm
  {
    mov eax, dword ptr [ebp+4];
  }
}
