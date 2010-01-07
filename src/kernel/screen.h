//------------------------------------------------------------------------------
// screen.h
//	Definition of the Screen class
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

#include "common.h"

namespace GenOS {

  class SerialPort;

  class Screen
  {
  public:
    static Screen cout;
    static const char* endl;

  private:
	  uint16  cursor_x;
	  uint16  cursor_y;
	  uint16* video_memory;
    uint16  video_length;
    
    SerialPort* dumpPort;

  public:
    static void Initialize();

	  // Clears the screen and puts the cursor in top left
	  void Clear();

    void DumpMemory(intptr start, uint32 size);

    Screen& operator <<(uint8 n);
    Screen& operator <<(uint16 n);
    Screen& operator <<(uint32 n);
    Screen& operator <<(intptr n);
    Screen& operator <<(char c);
    Screen& operator <<(const char* s);

  private:
	  // Outputs a single character to the screen.
    void WriteChar(char c);
    
    // Outputs a null-terminated ASCII string to the screen.
	  void WriteString(const char* string);
	  
    // Outputs a hexadecimal number to the screen.
	  void WriteHex(uint32 value, uint8 bits = 32);
    void WriteHex(intptr value) { WriteHex((uint32)value, 32); }

	  // Outputs a decimal number to the screen.
	  void WriteInt(uint32 value);

	  void Scroll();
	  void SetCursor();
  };

}