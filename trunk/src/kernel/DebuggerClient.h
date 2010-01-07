//------------------------------------------------------------------------------
// DebuggerClient.h
//	Client stub for remote debugger
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

#include "registers.h"

namespace GenOS
{

  class SerialPort;

  class DebuggerProtocol
  {
  public:
    virtual const char* ReadMessage(SerialPort* port) = 0;

    virtual void InterpretMessage (const char* message, SerialPort* port, const Registers& regs) = 0;
  };

  class GDBProtocol : public DebuggerProtocol
  {
  private:
    static char s_buffer[];

  public:
    virtual const char* ReadMessage(SerialPort* port);
    virtual void SendMessage (const char* message, SerialPort* port);

    virtual void InterpretMessage (const char* message, SerialPort* port, const Registers& regs);
  };

  class DebuggerClient
  {
  private:
    SerialPort* _port;
    DebuggerProtocol* _protocol;

  public:
    DebuggerClient();
    ~DebuggerClient();

    void Initialize();

  private:
    static void __stdcall Serial_OnReceive(const Registers& regs, void* data);

  };

}