//------------------------------------------------------------------------------
// DebuggerClient.cpp
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

#include "DebuggerClient.h"

#include "Screen.h"
#include "Serial.h"
#include "StringManip.h"

using namespace GenOS;

static GDBProtocol s_protocol;

DebuggerClient::DebuggerClient(void)
{
}

DebuggerClient::~DebuggerClient(void)
{
}

void DebuggerClient::Initialize()
{
  _port = SerialPort::Acquire(IOPort::COM1);
  _protocol = &s_protocol; //new GDBProtocol();
  //const char* message = _protocol->ReadMessage(_port);
  //if (message == NULL)
  //  return;

  //_protocol->InterpretMessage(message, _port);
  _port->RegisterHandler (&DebuggerClient::Serial_OnReceive, this); 
}

void __stdcall DebuggerClient::Serial_OnReceive(const Registers& regs, void* data)
{
  DebuggerClient* that = (DebuggerClient*)data;

  const char* message = that->_protocol->ReadMessage(that->_port);
  that->_protocol->InterpretMessage(message, that->_port, regs);
}


const size_t GDBProtocol_bufferSize = 4095;
char GDBProtocol::s_buffer[GDBProtocol_bufferSize + 1] = {0};

const char* GDBProtocol::ReadMessage(SerialPort* port)
{
  // All GDB commands and responses (other than acknowledgments) are sent as a packet. 
  // A packet is introduced with the character `$', the actual packet-data, and the 
  // terminating character `#' followed by a two-digit checksum:
  //    $packet-data#checksum
  // The two-digit checksum is computed as the modulo 256 sum of all characters between 
  // the leading `$' and the trailing `#' (an eight bit unsigned checksum).

  char* pointer = 0;
  while (1)
  {
    pointer = s_buffer;
    
    *pointer = 0;

    uint8 computedChkSum = 0;
    uint8 value = 0;

    // Get start of packet
    value = port->Read();
    if (value != '$')
      return NULL;

    *pointer++ = value;

    // Get end of packet-data
    value = 0; // must reset the value for the checksum
    while (value != '#') 
    {
      // add previous value to check sum
      computedChkSum += value;

      // read current value
      value = port->Read();

      // store this value into the array
      *pointer++ = value;
    }

    // read the packet checksum
    uint8 readChkSum = 0;

    value = port->Read();
    *pointer++ = value;
    readChkSum += (StringManip::FromHexDigit(value) << 4);

    value = port->Read();
    *pointer++ = value;
    readChkSum += StringManip::FromHexDigit(value);

    *pointer = 0;

    if (readChkSum == computedChkSum)
      break;
    
    // if checksums are different, request a retransmission
    port->Write('-');
  }

  port->Write('+');

  Screen::cout << "[GDB] Packet received: " << s_buffer << Screen::endl;
  return s_buffer + 1;
}

void GDBProtocol::SendMessage (const char* message, SerialPort* port)
{
  while (1)
  {
    Screen::cout << "[GDB] Sending message: " << message << Screen::endl;

    port->Write ('$');

    uint8 chkSum = 0;
    const char* p = message;

    while (*p != 0)
    {
      port->Write (*p);
      chkSum += *p;
      p++;
    }

    port->Write ('#');

    port->Write (StringManip::ToHexDigit((chkSum >> 4) & 0x0F));
    port->Write (StringManip::ToHexDigit(chkSum & 0x0F));

    uint8 ack = port->Read();
    Screen::cout << "[GDB] read ack: " << (char)ack << Screen::endl;
    if (ack == '+')
      break;
  }
}


static char s_registers[353] = {0};


static const char* ReadBigEndian (uint32& value, const char* string)
{
  value = 0;
  for (int i=0; i<8; i++)
  {
    char digit = *string;
    if( !StringManip::IsHexDigit(digit) )
      break;

    value = (value << 4) + StringManip::FromHexDigit(digit);
    string++;
  }

  return string;      
}


