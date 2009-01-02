//------------------------------------------------------------------------------
// kmain.cpp
//  Entry point
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

#define dd(x)                        \
        __asm _emit (x)       & 0xff \
        __asm _emit (x) >> 8  & 0xff \
        __asm _emit (x) >> 16 & 0xff \
        __asm _emit (x) >> 24 & 0xff

#define MULTIBOOT_MAGIC     0x1BADB002
#define MULTIBOOT_FLAGS     0x00010002
#define MULTIBOOT_CKSUM     (-(MULTIBOOT_MAGIC+MULTIBOOT_FLAGS))

#define KERNEL_BASE         0x00100000
#define KERNEL_START        0x00101000
#define KERNEL_TEXT_LEN     0x00002000
#define KERNEL_DATA_LEN     0x00002000
#define KERNEL_LENGTH       (KERNEL_TEXT_LEN + KERNEL_DATA_LEN)
#define KERNEL_END          (KERNEL_START + KERNEL_LENGTH)
#define KERNEL_STACK_LEN    0x1000
#define KERNEL_STACK        (KERNEL_END + KERNEL_STACK_LEN)


void kmain(uint32 magic, uint32 mbinfo);

void __declspec(naked) __multiboot_entry__(void)
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

    mov   esp, KERNEL_STACK-1

    xor   ecx, ecx
    push  ecx
    popf

    push  ebx       ; multiboot info address
    push  eax       ; magic
    call  kmain

    jmp   $
  }
}

void kmain(uint32 /*magic*/, uint32 mbinfo)
{
  GenOS::Kernel kernel((intptr)KERNEL_BASE, (intptr)KERNEL_STACK, mbinfo);
  kernel.Run();
}