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


Kernel::Kernel(const intptr kernel_start, const intptr kernel_end, const uint32 mbinfo)
: _start(kernel_start), _end(kernel_end), _mbinfo(mbinfo)
{
}

void Kernel::Run()
{
  MultibootInfo* mbi = (MultibootInfo*)_mbinfo;

  Screen::Initialize();

  Screen::cout << "Starting GenOS" << Screen::endl; 

#if 1
  Screen::cout << "Available memory 0x" << (uint32)mbi->MemoryUpper << " KiB" << Screen::endl;
  Screen::cout << "Kernel 0x" << _start << "-0x" << _end << Screen::endl;
#endif

#if 0
  Screen::cout << " Flags         " << mbi->Flags << Screen::endl;         
  Screen::cout << " MemoryLower   " << mbi->MemoryLower << Screen::endl; 
  Screen::cout << " MemoryUpper   " << mbi->MemoryUpper << Screen::endl; 
  Screen::cout << " BootDevice    " << mbi->BootDevice[0] << mbi->BootDevice[1] << mbi->BootDevice[2] << mbi->BootDevice[3] << Screen::endl; 
  Screen::cout << " Cmdline       " << mbi->Cmdline << Screen::endl;
  Screen::cout << " ModulesCount  " << mbi->ModulesCount << Screen::endl; 
  Screen::cout << " Modules       " << mbi->Modules << Screen::endl; 
  Screen::cout << " Unused[4]     " << mbi->Unused[0] << mbi->Unused[1] << mbi->Unused[2] << mbi->Unused[3] << Screen::endl; 
  Screen::cout << " MmapLength    " << mbi->MmapLength << Screen::endl; 
  Screen::cout << " Mmap          " << mbi->Mmap << Screen::endl; 
#endif

#if 0
  if ((mbi->Flags & (1<<6)) && mbi->MmapLength)
  {
    Screen::cout << "Memory map (" << mbi->MmapLength << " bytes):" << Screen::endl;
    uint32 items = mbi->MmapLength / sizeof(MemoryMap);
    for (uint32 i=0; i<items; i++)
    {
      Screen::cout << " 0x" << mbi->Mmap[i].BaseAddrHigh << mbi->Mmap[i].BaseAddrLow;
      Screen::cout << " 0x"<< mbi->Mmap[i].LengthHigh << mbi->Mmap[i].LengthLow;
      switch(mbi->Mmap[i].Type)
      {
      case 1: Screen::cout << " Available" << Screen::endl; break;
      case 2: Screen::cout << " Reserved" << Screen::endl; break;
      case 3: Screen::cout << " ACPI Reclaim Memory" << Screen::endl; break;
      case 4: Screen::cout << " ACPI NVS Memory" << Screen::endl; break;
      }
    }
  }
#endif

  Screen::cout << "  - Initializing segments..." << Screen::endl;
  GDT::Initialize();

  Screen::cout << "  - Initializing interrupts..." << Screen::endl; 
  InterruptManager::Initialize();

  Screen::cout << "  - Initializing keyboard..." << Screen::endl; 
	Keyboard::Initialize(); 

  Screen::cout << "  - Initializing frame manager..." << Screen::endl; 
  FrameManager::Initialize(_end, mbi->MemoryUpper*1024 - ((uint32)_end - (uint32)_start));

  Screen::cout << "  - Initializing timer..." << Screen::endl; 
	Timer::Initialize(50); 

  __asm sti

  Screen::cout << "  - Entering idle loop..." << Screen::endl; 
  Idle();
}


void Kernel::Panic(const char* message)
{
  Screen::cout << "***********************" << Screen::endl; 
  Screen::cout << "******** PANIC ********" << Screen::endl; 
  Screen::cout << "***********************" << Screen::endl; 
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