//------------------------------------------------------------------------------
// kldr.cpp
//  Module loaded by Multiboot
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

#define dw(x)                           \
        __asm _emit (x)       & 0xff  \
        __asm _emit (x) >> 8  & 0xff 

#define dd(x)       \
        dw(x)       \
        dw((x)>>16)

#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x00010002
#define MULTIBOOT_CKSUM     (-(MULTIBOOT_MAGIC+MULTIBOOT_FLAGS))
#define MULTIBOOT_KEY       0x2BADB002

#define KERNEL_BASE         0x00100000
#define KERNEL_START        0x00101000
#define KERNEL_TEXT_LEN     0x00001000
#define KERNEL_DATA_LEN     0x00002000
#define KERNEL_LENGTH       (KERNEL_TEXT_LEN + KERNEL_DATA_LEN)
#define KERNEL_END          (KERNEL_START + KERNEL_LENGTH)
#define KERNEL_STACK_LEN    0x1000
#define KERNEL_STACK        (KERNEL_END + KERNEL_STACK_LEN)

#include "common.h"
#include "screen.h"

using namespace GenOS;



struct MBModule
{
  uint32      Start;
  uint32      End;
  const char* CmdLine;
  uint32      Reserved;
};

struct MBMemoryMap
{
  uint32 Size;
  uint32 BaseAddrLow;
  uint32 BaseAddrHigh;
  uint32 LengthLow;
  uint32 LengthHigh;
  uint32 Type;
};

struct MBInfo
{
  uint32        Flags;
  uint32        MemoryLower;
  uint32        MemoryUpper;
  uint8         BootDevice[4];
  const char*   Cmdline;
  uint32        ModulesCount;
  MBModule*     Modules;
  uint32        Unused[4];
  uint32        MmapLength;
  MBMemoryMap*  Mmap;
};


void __declspec(naked) __multiboot_header__(void)
{
  __asm {
    dd(MULTIBOOT_MAGIC)          ; magic
    dd(MULTIBOOT_FLAGS)          ; flags
    dd(MULTIBOOT_CKSUM)          ; checksum

    dd(KERNEL_START)             ; header_addr
    dd(KERNEL_START)             ; load_addr
    dd(KERNEL_END)               ; load_end_addr
    dd(0x00000000)               ; bss_end_addr
    dd(0x00101030)               ; entry_addr
    dd(0x00000000)               ; mode_type
    dd(0x00000000)               ; width
    dd(0x00000000)               ; height
    dd(0x00000000)               ; depth
  }
}

