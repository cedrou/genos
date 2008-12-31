//------------------------------------------------------------------------------
// gdt.cpp
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

#include "gdt.h"
#include "registers.h"
#include "screen.h"

using namespace GenOS;


#pragma pack(push,1)

struct GlobalDescriptor
{
  uint16 limit_low;           // The lower 16 bits of the limit.
  uint16 base_low;            // The lower 16 bits of the base.
  uint8  base_middle;         // The next 8 bits of the base.
  uint8  access;              // Access flags, determine what ring this segment can be used in.
  uint8  granularity;
  uint8  base_high;           // The last 8 bits of the base.
};

struct GlobalDescriptorTable
{
  uint16 limit;
  uint32 base;     // The address of the first element in our idt_entry_t array.
};

#pragma pack(pop)

static GlobalDescriptor gdt[5];


void GDT::Initialize()
{
  // Fill in the GDT
  EncryptGdtEntry(0, 0, 0, 0, 0);                // Null segment
  EncryptGdtEntry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
  EncryptGdtEntry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
  EncryptGdtEntry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
  EncryptGdtEntry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
  
  // Give the GDT to the CPU
  GlobalDescriptorTable gdtptr; 
  gdtptr.limit = sizeof(GlobalDescriptor) * 5 -1;
  gdtptr.base  = (uint32)&gdt;
  __asm 
  {
    lgdt gdtptr

    mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
    mov ds, ax        ; Load all data segment selectors
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; jmp 0x08:.flush   ; far jump
    push 0x08
    mov eax, offset flush
    push eax
    _emit 0xCB ; far return
flush:
  }
}

// Set the value of one GDT entry.
void GDT::EncryptGdtEntry(uint8 index, uint32 base, uint32 limit, uint8 access, uint8 granularity)
{
   gdt[index].base_low    = (base & 0xFFFF);
   gdt[index].base_middle = (base >> 16) & 0xFF;
   gdt[index].base_high   = (base >> 24) & 0xFF;

   gdt[index].limit_low   = (limit & 0xFFFF);
   gdt[index].granularity = (limit >> 16) & 0x0F;

   gdt[index].granularity |= granularity & 0xF0;
   gdt[index].access      = access;
}

