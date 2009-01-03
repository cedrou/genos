//------------------------------------------------------------------------------
// pagemanager.cpp
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

#include "pagemanager.h"
#include "framemanager.h"
#include "screen.h"
#include "intmgr.h"
#include "kernel.h"

using namespace GenOS;

void PageManager::Initialize(intptr kernel_start, intptr kernel_end)
{
  // Allocate a frame to store the page directory
  PageManager* newManager = (PageManager*)FrameManager::Get();

  // Identity map
  // 0x00000000 -> 0x00100000 : Lower memory
  for( uint32 address = 0x00000000; address < 0x00100000; address += 0x1000)
  {
    newManager->PreMap(address, address);
  }

  // Identity map
  // kernel_start -> kernel_end : Kernel
  for( uint32 address = (uint32)kernel_start; address < (uint32)kernel_end; address += 0x1000)
  {
    newManager->PreMap(address, address);
  }

  // 0xFFFFF000 -> 0xFFFFFFFF : Kernel page directory
  newManager->PreMap((uint32)newManager, 0xFFFFF000);

  // Register the timer interrupt handler.
  InterruptManager::RegisterInterrupt(InterruptManager::PageFault, &PageFaultHandler);

  // Enable paging
  __asm
  {
    mov eax, [newManager]
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
  }
}

void PageManager::PreMap(uint32 physicalAddress, uint32 virtualAddress)
{
  const uint32 tableIndex = (uint32)virtualAddress >> 22; // (address / 4096) / 1024
  const uint32 pageIndex = ((uint32)virtualAddress >> 12) & 0x03FF; // (address / 4096) % 1024

  if(!(dir.Tables[tableIndex] & Present))
  {
    dir.Tables[tableIndex] = (uint32)FrameManager::Get() | Present | RW;
  }

  PageManager::PageTable* table = (PageManager::PageTable*)(dir.Tables[tableIndex] & 0xFFFFF000);
  
  table->Pages[pageIndex] = physicalAddress | Present | RW;
}


void PageManager::Map(uint32 physicalAddress, uint32 virtualAddress, uint32 flags)
{
  const uint32 tableIndex = (uint32)virtualAddress >> 22; // (address / 4096) / 1024
  const uint32 pageIndex = ((uint32)virtualAddress >> 12) & 0x03FF; // (address / 4096) % 1024

  // Paging is enabled, so this = dir = 0xFFFFF000;
  if(!dir.Tables[tableIndex] & Present)
  {
    // doesn't exist, so alloc a page and add into pdir
    dir.Tables[tableIndex] = (uint32)FrameManager::Get() | Present | RW;
  }

  PageManager::PageTable* table = (PageManager::PageTable*)(0xFFC00000 + (tableIndex * 0x1000)); // virt addr of page table
  if(!table->Pages[pageIndex] & Present)
  {
    // page isn't mapped
    table->Pages[pageIndex] = physicalAddress | (flags & 0x0FFF) | Present;
  }
}

void PageManager::Switch()
{
  intptr physicalAddress = GetPhysicalAddress((intptr)0xFFFFF000);
  __asm
  {
    mov eax, [physicalAddress]
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
  }
}

intptr PageManager::GetPhysicalAddress(intptr virtualAddress)
{
  uint32 tableIndex = (uint32)virtualAddress >> 22;           // (address / 4096) / 1024
  uint32 pageIndex = ((uint32)virtualAddress >> 12) & 0x03FF; // (address / 4096) % 1024

  PageDirectory* pageDirectory = (PageDirectory*)0xFFFFF000;
  if(!dir.Tables[tableIndex] & Present) return NULL;

  PageTable* table = (PageTable*)(0xFFC00000 + (tableIndex * 0x1000)); // virt addr of page table
  if(!table->Pages[pageIndex] & Present) return NULL;

  return (intptr)((table->Pages[pageIndex] & ~0xFFF) + ((uint32)virtualAddress & 0xFFF));
}

void PageManager::PageFaultHandler(Registers regs)
{
   // A page fault has occurred.
   // The faulting address is stored in the CR2 register.
   uint32 faulting_address;
   __asm mov eax, cr2;
   __asm mov faulting_address, eax;

   // The error code gives us details of what happened.
   int present  = !(regs.err_code & 0x1); // Page not present
   int rw       = regs.err_code & 0x2;    // Write operation?
   int us       = regs.err_code & 0x4;    // Processor was in user-mode?

   // Output an error message.
   Screen::cout << "***PAGE FAULT (";
   if (present) Screen::cout << "present ";
   if (rw) Screen::cout << "read-only ";
   if (us) Screen::cout << "user-mode ";

   Screen::cout << ") at 0x" << faulting_address << Screen::endl;

   PANIC("Page fault");
}