void __declspec(naked) __entry_point__(void)
{
  uint32 magic;
  MBInfo* mbi;

  __asm
  {
    mov magic, eax;
    mov mbi, ebx;
  }

  Screen::Initialize();

  if(magic != MULTIBOOT_KEY)
  {
    Screen::cout << "Invalid GRUB magic key !" << Screen::endl;
    __asm hlt;
  }

  Screen::cout << "Loading GenOS..." << Screen::endl;
  Screen::cout << Screen::endl;

  Screen::cout << "Multiboot information structure:" << Screen::endl;
  Screen::cout << "  Flags         " << mbi->Flags << Screen::endl;         
  Screen::cout << "  MemoryLower   " << mbi->MemoryLower << Screen::endl; 
  Screen::cout << "  MemoryUpper   " << mbi->MemoryUpper << Screen::endl; 
  Screen::cout << "  BootDevice    " << mbi->BootDevice[0] << mbi->BootDevice[1] << mbi->BootDevice[2] << mbi->BootDevice[3] << Screen::endl; 
  Screen::cout << "  Cmdline       " << mbi->Cmdline << Screen::endl;
  Screen::cout << "  ModulesCount  " << mbi->ModulesCount << Screen::endl; 
  Screen::cout << "  Modules       " << (uint32)mbi->Modules << Screen::endl; 
  Screen::cout << "  Unused[4]     " << mbi->Unused[0] << mbi->Unused[1] << mbi->Unused[2] << mbi->Unused[3] << Screen::endl; 
  Screen::cout << "  MmapLength    " << mbi->MmapLength << Screen::endl; 
  Screen::cout << "  Mmap          " << mbi->Mmap << Screen::endl; 
  Screen::cout << Screen::endl;
  
  Screen::cout << "Modules:" << Screen::endl; 
  for(uint32 i=0; i<mbi->ModulesCount; i++)
  {
    Screen::cout << "  Start         " << mbi->Modules[i].Start << Screen::endl; 
    Screen::cout << "  End           " << mbi->Modules[i].End << Screen::endl; 
    Screen::cout << "  CmdLine       " << mbi->Modules[i].CmdLine << Screen::endl; 
    Screen::cout << "  Reserved      " << mbi->Modules[i].Reserved << Screen::endl; 
  }

  if ((mbi->Flags & (1<<6)) && mbi->MmapLength)
  {
    Screen::cout << "Memory map (" << mbi->MmapLength << " bytes):" << Screen::endl;
    uint32 items = mbi->MmapLength / sizeof(MBMemoryMap);
    for (uint32 i=0; i<items; i++)
    {
      Screen::cout << "  0x" << mbi->Mmap[i].BaseAddrHigh << mbi->Mmap[i].BaseAddrLow;
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

  const uint32 kernelPhysicalStart = mbi->Modules[0].Start;
  const uint32 kernelPhysicalEnd = mbi->Modules[0].End;
  const uint32 kernelPhysicalStack = kernelPhysicalEnd + 0x1000;
  uint32 physicalBase = kernelPhysicalStack;

  // Allocate a frame to store the page directory
  uint32* pageDirectory = (uint32*)physicalBase;
  physicalBase += 0x1000;
  memset((intptr)pageDirectory, (uint32)0, 1024);

  // Allocate a frame for a page table.
  // Identity map the lower memory + kldr (0 -> kernelStart)
  uint32* pageTable0 = (uint32*)physicalBase;
  physicalBase += 0x1000;
  memset((intptr)pageTable0, (uint32)0, 1024);
  pageDirectory[0] = (uint32)pageTable0 | 3;
  for( uint32 address = 0x00000000; address < kernelPhysicalStart; address += 0x1000)
  {
    pageTable0[address >> 12] = address | 3;
  }

  // Allocate a frame for a page table.
  // Map the kernel at 3GiB
  uint32* pageTable768 = (uint32*)physicalBase;
  physicalBase += 0x1000;
  memset((intptr)pageTable768, (uint32)0, 1024);
  pageDirectory[768] = (uint32)pageTable768 | 3;
  for( uint32 address = kernelPhysicalStart; address < kernelPhysicalStack; address += 0x1000)
  {
    const uint32 virtualAddress = 0xC0000000 - kernelPhysicalStart + address;
    const uint32 pageIndex = (virtualAddress >> 12) & 0x03FF;
    pageTable768[pageIndex] = address | 3;
  }

  pageDirectory[1023] = (uint32)pageDirectory | 3;


  // Enable paging
  __asm
  {
    mov eax, [pageDirectory]
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
  }

  const uint32 kernelVirtualStart = 0xC0000000;
  const uint32 kernelVirtualEnd = kernelVirtualStart - kernelPhysicalStart + kernelPhysicalEnd;
  const uint32 kernelVirtualStack = kernelVirtualStart - kernelPhysicalStart + kernelPhysicalStack;
  const uint32 kernelSize = kernelVirtualStack - kernelVirtualStart;
  const uint32 physicalSize = mbi->MemoryUpper - (physicalBase - 0x00100000);

  Screen::cout << kernelPhysicalStart << " -> " << kernelVirtualStart << Screen::endl;
  Screen::cout << kernelPhysicalEnd << " -> " << kernelVirtualEnd << Screen::endl;
  Screen::cout << kernelPhysicalStack << " -> " << kernelVirtualStack << Screen::endl;
  Screen::cout << kernelSize << Screen::endl;
  Screen::cout << physicalSize << Screen::endl;

  __asm
  {
    ; set up the stack for our kernel
    mov esp, kernelVirtualStack

    push physicalSize;
    push physicalBase;
    push kernelSize;

    mov eax, 0xC0001000
    call eax;
  }

  __asm hlt
}

void kldr()
{
}