void GDBProtocol::InterpretMessage (const char* message, SerialPort* port, const Registers& regs)
{
  if (message == NULL)
    return;

  // see http://sourceware.org/gdb/current/onlinedocs/gdb/General-Query-Packets.html
  // A GDB debugging stub is required to support at lease 'g', 'G', 'm', 'M', 's', and 'c' commands.
  // All other commands are optional.
  switch (message[0])
  {
  case 'q':
    if (memcmp ((intptr)message, "qSupported", 10) == 0)
    {
      SendMessage("PacketSize=FFF", port);
      break;
    }
    if (memcmp ((intptr)message, "qAttached", 10) == 0)
    {
      SendMessage("1", port);
      break;
    }
    SendMessage("", port);
    break;

  case 'H':
    // set thread	- Hct...
    // Set thread for subsequent operations (`m', `M', `g', `G', et.al.). 
    // c = `c' for thread used in step and continue; t... can be -1 for all threads. 
    // c = `g' for thread used in other operations. If zero, pick a thread, any thread.
    // reply "OK" for success
    // reply "E<nn>" for an error
    SendMessage("", port);
    break;
    
  case '?':
    // last signal - ?
    // Indicate the reason the target halted.
    // reply
    SendMessage("S05", port);
    break;
    
  case 'g':
    {
      char* response = s_registers;
		  /*000*/ response = StringManip::ToHexString (regs.eax, response);
		  /*008*/ response = StringManip::ToHexString (regs.ecx, response);
		  /*016*/ response = StringManip::ToHexString (regs.edx, response);
		  /*024*/ response = StringManip::ToHexString (regs.ebx, response);
		  /*032*/ response = StringManip::ToHexString (regs.esp, response);
		  /*040*/ response = StringManip::ToHexString (regs.ebp, response);
		  /*048*/ response = StringManip::ToHexString (regs.esi, response);
		  /*056*/ response = StringManip::ToHexString (regs.edi, response);
		  /*064*/ response = StringManip::ToHexString (regs.eip, response);
		  /*072*/ response = StringManip::ToHexString (regs.eflags, response);
		  /*080*/ response = StringManip::ToHexString (regs.cs, response);
      /*088*/ response = StringManip::ToHexString (regs.ss, response);
      /*096*/ response = StringManip::ToHexString (regs.ds, response);
      /*104*/ response = StringManip::ToHexString (regs.ds, response); // es
      /*112*/ response = StringManip::ToHexString (regs.ds, response); // fs
      /*120*/ response = StringManip::ToHexString (regs.ds, response); // gs
      /*128*/ response = StringManip::ToHexString ((uint64)0, response); response = StringManip::ToHexString ((uint16)0, response); // st(0)=fpustate[28]
      /*148*/ response = StringManip::ToHexString ((uint64)0, response); response = StringManip::ToHexString ((uint16)0, response); // st(1)
      /*168*/ response = StringManip::ToHexString ((uint64)0, response); response = StringManip::ToHexString ((uint16)0, response); // st(2)
      /*188*/ response = StringManip::ToHexString ((uint64)0, response); response = StringManip::ToHexString ((uint16)0, response); // st(3)
      /*208*/ response = StringManip::ToHexString ((uint64)0, response); response = StringManip::ToHexString ((uint16)0, response); // st(4)
      /*228*/ response = StringManip::ToHexString ((uint64)0, response); response = StringManip::ToHexString ((uint16)0, response); // st(5)
      /*248*/ response = StringManip::ToHexString ((uint64)0, response); response = StringManip::ToHexString ((uint16)0, response); // st(6)
      /*268*/ response = StringManip::ToHexString ((uint64)0, response); response = StringManip::ToHexString ((uint16)0, response); // st(7)=fpustate[98]
      /*288*/ response = StringManip::ToHexString ((uint32)0, response); // fctrl
      /*296*/ response = StringManip::ToHexString ((uint32)0, response); // fstat
      /*304*/ response = StringManip::ToHexString ((uint32)0, response); // ftag
      /*312*/ response = StringManip::ToHexString ((uint32)0, response); // fiseg
      /*320*/ response = StringManip::ToHexString ((uint32)0, response); // fioff
      /*328*/ response = StringManip::ToHexString ((uint32)0, response); // foseg
      /*336*/ response = StringManip::ToHexString ((uint32)0, response); // fooff
      /*344*/ response = StringManip::ToHexString ((uint32)0, response); // fof
      /*352*/ 
      SendMessage(s_registers, port);
    }
    break;

  case 'm':
    {
      uint32 address = 0;
      uint32 count = 0;

      message = ReadBigEndian (address, message + 1);
    Screen::cout << "[GDB] address: " << address << Screen::endl;
      message++; // skip ','
      message = ReadBigEndian (count, message);
    Screen::cout << "[GDB] count: " << count << Screen::endl;

      if (count > GDBProtocol_bufferSize)
      {
        SendMessage("E00", port);
      }
      else
      {
        char* response = StringManip::ToHexString((intptr)address, count, s_buffer);
        *response = 0;
        SendMessage(s_buffer, port);
      }
    }
    break;

  case 'G':
  case 'M':
  case 's':
  case 'c':
  default:
    SendMessage("", port);
    break;
  }
}
