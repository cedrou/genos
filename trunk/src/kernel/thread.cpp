//------------------------------------------------------------------------------
// thread.cpp
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

#include "thread.h"
#include <intmgr.h>

using namespace GenOS;

uint32 Thread::_counter = 0;

Thread::Thread(Process* parent, void* entryPoint)
{
  _tid = _counter++;  // Thread ID
  _parent = parent;  // Process ID
  _next = NULL;
  _ticks = 0;
  _priority = 5;
  _esp = 0; 
  _stackbase = 0;
  _stacksize = 0x4000;
  InitStack(entryPoint);
}

Thread::~Thread()
{
}

Thread* Thread::Current()
{
  return NULL;
}

void Thread::InitStack(void* entryPoint)
{
  uint8* stack = new uint8[_stacksize];
  uint8* end = stack + _stacksize;
  //Screen::cout << "InitStack: " << (uint32)stack << " - " << (uint32)end << Screen::endl;

  uint32* esp = (uint32*)end;
  *(--esp) = 0x10; // ss
  *(--esp) = 0; // useresp
  *(--esp) = 0x00000202; // eflags
  *(--esp) = 0x08; // cs
  *(--esp) = (uint32)entryPoint; // eip

  *(--esp) = 0; // err_code
  *(--esp) = 0x20; // int_no (Timer)
  
  *(--esp) = 0; // eax
  *(--esp) = 0; // ecx
  *(--esp) = 0; // edx
  *(--esp) = 0; // ebx
  *(--esp) = (uint32)end; // esp
  *(--esp) = 0; // ebp
  *(--esp) = 0; // esi
  *(--esp) = 0; // edi
  
  *(--esp) = 0x10; // ds
  
  *(--esp) = 0; // 'void* data' argument
  *(--esp) = 0; // 'const Register& regs' argument
  *(--esp) = (uint32)InterruptManager::IsrEnd; // returned eip
  *(--esp) = (uint32)end; // saved ebp

  _stackbase = (uint32)stack;
  _esp = (uint32)(esp);
}
