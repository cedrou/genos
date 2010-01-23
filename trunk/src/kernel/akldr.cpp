//------------------------------------------------------------------------------
// akldr.cpp
//  Module loaded by Multiboot
//------------------------------------------------------------------------------
// This file is part of the GenOS (Genesis Operating System) project.
// The latest version can be found at http://code.google.com/p/genos
//------------------------------------------------------------------------------
// Copyright (c) 2008-2010 Cedric Rousseau
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

#define dd(x)                         \
        __asm _emit (x)       & 0xff  \
        __asm _emit (x) >> 8  & 0xff  \
        __asm _emit (x) >> 16 & 0xff  \
        __asm _emit (x) >> 24 & 0xff 

#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x00010002
#define MULTIBOOT_CKSUM     (-(MULTIBOOT_MAGIC+MULTIBOOT_FLAGS))
#define MULTIBOOT_KEY       0x2BADB002

#include "kernel.h"
#include "peformat.h"

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

    dd(0x00101000)               ; header_addr
    dd(0x00101000)               ; load_addr
    dd(0x00102000)               ; load_end_addr
    dd(0x00103000)               ; bss_end_addr
    dd(0x00101030)               ; entry_addr
    dd(0x00000000)               ; mode_type
    dd(0x00000000)               ; width
    dd(0x00000000)               ; height
    dd(0x00000000)               ; depth
  }
}

void memset(intptr dst, uint32 value, size_t count);

MBInfo* mbi;
GenOS::KernelBootInfo kbi;

uint32 bitsetItems;
uint32* bitset;

IMAGE_DOS_HEADER* kernel_dos;
IMAGE_NT_HEADERS32* kernel_nt32;
IMAGE_DOS_HEADER* corlib_dos;
IMAGE_NT_HEADERS32* corlib_nt32;

uint32 pageIndex;

