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

#define dd(x)                         \
        __asm _emit (x)       & 0xff  \
        __asm _emit (x) >> 8  & 0xff  \
        __asm _emit (x) >> 16 & 0xff  \
        __asm _emit (x) >> 24 & 0xff 

#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x00010002
#define MULTIBOOT_CKSUM     (-(MULTIBOOT_MAGIC+MULTIBOOT_FLAGS))
#define MULTIBOOT_KEY       0x2BADB002

#define KERNEL_BASE         0x00100000
#define KERNEL_START        0x00101000
#define KERNEL_TEXT_LEN     0x00001000
#define KERNEL_DATA_LEN     0x00000000
#define KERNEL_LENGTH       (KERNEL_TEXT_LEN + KERNEL_DATA_LEN)
#define KERNEL_END          (KERNEL_START + KERNEL_LENGTH)

typedef unsigned int   uint32;
typedef unsigned char  uint8;
typedef          void* intptr;

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
    dd(0x00101050)               ; entry_addr
    dd(0x00000000)               ; mode_type
    dd(0x00000000)               ; width
    dd(0x00000000)               ; height
    dd(0x00000000)               ; depth
  }
}

void memset(intptr dst, uint32 value, size_t count);

void __declspec(naked) __entry_point__(void)
{
  MBInfo* mbi;

  uint32 kernelPhysicalStart;
  uint32 kernelPhysicalEnd;
  uint32 kernelPhysicalStack;
  uint32 kernelVirtualStart;
  uint32 kernelVirtualEnd;
  uint32 kernelVirtualStack;
  
  uint32 physicalBase;
  uint32 physicalSize;

  uint32 kernelSize;

  uint32* pageDirectory;
  uint32* pageTable0;
  uint32* pageTable768;

  __asm
  {
    cmp eax, MULTIBOOT_KEY
    jz valid_grub_id
    hlt

valid_grub_id:
    mov mbi, ebx;
  }

  kernelPhysicalStart = mbi->Modules[0].Start;
  kernelPhysicalEnd   = mbi->Modules[0].End;
  kernelPhysicalStack = kernelPhysicalEnd + 0x1000;

  kernelVirtualStart  = 0xC0000000;
  kernelVirtualEnd    = kernelVirtualStart - kernelPhysicalStart + kernelPhysicalEnd;
  kernelVirtualStack  = kernelVirtualStart - kernelPhysicalStart + kernelPhysicalStack;
  
  kernelSize          = kernelVirtualStack - kernelVirtualStart;
  
  physicalBase        = kernelPhysicalStack;

  // Prepare paging
  //----------------------------------------------

  // Allocate a frame to store the page directory
  pageDirectory = (uint32*)physicalBase;
  physicalBase += 0x1000;
  memset((intptr)pageDirectory, (uint32)0, 1024);

  // Allocate a frame for a page table.
  // Identity map: lower memory + kldr (0 -> kernelStart)
  pageTable0 = (uint32*)physicalBase;
  physicalBase += 0x1000;
  memset((intptr)pageTable0, (uint32)0, 1024);
  pageDirectory[0] = (uint32)pageTable0 | 3;
  for( uint32 address = 0x00000000; address < kernelPhysicalStart; address += 0x1000)
  {
    pageTable0[address >> 12] = address | 3;
  }

  // Allocate a frame for a page table.
  // Map the kernel at 3GiB
  pageTable768 = (uint32*)physicalBase;
  physicalBase += 0x1000;
  memset((intptr)pageTable768, (uint32)0, 1024);
  pageDirectory[768] = (uint32)pageTable768 | 3;
  for( uint32 address = kernelPhysicalStart; address < kernelPhysicalStack; address += 0x1000)
  {
    const uint32 virtualAddress = 0xC0000000 - kernelPhysicalStart + address;
    const uint32 pageIndex = (virtualAddress >> 12) & 0x03FF;
    pageTable768[pageIndex] = address | 3;
  }

  // Recursive map of the page directory
  pageDirectory[1023] = (uint32)pageDirectory | 3;



  physicalSize = mbi->MemoryUpper * 1024 - (physicalBase - 0x00100000);



  // Prepare frame manager
  //----------------------------------------------

  // Compute the total number of frames and the size (in 32 bits words)
  // of the bitset
  uint32 frames = physicalSize / 0x1000;
  uint32 bitset_items = frames / 32;

  // Place the size of the bitset at the start of the free memory block
  ((uint32*)physicalBase)[0] = frames / 32;

  // The bitset immediately follows this value
  uint32* bitset = &((uint32*)physicalBase)[1];

  // Initialize the bitset to 1 (all frames are available)
  memset(bitset, 0xFFFFFFFF, bitset_items);

  // Compute the number of frames used by the bitset
  uint32 bitset_size = bitset_items * 8;
  uint32 bitset_frames = (bitset_size + 0x0FFF) / 0x1000;

  // Clear the corresponding bits in the bitset
  for(uint32 index=0; index<bitset_frames; index++)
  {
    uint32 item = (index >> 5);    // frame_index / 32;
    uint32 bit = (index & 0x1F);   // frame_index % 32
    bitset[item] &= ~(1 << bit);
  }

  physicalSize -= bitset_frames * 0x1000;

  __asm
  {
; Enable paging
    mov eax, [pageDirectory]
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

; set up the stack for our kernel
    mov esp, kernelVirtualStack

; call kernel
    push physicalSize;
    push physicalBase;
    push kernelSize;

    mov eax, 0xC0001000
    call eax;
  }

  // This should never be reached
  __asm hlt
}

void memset(intptr dst, uint32 value, size_t count)
{
	uint32* d = (uint32*)dst;
	while(count--) { *d++ = value; }
}
