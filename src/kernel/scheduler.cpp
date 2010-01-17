//------------------------------------------------------------------------------
// scheduler.cpp
//	
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

#include "scheduler.h"
#include <process.h>
#include <thread.h>
#include <screen.h>
#include <timer.h>
#include <intmgr.h>

using namespace GenOS;

void Scheduler::TickHandler(const Registers& /*regs*/, void* data)
{
  Scheduler* _this = (Scheduler*)data;
  uint32 temp = 0xBAADF00D;

  Thread* previousThread = _this->_currentThread;
  Thread* nextThread = _this->NextThread();

  if( previousThread != nextThread )
  {
    //regs.Print();
    //Screen::cout << "--- Stack dump ---"; Screen::cout << Screen::endl;
    //uint32 esp_value;
    //_asm mov esp_value, esp;
    //for ( int i = 0; i<20; i++ )
    //{
    //  Screen::cout << esp_value + i*4;
    //  Screen::cout << " ";
    //  Screen::cout << *(uint32*)(esp_value + i*4);
    //  Screen::cout << Screen::endl;
    //}

    //Kernel::Hang();

    // C003FF68   C00421A8   -10  edi
    //            FFFFFFFF   -0C  edx
    // c003FF70   C004216C   -08  ecx 
    //            BAADF00D   -04  temp
    // c003FF80   00001F88    00  previous ebp
    //            C0001696    04  return eip (InterruptManager::Isr + 0x5A)
    //            C003FF88    08  const Register& regs
    //            C003FF68    0C  void* data
    // c003FF90   ds          08  Register regs
    //            edi
    //            esi
    //            ebp
    // c003FFA0   esp
    //            ebx
    //            edx
    //            ecx
    // c003FFB0   eax
    //            interrupt
    //            error code
    //            eip
    // c003FFC0   cs
    //            eflags
    //            useresp
    //            ss
    //    <thread stack>
    //    ...
    //            

    // Switch thread
    
    // Save current ESP to previous thread structure
    if( previousThread )
    {
      _asm mov temp, ebp;
      previousThread->_esp = temp;
    }

    temp = _this->_currentThread->_esp;
    _asm mov ebp, temp;
  }
}

Thread* Scheduler::NextThread()
{
  if(_currentThread)
  {
    // Decrement time to live counter.
    //Screen::cout << "** Decrement current thread TTL..." << Screen::endl;
    _currentThread->_ticks--;

    // If not zero, continue with this thread
    if (_currentThread->_ticks) 
    {
      //Screen::cout << "C";
      return _currentThread;
    }

    // The thread has came to its end, select the next to execute
    //Screen::cout << "** Current thread has finished its time..." << Screen::endl;
    if(_currentThread->_next)
    {
      //Screen::cout << "** Get next thread..." << Screen::endl;
      _currentThread = _currentThread->_next;
    }
    else
    {
      // Last thread of the current process
      // -> get the first of the next process
      //Screen::cout << "** Get first thread of next process..." << Screen::endl;
      Process* parent = _currentThread->_parent;
      parent = parent->_next;
      if(!parent) parent = _processes;

      _currentThread = parent->_threads;
    }
  }
  else
  {
    //Screen::cout << "** Get first thread of first process..." << Screen::endl;

    _currentThread = _processes->_threads;
  }

  // Initialize its time to live
  //Screen::cout << "** Initializing current thread TTL..." << Screen::endl;
  _currentThread->_ticks = _currentThread->_priority;

  return _currentThread;
}

Scheduler::Scheduler()
{
}


_declspec(noreturn) void Scheduler::Initialize()
{
  _asm cli;

  Process* kernelProcess = new Process();
  Thread* kernelThread = new Thread(kernelProcess, Kernel::Run_step2_thread);

  kernelProcess->_threads = kernelThread;

  _processes = kernelProcess;
  _currentThread = 0;

  Kernel::Timer()->RegisterHandler(&Scheduler::TickHandler, this);

  // Register the timer interrupt handler.
  //InterruptManager::RegisterInterrupt(InterruptManager::SystemTimer, &Scheduler::TickHandler, this);

  //Screen::cout << "Scheduler enabled";

  _asm sti
  _asm hlt
}

void Scheduler::AddProcess(Process * p)
{
  _asm cli;

  p->_next = _processes;
  _processes = p;

  _asm sti;
}
