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
#include "kheap.h"

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


Kernel::Kernel(KernelBootInfo* bootinfo)
: _bootinfo(bootinfo)
{
}

void Kernel::Run()
{
  Screen::Initialize();

  Screen::cout << "Starting GenOS" << Screen::endl; 

#if 0
  Screen::cout << "kernelSize: 0x" <<                  _bootinfo->kernelSize << Screen::endl;
  Screen::cout << "kernelPhysicalStart: 0x" <<         _bootinfo->kernelPhysicalStart << Screen::endl;
  Screen::cout << "kernelPhysicalEnd: 0x" <<           _bootinfo->kernelPhysicalEnd << Screen::endl;
  Screen::cout << "kernelVirtualStart: 0x" <<          _bootinfo->kernelVirtualStart << Screen::endl;
  Screen::cout << "stackSize: 0x" <<                   _bootinfo->stackSize << Screen::endl;
  Screen::cout << "stackPhysicalStart: 0x" <<          _bootinfo->stackPhysicalStart << Screen::endl;
  Screen::cout << "stackPhysicalEnd: 0x" <<            _bootinfo->stackPhysicalEnd << Screen::endl;
  Screen::cout << "stackVirtualStart: 0x" <<           _bootinfo->stackVirtualStart << Screen::endl;
  Screen::cout << "stackVirtualEnd: 0x" <<             _bootinfo->stackVirtualEnd << Screen::endl;
  Screen::cout << "frameManagerSize: 0x" <<            _bootinfo->frameManagerSize << Screen::endl;
  Screen::cout << "frameManagerPhysicalStart: 0x" <<   _bootinfo->frameManagerPhysicalStart << Screen::endl;
  Screen::cout << "frameManagerPhysicalEnd: 0x" <<     _bootinfo->frameManagerPhysicalEnd << Screen::endl;
  Screen::cout << "frameManagerVirtualStart: 0x" <<    _bootinfo->frameManagerVirtualStart << Screen::endl;
  Screen::cout << "frameManagerVirtualEnd: 0x" <<      _bootinfo->frameManagerVirtualEnd << Screen::endl;
  Screen::cout << "availableMemoryPhysicalBase: 0x" << _bootinfo->availableMemoryPhysicalBase << Screen::endl;
  Screen::cout << "availableMemorySize: 0x" <<         _bootinfo->availableMemorySize << Screen::endl;
  Screen::cout << "pageDirectory: 0x" <<               _bootinfo->pageDirectory << Screen::endl;
  Screen::cout << "pageTable0: 0x" <<                  _bootinfo->pageTable0 << Screen::endl;
  Screen::cout << "pageTable768: 0x" <<                _bootinfo->pageTable768 << Screen::endl;
  Hang();
#endif

  Screen::cout << "  - Initializing segments..." << Screen::endl;
  GDT::Initialize();

  Screen::cout << "  - Initializing interrupts..." << Screen::endl; 
  InterruptManager::Initialize();

  Screen::cout << "  - Initializing frame manager (" << _bootinfo->availableMemoryPhysicalBase << "," << _bootinfo->frameManagerVirtualStart << ")..." << Screen::endl; 
  FrameManager::Initialize((paddr)_bootinfo->availableMemoryPhysicalBase, (vaddr)_bootinfo->frameManagerVirtualStart);

  Screen::cout << "  - Initializing page manager..." << Screen::endl; 
  PageManager::Initialize();

  Screen::cout << "  - Initializing kernel heap (" <<_bootinfo->frameManagerVirtualEnd << ", 0x01000000) ..." << Screen::endl; 
  Kheap::Initialize((vaddr)_bootinfo->frameManagerVirtualEnd, 0x01000000); // 16 MiB

  Screen::cout << "  - Initializing keyboard..." << Screen::endl; 
	Keyboard::Initialize(); 

  Screen::cout << "  - Initializing timer..." << Screen::endl; 
	Timer::Initialize(50); 

  // PageManager tests
  //---------------------------------
  //paddr physicalAddress = FrameManager::GetFrame();
  //vaddr virtualAddress = (vaddr)0x10000000;
  //PageManager::Map(physicalAddress, virtualAddress, PageManager::Page::Writable);

  //((uint32*)virtualAddress)[0] = 0xBAADF00D;
  //Screen::cout.DumpMemory((intptr)virtualAddress, 256);

  //PageManager::Unmap(virtualAddress);

  // KHeap tests
  //---------------------------------
  //uint32* p1 = (uint32*)Kheap::Instance->Allocate(4,0);
  //*p1 = 0x11111111;

  //uint32* p2 = (uint32*)Kheap::Instance->Allocate(16,0);
  //p2[0] = 0x22222222;
  //p2[1] = 0x33333333;
  //p2[2] = 0x44444444;
  //p2[3] = 0x55555555;

  //Kheap::Instance->Free(p1);

  //uint32* p3 = (uint32*)Kheap::Instance->Allocate(4,0);
  //*p3 = 0x66666666;

  //Kheap::Instance->Free(p3);
  //Kheap::Instance->Free(p2);

  //Screen::cout.DumpMemory((intptr)((uint32)Kheap::Instance - 8), 256);


  __asm sti;

  Screen::cout << "  - Entering idle loop..." << Screen::endl; 
  Idle();
}


void Kernel::Panic(const char* message, const char* file, uint32 line, const char* function)
{
  __asm cli;

  Screen::cout << "******** PANIC (" << message << ") ********" << Screen::endl; 
  Screen::cout << "in " << function << Screen::endl;
  Screen::cout << "(" << file << ":" << line << ")" << Screen::endl;

  Hang();
}

void Kernel::Assert(const char* message, const char* file, uint32 line, const char* function)
{
  __asm cli;

  Screen::cout << "******** ASSERTION FAILED ********" << Screen::endl; 
  Screen::cout << "in " << function << Screen::endl;
  Screen::cout << "(" << file << ":" << line << ")" << Screen::endl;
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