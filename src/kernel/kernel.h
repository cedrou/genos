//------------------------------------------------------------------------------
// kernel.h
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

#pragma once

#include "common.h"

namespace GenOS {

#define PANIC(msg) Kernel::Panic(msg, __FILE__, __LINE__, __FUNCSIG__);
#define ASSERT(b) ((b) ? (void)0 : Kernel::Assert(#b,__FILE__, __LINE__, __FUNCSIG__))

  struct KernelBootInfo
  {
    uint32 kernelSize;
    uint32 kernelPhysicalStart;
    uint32 kernelPhysicalEnd;
    uint32 kernelVirtualStart;

    uint32 stackSize;
    uint32 stackPhysicalStart;
    uint32 stackPhysicalEnd;
    uint32 stackVirtualStart;
    uint32 stackVirtualEnd;

    uint32 frameManagerSize;
    uint32 frameManagerPhysicalStart;
    uint32 frameManagerPhysicalEnd;
    uint32 frameManagerVirtualStart;
    uint32 frameManagerVirtualEnd;
    
    uint32 availableMemoryPhysicalBase;
    uint32 availableMemorySize;

    uint32* pageDirectory;
    uint32* pageTable0;
    uint32* pageTable768;
  };

  class Kernel 
  {
  private:
    const KernelBootInfo* _bootinfo;

  public:
    Kernel(KernelBootInfo* bootinfo);
    void Run();

    static void Panic(const char* message, const char* file, uint32 line, const char* function);
    static void Assert(const char* message, const char* file, uint32 line, const char* function);
    static void Hang();
    static void Idle();

  private:
    Kernel(const Kernel&);
    Kernel& operator=(const Kernel&);
  };

};