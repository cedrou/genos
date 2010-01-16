//------------------------------------------------------------------------------
// DisplayCGA.cpp
//	HAL - CGA display
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

#include "DisplayCGA.h"
#include <HAL/ioports.h>

using namespace GenOS::HAL;

static       uint16*  CGA_MEMORY  = (uint16*)0xB8000;
static const size_t   CGA_LENGTH  = 0x4000;
static const size_t   CGA_COLUMNS = 80;
static const size_t   CGA_ROWS    = 25;

void DisplayCGA::Clear(uint8 backColor)
{
  const uint16 cgaChar = (((uint16)(backColor & 0x0F)) << 12) | (((uint16)(backColor & 0x0F)) << 8) | 0x20;
  // Fill buffer with spaces of color bgColor
	for (size_t i = 0; i < CGA_COLUMNS * CGA_ROWS; i++)
	{
		CGA_MEMORY[i] = cgaChar;
	}
}

// Move the cursor
void DisplayCGA::SetCursor(uint16 x, uint16 y)
{
	const uint16 location = (uint16)(y * CGA_COLUMNS + x);

  // Set the high cursor byte.
	IOPort::Out8(IOPort::CGA_selector, 0x0E);                  
	IOPort::Out8(IOPort::CGA_data, (location >> 8) & 0xFF);

  // Set the low cursor byte.
	IOPort::Out8(IOPort::CGA_selector, 0x0F);                  
	IOPort::Out8(IOPort::CGA_data, location & 0xFF);
}

void DisplayCGA::PutChar (char c, uint16 x, uint16 y)
{
	const uint16 location = (uint16)(y * CGA_COLUMNS + x);
  ASSERT (location < CGA_LENGTH);

  *(char*)(&CGA_MEMORY[location]) = c;
}

void DisplayCGA::PutChar (char c, uint16 x, uint16 y, uint8 backColor, uint8 foreColor)
{
	const uint16 location = (uint16)(y * CGA_COLUMNS + x);
  ASSERT (location < CGA_LENGTH);

  const uint8 attribute = (backColor << 4) | (foreColor & 0x0F);

  CGA_MEMORY[location] = (((uint16)attribute) << 8) | c;
}

void DisplayCGA::Scroll(uint16 lines)
{
  ASSERT ((lines * CGA_COLUMNS) < CGA_LENGTH);

	// Move the current text chunk that makes up the screen
	// back in the buffer by a line
	memcpy((intptr)CGA_MEMORY, (intptr)(CGA_MEMORY + lines * CGA_COLUMNS), (CGA_ROWS - lines) * 2 * CGA_COLUMNS);

	// The last line should now be blank. Do this by writing
	// 80 spaces to it.
 	const uint16 start = (uint16)((CGA_ROWS - lines) * CGA_COLUMNS + 0);
 	const uint16 end   = (uint16)(      CGA_ROWS * CGA_COLUMNS + 0);

	for (uint16 location = start; location < end; location++)
	{
		 *(char*)(&CGA_MEMORY[location]) = ' ';
	}
} 
