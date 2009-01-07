//------------------------------------------------------------------------------
// kernel.cpp
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
#include "pagemanager.h"
#include "keyboard.h"

using namespace GenOS;



#pragma pack(push,1)

struct MemoryMap
{
  uint32 Size;
  uint32 BaseAddrLow;
  uint32 BaseAddrHigh;
  uint32 LengthLow;
  uint32 LengthHigh;
  uint32 Type;
};

struct MultibootInfo
{
  uint32      Flags;
  uint32      MemoryLower;
  uint32      MemoryUpper;
  uint8       BootDevice[4];
  const char* Cmdline;
  uint32      ModulesCount;
  intptr      Modules;
  uint32      Unused[4];
  uint32      MmapLength;
  MemoryMap*  Mmap;
};

#pragma pack(pop)


Kernel::Kernel(uint32 kernel_size, paddr physical_base, uint32 physical_size)
: _size(kernel_size), _physical_base(physical_base), _physical_size(physical_size)
{
}

void Kernel::Run()
{
  Screen::Initialize();

  Screen::cout << "Starting GenOS" << Screen::endl; 

  Screen::cout << "  - Initializing segments..." << Screen::endl;
  GDT::Initialize();

  Screen::cout << "  - Initializing interrupts..." << Screen::endl; 
  InterruptManager::Initialize();

  Screen::cout << "  - Initializing frame manager..." << Screen::endl; 
  FrameManager::Initialize(_physical_base, _physical_size);

  Screen::cout << "  - Initializing page manager..." << Screen::endl; 
  PageManager::Initialize();

  Screen::cout << "  - Initializing keyboard..." << Screen::endl; 
	Keyboard::Initialize(); 

  Screen::cout << "  - Initializing timer..." << Screen::endl; 
	Timer::Initialize(50); 

  __asm sti;

  Screen::cout << "  - Entering idle loop..." << Screen::endl; 
  Idle();
}


void Kernel::Panic(const char* message, const char* file, uint32 line)
{
  __asm cli;

  Screen::cout << "******** PANIC (" << message << ") ********" << Screen::endl; 
  Screen::cout << "at " << file << ":" << line << Screen::endl;

  Hang();
}

void Kernel::Assert(const char* message, const char* file, uint32 line)
{
  __asm cli;

  Screen::cout << "******** ASSERTION FAILED ********" << Screen::endl; 
  Screen::cout << "at " << file << ":" << line << Screen::endl;
  Screen::cout << message << Screen::endl; 

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