//------------------------------------------------------------------------------
// kernel.h
//	Kernel class
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

#include "kernel.h"
#include "screen.h"
#include "gdt.h"
#include "intmgr.h"
#include "timer.h"
#include "framemanager.h"

using namespace GenOS;

Kernel::Kernel(const intptr kernel_start, const intptr kernel_end, const size_t upper_memory_size)
: _start(kernel_start), _end(kernel_end), _upper_memory_size(upper_memory_size)
{
}

void Kernel::Run()
{
  Screen::Initialize();

  Screen::WriteString("Kernel loaded at address 0x");
  Screen::WriteHex(_start);
  Screen::WriteChar('\n');

  Screen::WriteString("Kernel ends at address 0x");
  Screen::WriteHex(_end);
  Screen::WriteChar('\n');

  Screen::WriteString("Available upper memory 0x");
  Screen::WriteHex(_upper_memory_size);
  Screen::WriteString(" bytes\n");

  Screen::WriteString("Starting GenOS\n"); 

  Screen::WriteString("  - Initializing segments...\n"); 
  GDT::Initialize();

  Screen::WriteString("  - Initializing interrupts...\n"); 
  InterruptManager::Initialize();

  Screen::WriteString("  - Initializing frame manager...\n"); 
	FrameManager::Initialize(_end, _upper_memory_size - ((uint32)_end - (uint32)_start));

  Screen::WriteString("  - Initializing timer...\n"); 
	Timer::Initialize(50); 

  Screen::WriteString("  - Entering idle loop...\n"); 
  __asm sti
  Idle();
}


void Kernel::Panic(const char* message)
{
  Screen::WriteString("***********************\n"); 
  Screen::WriteString("******** PANIC ********\n"); 
  Screen::WriteString("***********************\n"); 
  Screen::WriteString(message); 

  Hang();
}

void __declspec(naked) Kernel::Idle()
{
  __asm hlt
	__asm jmp Idle
}


void __declspec(naked) Kernel::Hang()
{
	__asm cli
  __asm hlt
}