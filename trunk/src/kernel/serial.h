//------------------------------------------------------------------------------
// serial.h
//	serial port management
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

#include "fifo.h"
#include "ioports.h"
#include "intmgr.h"

namespace GenOS 
{

  class SerialPort
  {
  private:
    static SerialPort s_COM1;
    static SerialPort s_COM2;
    static SerialPort s_COM3;
    static SerialPort s_COM4;

    static bool s_isCOM1Available;
    static bool s_isCOM2Available;
    static bool s_isCOM3Available;
    static bool s_isCOM4Available;

  private:
    IOPort::Ports                 _base;
    InterruptManager::HandlerInfo _handler;
    Fifo<uint8, 1024>             _cache;

  public: 
    static SerialPort* Acquire(IOPort::Ports port);
    static void Release(IOPort::Ports port);

    void RegisterHandler(InterruptManager::Handler handler, void* data = NULL);
    void UnregisterHandler();

    void Write (uint8 character) const;
    uint8 Read ();

    uint8 ReadByteNoWait () const;


  private:
    SerialPort(uint32 port);
    static void __stdcall InterruptHandler(const Registers& regs, void* data);
  };

}
