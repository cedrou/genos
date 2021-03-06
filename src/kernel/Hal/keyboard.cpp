//------------------------------------------------------------------------------
// keyboard.cpp
//	HAL - Keyboard
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

#include "keyboard.h"
#include <HAL/ioports.h>

using namespace GenOS::HAL;

uint8 Keyboard::GetData(bool wait)
{
  // wait while input buffer is empty
  while (wait && (IOPort::In8 (IOPort::KBD_Status) & 0x01) == 0)
    ;

  return IOPort::In8 (IOPort::KBD_Data);
}

uint32 Keyboard::GetScanCode ()
{
  uint32 scancode = GetData();

  if (scancode == 0xE0) // escape scancode
    scancode = 0xE000 + GetData();

  //else if (scancode = 0xE1) // escape (make+break) : only for [Pause/Break] key
  //  scancode = 

  return scancode;
}

void Keyboard::SetLEDs(LED leds)
{
  // wait while input buffer is full
  while ((IOPort::In8 (IOPort::KBD_Status) & 0x02) != 0)
    ;

  // Send command "Set LEDS"
  IOPort::Out8 (IOPort::KBD_Data, 0xED);

  // wait while input buffer is full
  while ((IOPort::In8 (IOPort::KBD_Status) & 0x02) != 0)
    ;

  // Send LEDs config
  IOPort::Out8 (IOPort::KBD_Data, (uint8)leds);
}