void __declspec(naked) __entry_point__(void)
{
  __asm
  {
    cmp     eax, MULTIBOOT_KEY
    jz      valid_grub_id
    xor     ecx, ecx
    mov     edx, 0x1041 ; Red 'A'
    mov     [ecx+0B8000h], dx
    hlt

valid_grub_id:
    mov     mbi, ebx;
  }

  // The kernel was already loaded by Grub
  kbi.kernelSize                  = (mbi->Modules[0].End - mbi->Modules[0].Start + 0x0FFF) & 0xFFFFF000;
  kbi.kernelPhysicalStart         = mbi->Modules[0].Start;
  kbi.kernelPhysicalEnd           = mbi->Modules[0].End;

  kernel_dos = (IMAGE_DOS_HEADER*)kbi.kernelPhysicalStart;
  kernel_nt32 = (IMAGE_NT_HEADERS32*)(kbi.kernelPhysicalStart + kernel_dos->e_lfanew);

  kbi.kernelVirtualStart          = kernel_nt32->OptionalHeader.ImageBase; //0xC0000000;
  kbi.kernelVirtualSize           = kernel_nt32->OptionalHeader.SizeOfImage;
  kbi.kernelMissingPages          = (kbi.kernelVirtualSize - kbi.kernelSize) >> 12;

  if (kbi.kernelVirtualStart != 0xC0000000)
  {
    ((uint16*)0xB8000)[0] = 0x1000 | 'B';
    __asm hlt;
  }

  // The corlib was already loaded by Grub
  kbi.corlibSize                  = (mbi->Modules[1].End - mbi->Modules[1].Start + 0x0FFF) & 0xFFFFF000;
  kbi.corlibPhysicalStart         = mbi->Modules[1].Start;
  kbi.corlibPhysicalEnd           = mbi->Modules[1].Start + kbi.corlibSize;

  corlib_dos = (IMAGE_DOS_HEADER*)kbi.corlibPhysicalStart;
  corlib_nt32 = (IMAGE_NT_HEADERS32*)(kbi.corlibPhysicalStart + corlib_dos->e_lfanew);

  kbi.corlibVirtualStart          = corlib_nt32->OptionalHeader.ImageBase; //0xC0010000; //kbi.kernelVirtualStart + kbi.kernelSize;
  kbi.corlibVirtualSize           = corlib_nt32->OptionalHeader.SizeOfImage;
  kbi.corlibMissingPages          = (kbi.corlibVirtualSize - kbi.corlibSize) >> 12;

  if (kbi.corlibVirtualStart <= kbi.kernelVirtualStart + kbi.kernelVirtualSize)
  {
    ((uint16*)0xB8000)[0] = 0x1000 | 'C';
    __asm hlt;
  }



  // Place the kernel stack after the kernel.
  // Skip the first page which is a page guard
  kbi.stackSize                   = 0x00010000; // 64k stack
  kbi.stackPhysicalStart          = kbi.corlibPhysicalEnd;
  kbi.stackPhysicalEnd            = kbi.stackPhysicalStart + kbi.stackSize;
  kbi.stackVirtualStart           = 0xC0021000; //kbi.corlibVirtualStart + kbi.corlibSize + 0x1000;   // PageGuard placeholder
  kbi.stackVirtualEnd             = kbi.stackVirtualStart + kbi.stackSize;

  // Reserve a frame for the CRT
  kbi.crtSize                     = 0x1000;
  kbi.crtPhysicalStart            = kbi.stackPhysicalEnd;
  kbi.crtPhysicalEnd              = kbi.crtPhysicalStart + kbi.crtSize;
  kbi.crtVirtualStart             = kbi.stackVirtualEnd;
  kbi.crtVirtualEnd               = kbi.crtVirtualStart + kbi.crtSize;

  // Compute the current available memory base and size
  kbi.availableMemoryPhysicalBase = kbi.crtPhysicalEnd;
  kbi.availableMemorySize         = mbi->MemoryUpper * 1024 - (kbi.availableMemoryPhysicalBase - 0x00100000);


  // Prepare frame manager
  //----------------------------------------------

  // The frame manager starts just after the kernel stack
  kbi.frameManagerPhysicalStart = kbi.availableMemoryPhysicalBase;

  // Compute the the size (in 32 bits words) of the bitset
  bitsetItems = (kbi.availableMemorySize / 0x1000) / 32; // 128MiB = (134217728 / 4096) / 32 = 1024 items

  // Place this value at the start of the data block
  ((uint32*)kbi.frameManagerPhysicalStart)[0] = bitsetItems;

  // The bitset immediately follows this value
  bitset = &((uint32*)kbi.frameManagerPhysicalStart)[1];

  // Initialize the bitset to 1 (all frames are available)
  memset(bitset, 0xFFFFFFFF, bitsetItems);

  // Compute the number of bytes (aligned on 4kiB frames) used by the bitset
  kbi.frameManagerSize = 0x1000 * (((bitsetItems + 1) * 4 + 0x0FFF) / 0x1000);
  kbi.frameManagerPhysicalEnd = kbi.frameManagerPhysicalStart + kbi.frameManagerSize;

  // Compute virtual addresses
  kbi.frameManagerVirtualStart = kbi.crtVirtualEnd;
  kbi.frameManagerVirtualEnd = kbi.frameManagerVirtualStart + kbi.frameManagerSize;

  // Move the available memory pointer
  kbi.availableMemoryPhysicalBase = kbi.frameManagerPhysicalEnd;


  // Prepare paging
  //----------------------------------------------

  // Allocate a frame to store the page directory
  kbi.pageDirectory = (uint32*)kbi.availableMemoryPhysicalBase;
  kbi.availableMemoryPhysicalBase += 0x1000;
  memset((intptr)kbi.pageDirectory, (uint32)0, 1024);


  // Allocate a frame for page table 0 (v:0x00000000 -> v:0x00400000).
  kbi.pageTable0 = (uint32*)kbi.availableMemoryPhysicalBase;
  kbi.availableMemoryPhysicalBase += 0x1000;
  memset((intptr)kbi.pageTable0, (uint32)0, 1024);
  kbi.pageDirectory[0] = (uint32)kbi.pageTable0 | 3;

  // Identity map: lower memory + kldr (p:0x00000000 -> p:kernelStart)
  for( uint32 address = 0x00000000; address < kbi.kernelPhysicalStart; address += 0x1000)
  {
    const uint32 virtualAddress = address;
    pageIndex = (virtualAddress >> 12) & 0x03FF;
    kbi.pageTable0[pageIndex] = address | 3;
  }


  // Allocate a frame for page table 768 (v:0xC0000000 -> v:0xC0400000).
  //   768 = 0x300 = (0xC0000000 >> 22)
  kbi.pageTable768 = (uint32*)kbi.availableMemoryPhysicalBase;
  kbi.availableMemoryPhysicalBase += 0x1000;
  memset((intptr)kbi.pageTable768, (uint32)0, 1024);
  kbi.pageDirectory[768] = (uint32)kbi.pageTable768 | 3;

  // Map the kernel and the corlib at 3GiB
  for( uint32 address = kbi.kernelPhysicalStart; address < kbi.kernelPhysicalEnd; address += 0x1000)
  {
    const uint32 virtualAddress = kbi.kernelVirtualStart + (address - kbi.kernelPhysicalStart);
    pageIndex = (virtualAddress >> 12) & 0x03FF;
    kbi.pageTable768[pageIndex] = address | 3;
  }
  for( uint32 index = 0; index < kbi.kernelMissingPages; index++ )
  {
    // Allocate a frame for the new page
    const uint32 frame = kbi.availableMemoryPhysicalBase;
    kbi.availableMemoryPhysicalBase += 0x1000;
    memset((intptr)frame, (uint32)0, 1024);

    kbi.pageTable768[pageIndex + index + 1] = frame | 3;
  }

  for( uint32 address = kbi.corlibPhysicalStart; address < kbi.corlibPhysicalEnd; address += 0x1000)
  {
    const uint32 virtualAddress = kbi.corlibVirtualStart + (address - kbi.corlibPhysicalStart);
    pageIndex = (virtualAddress >> 12) & 0x03FF;
    kbi.pageTable768[pageIndex] = address | 3;
  }
  for( uint32 index = 0; index < kbi.corlibMissingPages; index++ )
  {
    // Allocate a frame for the new page
    const uint32 frame = kbi.availableMemoryPhysicalBase;
    kbi.availableMemoryPhysicalBase += 0x1000;
    memset((intptr)frame, (uint32)0, 1024);

    kbi.pageTable768[pageIndex + index + 1] = frame | 3;
  }

  for( uint32 address = kbi.stackPhysicalStart; address < kbi.stackPhysicalEnd; address += 0x1000)
  {
    const uint32 virtualAddress = kbi.stackVirtualStart + (address - kbi.stackPhysicalStart);
    pageIndex = (virtualAddress >> 12) & 0x03FF;
    kbi.pageTable768[pageIndex] = address | 3;
  }

  for( uint32 address = kbi.crtPhysicalStart; address < kbi.crtPhysicalEnd; address += 0x1000)
  {
    const uint32 virtualAddress = kbi.crtVirtualStart + (address - kbi.crtPhysicalStart);
    pageIndex = (virtualAddress >> 12) & 0x03FF;
    kbi.pageTable768[pageIndex] = address | 3;
  }

  for( uint32 address = kbi.frameManagerPhysicalStart; address < kbi.frameManagerPhysicalEnd; address += 0x1000)
  {
    const uint32 virtualAddress = kbi.frameManagerVirtualStart + (address - kbi.frameManagerPhysicalStart);
    pageIndex = (virtualAddress >> 12) & 0x03FF;
    kbi.pageTable768[pageIndex] = address | 3;
  }


  // Recursive map of the page directory
  kbi.pageDirectory[1023] = (uint32)kbi.pageDirectory | 3;



  // Clear in the bitset the corresponding bits of the used frames 
  for(uint32 index=0; index<(kbi.availableMemoryPhysicalBase-kbi.frameManagerPhysicalStart) / 0x1000; index++)
  {
    uint32 item = (index >> 5);    // frame_index / 32;
    uint32 bit = (index & 0x1F);   // frame_index % 32
    bitset[item] &= ~(1 << bit);
  }

  kbi.availableMemorySize         = mbi->MemoryUpper * 1024 - (kbi.availableMemoryPhysicalBase - 0x00100000);

  const uint32 entryPoint = kernel_nt32->OptionalHeader.ImageBase + kernel_nt32->OptionalHeader.AddressOfEntryPoint;

  ((uint16*)0xB8000)[0] = 0x1000 | 'Z';

  __asm
  {
; Enable paging
    mov eax, kbi.pageDirectory
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
  }

  ((uint16*)0xB8000)[0] = 0x1000 | 'Y';

  __asm
  {
; set up the stack for our kernel
    mov esp, kbi.stackVirtualEnd

; call kernel
    lea eax, kbi
    push eax

; TODO: dynamically read entry point
    mov eax, entryPoint
    call eax;

; this should never be reached
    hlt
  }
}

void memset(intptr dst, uint32 value, size_t count)
{
	uint32* d = (uint32*)dst;
	while(count--) { *d++ = value; }
}

