//------------------------------------------------------------------------------
// serial.cpp
//	serial port manager
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

#include "serial.h"

#include "intmgr.h"
#include "screen.h"

using namespace GenOS;
using namespace GenOS::HAL;

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

SerialPort::SerialPort(uint32 port)
{
  const IOPort::Ports bases[] = { IOPort::COM1, IOPort::COM2, IOPort::COM3, IOPort::COM4 };
  _base = bases[port - 1];

	// Disable all UART interrupts
  IOPort::Out8 ((uint16)(_base + IOPort::COM_IER), 0x00);          

	// Enable DLAB (set baud rate divisor)
  IOPort::Out8 ((uint16)(_base + IOPort::COM_LCR), (uint8)LCR::DLAB);           

	// Set Baud rate
	int baudRate = 33800;
	int divisor = 115200 / baudRate;
	IOPort::Out8 ((uint16)(_base + IOPort::COM_DLL), (uint8)(divisor & 0xFF));
	IOPort::Out8 ((uint16)(_base + IOPort::COM_DLM), (uint8)(divisor>>8 & 0xFF));

	// Reset DLAB, Set 8 bits, no parity, one stop bit
  IOPort::Out8 ((uint16)(_base + IOPort::COM_LCR), (uint8)(LCR::CS8 | LCR::ST1 | LCR::PNO));

	// Enable FIFO, clear them, with 14-byte threshold
  IOPort::Out8 ((uint16)(_base + IOPort::COM_FCR), (uint8)(FCR::Enabled | FCR::CLR_RCVR | FCR::CLR_XMIT | FCR::TL14));

	// IRQs enabled, RTS/DSR set
  IOPort::Out8 ((uint16)(_base + IOPort::COM_MCR), (uint8)(MCR::DTR | MCR::RTS | MCR::OUT2));     
  
	// Interrupt when data received
  IOPort::Out8 ((uint16)(_base + IOPort::COM_IER), (uint8)IER::DR);
}

void SerialPort::RegisterHandler(InterruptManager::Handler handler, void* data)
{
  _handler.Address = handler;
  _handler.Data = data;
}

void SerialPort::UnregisterHandler()
{
  _handler.Address = NULL;
  _handler.Data = NULL;
}

void SerialPort::InterruptHandler(const Registers& regs, void* data)
{
  SerialPort* that = ((SerialPort*)data);

  const uint8 readValue = that->ReadByteNoWait();
  //Screen::cout << "[SERIAL] queue '" << (char)readValue << "' (0x" << readValue << ")" << Screen::endl;
  that->_cache.Queue (readValue);

  if (that->_handler.Address)
    that->_handler.Address(regs, that->_handler.Data);
}


void SerialPort::Write (uint8 character) const
{
  while ((IOPort::In8 ((uint16)(_base + IOPort::COM_LSR)) & (uint8)LSR::THRE) == 0)
		;

  IOPort::Out8 ((uint16)(_base + IOPort::COM_THB), character);
  //Screen::cout << "[SERIAL] write '" << (char)character << "' (0x" << character << ")" << Screen::endl;
}

uint8 SerialPort::Read ()
{
  uint8 value = 0;
  if (_cache.Dequeue(value))
  {
    //Screen::cout << "[SERIAL] dequeue '" << (char)value << "' (0x" << value << ")" << Screen::endl;
    return value;
  }

	while ((IOPort::In8 ((uint16)(_base + IOPort::COM_LSR)) & (uint8)LSR::DR) == 0)
		;

  value = IOPort::In8 ((uint16)(_base + IOPort::COM_RBR));
  //Screen::cout << "[SERIAL] read '" << (char)value << "' (0x" << value << ")" << Screen::endl;
  return value;
}

uint8 SerialPort::ReadByteNoWait () const
{
  uint8 value = IOPort::In8 ((uint16)(_base + IOPort::COM_RBR));
  //Screen::cout << "[SERIAL] read nowait '" << (char)value << "' (0x" << value << ")" << Screen::endl;
  return value;
}

SerialPort SerialPort::s_COM1(1);
SerialPort SerialPort::s_COM2(2);
SerialPort SerialPort::s_COM3(3);
SerialPort SerialPort::s_COM4(4);

bool SerialPort::s_isCOM1Available = true;
bool SerialPort::s_isCOM2Available = true;
bool SerialPort::s_isCOM3Available = false;
bool SerialPort::s_isCOM4Available = false;

SerialPort* SerialPort::Acquire(IOPort::Ports port)
{
  switch (port)
  {
  case IOPort::COM1:
    if (!s_isCOM1Available)
      return NULL;
    s_isCOM1Available = false;
    InterruptManager::RegisterInterrupt(InterruptManager::COM1Default, &SerialPort::InterruptHandler, &s_COM1);
    return &s_COM1;

  case IOPort::COM2:
    if (!s_isCOM2Available)
      return NULL;
    s_isCOM2Available = false;
    InterruptManager::RegisterInterrupt(InterruptManager::COM2Default, &SerialPort::InterruptHandler, &s_COM2);
    return &s_COM2;

  case IOPort::COM3:
    if (!s_isCOM3Available)
      return NULL;
    s_isCOM3Available = false;
    //InterruptManager::RegisterInterrupt(InterruptManager::COM3User, &SerialPort::InterruptHandler, &s_COM3);
    return &s_COM3;

  case IOPort::COM4:
    if (!s_isCOM4Available)
      return NULL;
    s_isCOM4Available = false;
    //InterruptManager::RegisterInterrupt(InterruptManager::COM4User, &SerialPort::InterruptHandler, &s_COM4);
    return &s_COM4;
  }

  return NULL;
}

void SerialPort::Release(IOPort::Ports port)
{
  switch (port)
  {
  case IOPort::COM1:
    s_isCOM1Available = true;
    InterruptManager::UnregisterInterrupt(InterruptManager::COM1Default);
    break;

  case IOPort::COM2:
    s_isCOM2Available = true;
    InterruptManager::UnregisterInterrupt(InterruptManager::COM2Default);
    break;

  case IOPort::COM3:
    s_isCOM3Available = true;
    InterruptManager::UnregisterInterrupt(InterruptManager::COM3User);
    break;

  case IOPort::COM4:
    s_isCOM4Available = true;
    InterruptManager::UnregisterInterrupt(InterruptManager::COM4User);
    break;
  }
}
