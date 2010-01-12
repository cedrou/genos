//------------------------------------------------------------------------------
// UART.cpp
//	HAL - Universal Asynchronous Receiver/Transmitter
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

#include "UART.h"
#include "ioports.h"


namespace GenOS {
  namespace HAL {

class IER { public: enum values
{
	DR = 0x01,    // Data ready, it is generated if data waits to be read by the CPU.
	THRE = 0x02,  // THR Empty, this interrupt tells the CPU to write characters to the THR.
	SI = 0x04,    // Status interrupt. It informs the CPU of occurred transmission errors during reception.
	MSI = 0x08    // Modem status interrupt. It is triggered whenever one of the delta-bits is set (see MSR).
};};

class LCR { public: enum values
{
  // Word length
  CS5 = 0x00, // 5bits
  CS6 = 0x01, // 6bits
  CS7 = 0x02, // 7bits
  CS8 = 0x03, // 8bits
  // Stop bit
  ST1 = 0x00, // 1
  ST2 = 0x04, // 2
  // Parity
  PNO = 0x00, // None
  POD = 0x08, // Odd
  PEV = 0x18, // Even
  PMK = 0x28, // Mark 
  PSP = 0x38, // Space

  BRK = 0x40,
  DLAB = 0x80,
};};

class FCR { public: enum values
{
	Enabled = 0x01,   // FIFO enable.
	CLR_RCVR = 0x02,  // Clear receiver FIFO. This bit is self-clearing.
	CLR_XMIT = 0x04,  // Clear transmitter FIFO. This bit is self-clearing.
	DMA = 0x08,       // DMA mode
	// Receiver FIFO trigger level
	TL1 = 0x00,
	TL4 = 0x40,
	TL8 = 0x80,
	TL14 = 0xC0,
};};

class MCR { public: enum values
{
	DTR = 0x01,
	RTS = 0x02,
	OUT1 = 0x04,
	OUT2 = 0x08,
	LOOP = 0x10,
};};

class LSR { public: enum values
{
	DR = 0x01, // Data Ready. Reset by reading RBR (but only if the RX FIFO is empty, 16550+).
	OE = 0x02, // Overrun Error. Reset by reading LSR. Indicates loss of data.
	PE = 0x04, // Parity Error. Indicates transmission error. Reset by LSR.
	FE = 0x08, // Framing Error. Indicates missing stop bit. Reset by LSR.
	BI = 0x10, // Break Indicator. Set if RxD is 'space' for more than 1 word ('break'). Reset by reading LSR.
	THRE = 0x20, // Transmitter Holding Register Empty. Indicates that a new word can be written to THR. Reset by writing THR.
	TEMT = 0x40, // Transmitter Empty. Indicates that no transmission is running. Reset by reading LSR.
};};

  }
}

using namespace GenOS::HAL;

static const IOPort::Ports bases[] = { IOPort::COM1, IOPort::COM2, IOPort::COM3, IOPort::COM4 };

void UART::Initialize (uint32 port)
{
	// Disable all UART interrupts
  IOPort::Out8 ((uint16)(bases[port] + IOPort::COM_IER), 0x00);          

	// Enable DLAB (set baud rate divisor)
  IOPort::Out8 ((uint16)(bases[port] + IOPort::COM_LCR), (uint8)LCR::DLAB);           

	// Set Baud rate
	int baudRate = 38400;
	int divisor = 115200 / baudRate;
	IOPort::Out8 ((uint16)(bases[port] + IOPort::COM_DLL), (uint8)(divisor & 0xFF));
	IOPort::Out8 ((uint16)(bases[port] + IOPort::COM_DLM), (uint8)(divisor>>8 & 0xFF));

	// Reset DLAB, Set 8 bits, no parity, one stop bit
  IOPort::Out8 ((uint16)(bases[port] + IOPort::COM_LCR), (uint8)(LCR::CS8 | LCR::ST1 | LCR::PNO));

	// Enable FIFO, clear them, with 14-byte threshold
  IOPort::Out8 ((uint16)(bases[port] + IOPort::COM_FCR), (uint8)(FCR::Enabled | FCR::CLR_RCVR | FCR::CLR_XMIT | FCR::TL14));

	// IRQs enabled, RTS/DSR set
  IOPort::Out8 ((uint16)(bases[port] + IOPort::COM_MCR), (uint8)(MCR::DTR | MCR::RTS | MCR::OUT2));     
  
	// Interrupt when data received
  IOPort::Out8 ((uint16)(bases[port] + IOPort::COM_IER), (uint8)IER::DR);
}

bool UART::CanWrite (uint32 port)
{
  return (IOPort::In8 ((uint16)(bases[port] + IOPort::COM_LSR)) & (uint8)LSR::THRE) != 0;
}

bool UART::CanRead (uint32 port)
{
  return (IOPort::In8 ((uint16)(bases[port] + IOPort::COM_LSR)) & (uint8)LSR::DR) != 0;
}

void UART::WriteByte (uint32 port, uint8 character, bool wait)
{
  while (wait && !CanWrite(port))
		;

  IOPort::Out8 ((uint16)(bases[port] + IOPort::COM_THB), character);
}

uint8 UART::ReadByte (uint32 port, bool wait)
{
	while (wait && !CanRead(port))
		;

  return IOPort::In8 ((uint16)(bases[port] + IOPort::COM_RBR));
}
