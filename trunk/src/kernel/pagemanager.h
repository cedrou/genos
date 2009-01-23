//------------------------------------------------------------------------------
// pagemanager.h
//	Class which manages the physical<->virtual memory translation
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

#pragma once

#include "common.h"

namespace GenOS
{
  class PageManager
  {
  public:
    class Page
    {
    public:
      enum Attributes
      {
        Present        = 0x00000001,  // If set, the page is in physical memory. Otherwise, the page is swapped. 
        Writable       = 0x00000002,  // If set, the page is read/write. Otherwise the page is read-only. 
        User           = 0x00000004,  // If set, the page may be accessed by all. Otherwise, only the supervisor can access it. 
        WriteThrough   = 0x00000008,  // If set, write-through caching is enabled. If not, write-back is enabled. 
        CacheDisabled  = 0x00000010,  // If set, the page will not be cached. 
        Accessed       = 0x00000020,  // If set, the page has been read or written to. This bit is set by the CPU. 
        Dirty          = 0x00000040,  // (PageTable only) If set, the page table has been written to. This flag set by the CPU. 
        LargePage      = 0x00000080,  // If set, pages are 4 MiB in size. Otherwise, they are 4 KiB. 
        Global         = 0x00000100,  // If set, prevents the TLB from updating the address in it's cache if CR3 is reset. Note, that the page global enable bit in CR4 must be set to enable this feature. 
        Frame          = 0xFFFFF000,  // (PageTable only) If set, prevents the TLB from updating the address in it's cache if CR3 is reset. Note, that the page global enable bit in CR4 must be set to enable this feature. 
      };
    };

  private:

    class Entry
    {
    private:
      uint32 data;

    public:
      bool Allocate(Page::Attributes flags);
      paddr Free();

      void  SetFrame(paddr frame);
      paddr GetFrame();

      void  SetFlag(Page::Attributes flag);
      void  UnsetFlag(Page::Attributes flag);
      bool  TestFlag(Page::Attributes flag);
    };

    typedef Entry PageTableEntry;
    typedef Entry PageDirectoryEntry;

    class PageTable
    {
    public:
      PageTableEntry Entries[1024];

      void Clear();
      uint32 GetIndex(vaddr virtualAddress);
      PageTableEntry* FindEntry(vaddr virtualAddress);
    };

    class PageDirectory
    {
    public:
      PageDirectoryEntry Entries[1024];

      void Clear();
      uint32 GetIndex(vaddr virtualAddress);
      PageDirectoryEntry* FindEntry(vaddr virtualAddress);
    };

  public:
    /*static*/ PageDirectory* Current;

  public:
    /*static*/ void   Initialize();
  
    /*static*/ bool   Map(paddr physicalAddress, vaddr virtualAddress, Page::Attributes flags);
    /*static*/ bool   Unmap(vaddr virtualAddress);
    /*static*/ paddr  GetPhysicalAddress(vaddr virtualAddress);

  private:
    bool Switch(PageDirectory* dir);
    /*static*/ void FlushTLBEntry(vaddr virtualAddress);

    static void __stdcall PageFaultHandler(Registers reg, void* data);
  };
}