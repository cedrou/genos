//------------------------------------------------------------------------------
// intmgr.h
//	CPU Interruptions Manager
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

#pragma once

#include "common.h"
#include "registers.h"

namespace GenOS {

class InterruptManager
{
public:
  typedef void (__stdcall *Handler)(const Registers& reg, void* data);

  struct HandlerInfo
  {
    Handler Address;
    void*   Data;
  };


  enum Interrupts
  {
		DivideError           = 0x00,
		//IntelReserved       = 0x01,
		NMI                   = 0x02,
		Breakpoint            = 0x03,
		Overflow              = 0x04,
		BoundRangeExceeded    = 0x05,
		InvalidOpcode         = 0x06, 
		DeviceNotAvailable    = 0x07,
		DoubleFault           = 0x08,
    CoprocSegmentOverrun  = 0x09,
		InvalidTSS            = 0x0A,
		SegmentNotPresent     = 0x0B,
		StackSegmentFault     = 0x0C,
		GeneralProtection     = 0x0D,
		PageFault             = 0x0E,
		//IntelReserved       = 0x0F,	
		MathFault             = 0x10,
		AlignmentCheck        = 0x11,
		MachineCheck          = 0x12,
		SIMDException         = 0x13,
		//IntelReserved       = 0x14,	
		//IntelReserved       = 0x15,	
		//IntelReserved       = 0x16,	
		//IntelReserved       = 0x17,	
		//IntelReserved       = 0x18,	
		//IntelReserved       = 0x19,	
		//IntelReserved       = 0x1A,	
		//IntelReserved       = 0x1B,	
		//IntelReserved       = 0x1C,	
		//IntelReserved       = 0x1D,	
		//IntelReserved       = 0x1E,	
		//IntelReserved       = 0x1F,	

		IRQ0                  = 0x20,
		IRQ1                  = 0x21,
		IRQ2                  = 0x22,
		IRQ3                  = 0x23,
		IRQ4                  = 0x24,
		IRQ5                  = 0x25,
		IRQ6                  = 0x26,
		IRQ7                  = 0x27,
		IRQ8                  = 0x28,
		IRQ9                  = 0x29,
		IRQ10                 = 0x2A,
		IRQ11                 = 0x2B,
		IRQ12                 = 0x2C,
		IRQ13                 = 0x2D,
		IRQ14                 = 0x2E,
		IRQ15                 = 0x2F,

		SystemTimer           = IRQ0,
		Keyboard              = IRQ1,
		COM2Default           = IRQ3,
		COM4User              = IRQ3,
		COM1Default           = IRQ4,
		COM3User              = IRQ4,
		ParallelPort2         = IRQ5,
		FloppyDiskController  = IRQ6,
		ParallelPort1         = IRQ7,
		SoundCard             = IRQ7,
		RealTimeClock         = IRQ8,
		PS2Mouse              = IRQ12,
		ISA                   = IRQ13,
		CoProcessor386        = IRQ13,
		PrimaryIDE            = IRQ14,
		SecondaryIDE          = IRQ15,
  };


private:
  static HandlerInfo _handlers[256];

public:
  static void Initialize();

  static void RegisterInterrupt(uint8 n, Handler handler, void* data);
  static void UnregisterInterrupt(uint8 interrupt);

private:
  static void EncodeIdtEntry(uint8 num, uint32 base, uint16 sel, uint8 flags);

public:
  static void IsrCommon (); 
  static void IsrEnd (); 

  static bool InitHandlers();
};

}
