//------------------------------------------------------------------------------
// PIC.cpp
//	HAL - Programmable Interrupt Controller
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

#include "PIC.h"

#include "../bitmanip.h"
#include "ioports.h"

using namespace GenOS::HAL;


// The number od interrupt controllers
static const uint8 PIC_CTRLS = 2;

// The number of interrupts each controller handles
static const uint8 PIC_INT_PER_CTRL = 8;

// PIC commands
//static const uint8 PIC_COM_ROT_EOI_MODE = 0x00;
//static const uint8 PIC_COM_READ_IRR     = 0x0A; // Read interrupt request register
//static const uint8 PIC_COM_READ_ISR     = 0x0B; // Read interrupt in-service register
//static const uint8 PIC_COM_NOP          = 0x40; // No operation
//static const uint8 PIC_COM_CLR_SMASK    = 0x48; // Clear special mask mode
static const uint8 PIC_COM_EOI          = 0x20; // End of interrupt
//static const uint8 PIC_COM_EOF_SPC      = 0x60; // End of specific interrupt of IRQ BASE

// PIC ports
// [In] Read-int-requests or service-registers */
//static const uint16 read_int_requests_port[] = {0x20, 0xA0};
// [Out] PIC command register
static const uint16 CMD_PORT[] = { IOPort::PIC_Master_Command, IOPort::PIC_Slave_Command };
// [In,Out] PIC data register
static const uint16 DATA_PORT[] = { IOPort::PIC_Master_Data, IOPort::PIC_Slave_Data };
///* I/O port: Trigger type */
//static const uint trigger_type_port[] = {0x4D0, 0x4D1};


PIC::PIC()
{
}

void PIC::Remap (uint8 offset0, uint8 offset1)
{
  // Save masks
  const uint8 savedMask0 = GetMask(0);
  const uint8 savedMask1 = GetMask(1);

  // Send initialize command
  SendCommand (0, 0x11);
  SendCommand (1, 0x11);

  SendData (0, offset0);
  SendData (1, offset1);

  SendData (0, 0x04);
  SendData (1, 0x02);

  SendData (0, 0x01);
  SendData (1, 0x01);

  // Restore masks.
  SetMask (0, savedMask0);
  SetMask (1, savedMask1);
}

void PIC::EnableInterrupt (uint8 interrupt)
{
	const uint8 ctrl = GetController(interrupt);
	interrupt %= PIC_INT_PER_CTRL;

  uint8 mask = GetMask(ctrl);
  BitManip::ClearBit (mask, interrupt);
	SetMask(ctrl, mask);
}

void PIC::DisableInterrupt (uint8 interrupt)
{
	const uint8 ctrl = GetController(interrupt);
	interrupt %= PIC_INT_PER_CTRL;

  uint8 mask = GetMask(ctrl);
  BitManip::SetBit (mask, interrupt);
	SetMask(ctrl, mask);
}


void PIC::EndOfInterrupt (uint8 interrupt)
{
  // If this interrupt involved the slave, send it EOI.
	if (GetController(interrupt) == 1) 
  {
		SendCommand(1, PIC_COM_EOI);
	}

	// In both cases we must send EOI to master
	SendCommand(0, PIC_COM_EOI);
}

uint8 PIC::GetController (uint8 interrupt)
{
	ASSERT(interrupt < PIC_CTRLS * PIC_INT_PER_CTRL);
	return (interrupt / PIC_INT_PER_CTRL);
}

void PIC::SendCommand (uint8 controller, uint8 command)
{
	ASSERT(controller < PIC_CTRLS);
  IOPort::Out8(CMD_PORT[controller], command);
}

void PIC::SendData (uint8 controller, uint8 data)
{
	ASSERT(controller < PIC_CTRLS);
  IOPort::Out8(DATA_PORT[controller], data);
}

uint8 PIC::GetMask (uint8 controller)
{
	ASSERT(controller < PIC_CTRLS);
  return IOPort::In8(DATA_PORT[controller]);
}

void PIC::SetMask (uint8 controller, uint8 mask)
{
	ASSERT(controller < PIC_CTRLS);
  IOPort::Out8(DATA_PORT[controller], mask);
}
