//------------------------------------------------------------------------------
// serial.cpp
//	serial port manager
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

#include "serial.h"

#include "intmgr.h"
#include "screen.h"
#include "hal/uart.h"

using namespace GenOS;
using namespace GenOS::HAL;

SerialPort::SerialPort(uint32 port)
{
  _port = port;
  HAL::UART::Initialize (_port);
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
  //Screen::cout << "[SERIAL] IRQ port #" << that->_port << Screen::endl;

  const uint8 readValue = HAL::UART::ReadByte(that->_port, false);

  //Screen::cout << "[SERIAL] queue '" << (char)readValue << "' (0x" << readValue << ")" << Screen::endl;
  that->_cache.Queue (readValue);

  if (that->_handler.Address)
    that->_handler.Address(regs, that->_handler.Data);
}


void SerialPort::WriteByte (uint8 character)
{
  //Screen::cout << "[SERIAL] write '" << (char)character << "' (0x" << character << ")" << Screen::endl;
  HAL::UART::WriteByte (_port, character, true);
}

uint8 SerialPort::ReadByte ()
{
  uint8 value = 0;
  if (_cache.Dequeue(value))
  {
    //Screen::cout << "[SERIAL] dequeue '" << (char)value << "' (0x" << value << ")" << Screen::endl;
    return value;
  }

  value = HAL::UART::ReadByte (_port, true);
  //Screen::cout << "[SERIAL] read '" << (char)value << "' (0x" << value << ")" << Screen::endl;
  return value;
}

SerialPort SerialPort::s_COM1(0);
SerialPort SerialPort::s_COM2(1);
SerialPort SerialPort::s_COM3(2);
SerialPort SerialPort::s_COM4(3);

bool SerialPort::s_isCOM1Available = true;
bool SerialPort::s_isCOM2Available = true;
bool SerialPort::s_isCOM3Available = false;
bool SerialPort::s_isCOM4Available = false;

SerialPort* SerialPort::Acquire(IOPort::Ports port)
{
  //Screen::cout << "[SERIAL] acquire COM #" << (uint16)port << Screen::endl;

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
