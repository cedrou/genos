//------------------------------------------------------------------------------
// register.cpp
//	
//------------------------------------------------------------------------------
// Copyright (c) 2008, Cedric Rousseau
// All rights reserved.
// 
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

void Registers::Print()
{
  Screen::WriteString("DS  "); Screen::WriteHex(ds); Screen::WriteChar('\t');
  Screen::WriteString("EDI "); Screen::WriteHex(edi); Screen::WriteChar('\t');
  Screen::WriteString("ESI "); Screen::WriteHex(esi); Screen::WriteChar('\t');
  Screen::WriteString("EBP "); Screen::WriteHex(ebp); Screen::WriteChar('\t');
  Screen::WriteString("ESP "); Screen::WriteHex(esp); Screen::WriteChar('\n');
  Screen::WriteString("EBX "); Screen::WriteHex(ebx); Screen::WriteChar('\t');
  Screen::WriteString("EDX "); Screen::WriteHex(edx); Screen::WriteChar('\t');
  Screen::WriteString("ECX "); Screen::WriteHex(ecx); Screen::WriteChar('\t');
  Screen::WriteString("EAX "); Screen::WriteHex(eax); Screen::WriteChar('\t');
  Screen::WriteString("INT "); Screen::WriteHex(int_no); Screen::WriteChar('\n');
  Screen::WriteString("ERR "); Screen::WriteHex(err_code); Screen::WriteChar('\t');
  Screen::WriteString("EIP "); Screen::WriteHex(eip); Screen::WriteChar('\t');
  Screen::WriteString("CS  "); Screen::WriteHex(cs); Screen::WriteChar('\t');
  Screen::WriteString("EFL "); Screen::WriteHex(eflags); Screen::WriteChar('\t');
  Screen::WriteString("USP "); Screen::WriteHex(useresp); Screen::WriteChar('\n');
  Screen::WriteString("SS  "); Screen::WriteHex(ss); Screen::WriteChar('\n');
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

  Screen::WriteString("EAX "); Screen::WriteHex(regs.eax); Screen::WriteChar('\t');
  Screen::WriteString("EBX "); Screen::WriteHex(regs.ebx); Screen::WriteChar('\t');
  Screen::WriteString("ECX "); Screen::WriteHex(regs.ecx); Screen::WriteChar('\t');
  Screen::WriteString("EDX "); Screen::WriteHex(regs.edx); Screen::WriteChar('\n');
  Screen::WriteString("ESP "); Screen::WriteHex(regs.esp); Screen::WriteChar('\t');
  Screen::WriteString("EBP "); Screen::WriteHex(regs.ebp); Screen::WriteChar('\t');
  Screen::WriteString("EDI "); Screen::WriteHex(regs.edi); Screen::WriteChar('\t');
  Screen::WriteString("ESI "); Screen::WriteHex(regs.esi); Screen::WriteChar('\n');
  Screen::WriteString(" CS "); Screen::WriteHex(regs.cs); Screen::WriteChar('\t');
  Screen::WriteString(" DS "); Screen::WriteHex(regs.ds); Screen::WriteChar('\t');
  Screen::WriteString(" SS "); Screen::WriteHex(regs.ss); Screen::WriteChar('\n');
  Screen::WriteString(" ES "); Screen::WriteHex(regs.es); Screen::WriteChar('\t');
  Screen::WriteString(" FS "); Screen::WriteHex(regs.fs); Screen::WriteChar('\t');
  Screen::WriteString(" GS "); Screen::WriteHex(regs.gs); Screen::WriteChar('\n');
}

