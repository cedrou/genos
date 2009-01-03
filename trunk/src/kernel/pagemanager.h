//------------------------------------------------------------------------------
// pagemanager.h
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

#pragma once

#include "common.h"

namespace GenOS
{
  class PageManager
  {
  private:
    enum PageState
    {
      Present        = 0x0001,  // If set, the page is in physical memory. Otherwise, the pqge is swapped. 
      RW             = 0x0002,  // If set, the page is read/write. Otherwise the page is read-only. 
      User           = 0x0004,  // If set, the page may be accessed by all. Otherwise, only the supervisor can access it. 
      WriteThrough   = 0x0008,  // If set, write-through caching is enabled. If not, write-back is enabled. 
      CacheDisabled  = 0x0010,  // If set, the page will not be cached. 
      Accessed       = 0x0020,  // If set, the page has been read or written to. This bit is not updated by the CPU. 
      Dirty          = 0x0040,  // (PageTable only) If set, the page table has been written to. This flag is not updated by the CPU. 
      PageSize       = 0x0080,  // (Page only) If set, pages are 4 MiB in size. Otherwise, they are 4 KiB. 
      Global         = 0x0100,  // (PageTable only) If set, prevents the TLB from updating the address in it's cache if CR3 is reset. Note, that the page global enable bit in CR4 must be set to enable this feature. 
    };

    struct PageTable
    {
      uint32 Pages[1024];
    };
    struct PageDirectory
    {
      uint32 Tables[1024];
    };

  private:
    PageDirectory dir;

  public:
    static PageManager* Current;

  public:
    static void Initialize(intptr kernel_start, intptr kernel_end);
  
    void Map(uint32 physicalAddress, uint32 virtualAddress, uint32 flags);
    void Unmap(uint32 virtualAddress);

    void Switch();

  private:
    void PreMap(uint32 physicalAddress, uint32 virtualAddress);
    intptr GetPhysicalAddress(intptr virtualAddress);


    static void __stdcall PageFaultHandler(Registers reg);
  };
}