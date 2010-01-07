//------------------------------------------------------------------------------
// kernel.h
//	Kernel class
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

#define PANIC(msg) Kernel::Panic(msg, __FILE__, __LINE__, __FUNCSIG__);
#define ASSERT(b) ((b) ? (void)0 : Kernel::Assert(#b,__FILE__, __LINE__, __FUNCSIG__))

#include "common.h"

namespace GenOS {

  class DebuggerClient;
  class FrameManager;
  class PageManager;
  class Kheap;
  class PdbParser;
  class Keyboard;
  class Timer;
  class Scheduler;
  class SerialPort;

  struct KernelBootInfo
  {
    uint32 kernelSize;
    uint32 kernelPhysicalStart;
    uint32 kernelPhysicalEnd;
    uint32 kernelVirtualStart;

    uint32 pdbSize;
    uint32 pdbPhysicalStart;
    uint32 pdbPhysicalEnd;
    uint32 pdbVirtualStart;

    uint32 stackSize;
    uint32 stackPhysicalStart;
    uint32 stackPhysicalEnd;
    uint32 stackVirtualStart;
    uint32 stackVirtualEnd;

    uint32 crtSize;
    uint32 crtPhysicalStart;
    uint32 crtPhysicalEnd;
    uint32 crtVirtualStart;
    uint32 crtVirtualEnd;

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
    static Kernel* _instance;

    DebuggerClient* _debugger;
    FrameManager*   _framemgr;
    PageManager*    _pagemgr;
    Kheap*          _heap;
    PdbParser*      _pdb;
    Keyboard*       _keyboard;
    Timer*          _timer;
    Scheduler*      _scheduler;

  public:
    Kernel(KernelBootInfo* bootinfo);
    void Run_step1();
    void Run_step2();

    static void Run_step2_thread();

    static _declspec(noreturn) void Panic(const char* message, const char* file, uint32 line, const char* function);
    static _declspec(noreturn) void Assert(const char* message, const char* file, uint32 line, const char* function);
    static _declspec(noreturn) void Hang();
    static _declspec(noreturn) void Idle();

  public:
    static FrameManager* FrameManager() { return _instance->_framemgr; }
    static PageManager* PageManager() { return _instance->_pagemgr; }
    static Kheap* Kheap() { return _instance->_heap; }
    //static PdbParser* PdbParser() { return _instance->_pdb; }
    //static Keyboard* Keyboard() { return _instance->_keyboard; }
    static Timer* Timer() { return _instance->_timer; }
    //static Scheduler* Scheduler() { return _instance->_scheduler; }

  private:
    Kernel(const Kernel&);
    Kernel& operator=(const Kernel&);
  };

};
