//------------------------------------------------------------------------------
// screen.h
//	Definition of the Screen class
//------------------------------------------------------------------------------
// Copyright (c) 2008, Cedric Rousseau
// All rights reserved.
// 
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

  class Screen
  {
  private:
	  static uint16 cursor_x;
	  static uint16 cursor_y;
	  static uint16* video_memory;

  public:
    static void Initialize();

	  // Clears the screen and puts the cursor in top left
	  static void Clear();

	  // Outputs a single character to the screen.
    static void WriteChar(char c);
    
    // Outputs a null-terminated ASCII string to the screen.
	  static void WriteString(const char* string);
	  
    // Outputs a hexadecimal number to the screen.
	  static void WriteHex(uint32 value, uint8 bits = 32);
    static void WriteHex(intptr value) { WriteHex((uint32)value, 32); }

	  // Outputs a decimal number to the screen.
	  static void WriteInt(uint32 value);

    static void DumpMemory(intptr start, uint32 size);

  private:
	  static void Scroll();
	  static void SetCursor();
  };

}