//------------------------------------------------------------------------------
// timer.cpp
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

#include "timer.h"
#include "intmgr.h"
#include "screen.h"
#include "hal/ioports.h"

using namespace GenOS;
using namespace GenOS::HAL;

Timer::Timer(uint32 freq)
{
  // The value we send to the PIT is the value to divide it's input clock
  // (1193180 Hz) by, to get our required frequency. Important to note is
  // that the divisor must be small enough to fit into 16-bits.
  uint32 divisor = 1193180 / freq;

  // Send the command byte.
  IOPort::Out8(IOPort::PIT_Control, 0x36);

  // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
  uint8 l = (uint8)(divisor & 0xFF);
  uint8 h = (uint8)( (divisor>>8) & 0xFF );

  // Send the frequency divisor.
  IOPort::Out8(IOPort::PIT_0, l);
  IOPort::Out8(IOPort::PIT_0, h);

  _tick = 0;

  // Register the timer interrupt handler.
  InterruptManager::RegisterInterrupt(InterruptManager::SystemTimer, &Timer::TickHandler, this);
}

void Timer::RegisterHandler(InterruptManager::Handler handler, void* data)
{
  _handler.Address = handler;
  _handler.Data = data;
}

void Timer::TickHandler(const Registers& regs, void* data)
{
  Timer* that = ((Timer*)data);
  that->_tick++;

  if(that->_handler.Address)
    that->_handler.Address(regs, that->_handler.Data);
}

uint32 Timer::Ticks() const
{
  return _tick;
}
