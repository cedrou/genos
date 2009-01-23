//------------------------------------------------------------------------------
// pagemanager.cpp
//	
//------------------------------------------------------------------------------
// Copyright (c) 2008, Cedric Rousseau
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

#include "pagemanager.h"
#include "framemanager.h"
#include "screen.h"
#include "intmgr.h"
#include "kernel.h"

using namespace GenOS;


// PageManager::PageTableEntry
//------------------------------------------

void PageManager::Entry::SetFrame(paddr frame)
{
  data = ((uint32)frame & Page::Frame) | Page::Present;
}

paddr PageManager::Entry::GetFrame()
{
  return (paddr)(data & Page::Frame);
}

void PageManager::Entry::SetFlag(Page::Attributes flags)
{
  data |= flags;
}

void PageManager::Entry::UnsetFlag(Page::Attributes flags)
{
  data &= ~flags;
}

bool PageManager::Entry::TestFlag(Page::Attributes flag)
{
  return ((data & (uint32)flag) == (uint32)flag);
}

bool PageManager::Entry::Allocate(Page::Attributes flags)
{
  // Allocate a frame
  paddr frame = Kernel::FrameManager()->GetFrame();
  if(frame==NULL) return false;

  // Map it to the page
  SetFrame(frame);
  SetFlag(flags);

  return true;
}

paddr PageManager::Entry::Free()
{
  UnsetFlag(Page::Present);

  return GetFrame();
}



// PageManager
//------------------------------------------

//PageManager::PageDirectory* PageManager::Current = NULL;

void PageManager::Initialize()
{
  // Register the timer interrupt handler.
  InterruptManager::RegisterInterrupt(InterruptManager::PageFault, &PageFaultHandler, NULL);
}


void PageManager::PageFaultHandler(Registers regs, void*)
{
   // A page fault has occurred.
   // The faulting address is stored in the CR2 register.
   uint32 faulting_address;
   __asm mov eax, cr2;
   __asm mov faulting_address, eax;

   // The error code gives us details of what happened.
   int present  = regs.err_code & 0x1; // Page not present
   int rw       = regs.err_code & 0x2;    // Write operation?
   int us       = regs.err_code & 0x4;    // Processor was in user-mode?

   // Output an error message.
   Screen::cout << "*** PAGE FAULT ***" << Screen::endl;
   Screen::cout << (us ? "User " : "Kernel ")
                << "process tried to "
                << (rw ? "write " : "read ")
                << (present ? "a page and caused a protection fault " : "a non-present page entry ")
                << "at address 0x" << faulting_address << Screen::endl;
   Screen::cout << "EIP 0x" << regs.eip << Screen::endl;

   PANIC("Page fault");
}


void PageManager::PageTable::Clear()
{
  memset(Entries, (uint32)0, 1024);
}

void PageManager::PageDirectory::Clear()
{
  memset(Entries, (uint32)0, 1024);
}

uint32 PageManager::PageTable::GetIndex(vaddr virtualAddress)
{
  return ((uint32)virtualAddress >> 12) & 0x03FF;
}

uint32 PageManager::PageDirectory::GetIndex(vaddr virtualAddress)
{
  return (uint32)virtualAddress >> 22;
}

PageManager::PageTableEntry* PageManager::PageTable::FindEntry(vaddr virtualAddress)
{
  return &Entries[GetIndex(virtualAddress)];
}

PageManager::PageDirectoryEntry* PageManager::PageDirectory::FindEntry(vaddr virtualAddress)
{
  return &Entries[GetIndex(virtualAddress)];
}

bool PageManager::Switch(PageDirectory* dir)
{
  if(dir==NULL) return false;

  Current = dir;

  __asm
  {
    mov eax, dir
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
  }

  return true;
}

void PageManager::FlushTLBEntry(vaddr virtualAddress)
{
  _asm 
  {
    cli
    invlpg virtualAddress
    sti
  }
}

bool PageManager::Map(paddr physicalAddress, vaddr virtualAddress, Page::Attributes flags)
{
  // Paging is enabled, so this = dir = 0xFFFFF000;
  PageDirectory* dir = (PageDirectory*)0xFFFFF000;
  PageDirectoryEntry* pde = dir->FindEntry(virtualAddress);

  PageTable* table = (PageTable*)(0xFFC00000 + (dir->GetIndex(virtualAddress) * 0x1000));
  PageTableEntry* pte = table->FindEntry(virtualAddress);

  // Check if the page table associated to this virtual address is present
  if(!pde->TestFlag(Page::Present))
  {
    // doesn't exist, so alloc a page and add into pdir
    pde->Allocate(Page::Writable);
    //FlushTLBEntry(table);
  }

  if(pte->TestFlag(Page::Present))
    return false;

  // page isn't mapped
  if(physicalAddress==0)
  {
    physicalAddress = Kernel::FrameManager()->GetFrame();
  }
  pte->SetFrame(physicalAddress);
  pte->SetFlag(flags);

  //FlushTLBEntry(virtualAddress);

  return true;
}

bool PageManager::Unmap(vaddr virtualAddress)
{
  // Paging is enabled, so this = dir = 0xFFFFF000;
  PageDirectory* dir = (PageDirectory*)0xFFFFF000;
  PageDirectoryEntry* pde = dir->FindEntry(virtualAddress);

  if(!pde->TestFlag(Page::Present))
  {
    // doesn't exist, nothing is mapped
    return false;
  }

  PageTable* table = (PageTable*)(0xFFC00000 + (dir->GetIndex(virtualAddress) * 0x1000));
  PageTableEntry* pte = table->FindEntry(virtualAddress);

  if(!pte->TestFlag(Page::Present))
  {
    // page isn't mapped
    return false;
  }

  paddr frame = pte->Free();
  Kernel::FrameManager()->ReleaseFrame(frame);

  return true;
}

paddr PageManager::GetPhysicalAddress(vaddr virtualAddress)
{
  // Paging is enabled, so this = dir = 0xFFFFF000;
  PageDirectory* dir = (PageDirectory*)0xFFFFF000;
  PageDirectoryEntry* pde = dir->FindEntry(virtualAddress);

  if(!pde->TestFlag(Page::Present))
  {
    // doesn't exist, nothing is mapped
    return NULL;
  }

  PageTable* table = (PageTable*)(0xFFC00000 + (dir->GetIndex(virtualAddress) * 0x1000));
  PageTableEntry* pte = table->FindEntry(virtualAddress);

  if(!pte->TestFlag(Page::Present))
  {
    // page isn't mapped
    return NULL;
  }

  return pte->GetFrame();
}

