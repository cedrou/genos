//------------------------------------------------------------------------------
// screen.cpp
//	
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

#include "screen.h"
#include "ioports.h"
#include "serial.h"

using namespace GenOS;

Screen Screen::cout;
const char* Screen::endl = "\r\n";

void Screen::Initialize()
{
  cout.dumpPort = SerialPort::Acquire(IOPort::COM2);

  cout.cursor_x = 0;
  cout.cursor_y = 0;
  cout.video_memory = ((uint16*)0xB8000);
  cout.video_length = 0x4000;

  cout.Clear();
}

// Writes a single character out to the screen.
void Screen::WriteChar(char c)
{
  dumpPort->Write(c);

	// Backspace, move the cursor back one space and put a space
	if (c == 0x08 && cursor_x)
	{
		if (cursor_x)
		{
			cursor_x--;
		}
		else if(cursor_y)
		{
			cursor_x = 79;
			cursor_y--;
		}

		*(char*)(&video_memory[cursor_y*80 + cursor_x]) = ' ';
	}

	// Handle a tab by increasing the cursor's X, but only to a point
	// where it is divisible by 8.
	else if (c == 0x09)
	{
		cursor_x = (cursor_x+7) & ~7;
	}

	// Handle carriage return
	else if (c == '\r')
	{
		cursor_x = 0;
	}

	// Handle newline by moving cursor back to left and increasing the row
	else if (c == '\n')
	{
		cursor_x = 0;
		cursor_y++;
	}
	
	// Handle any other printable character.
	else if(c >= ' ')
	{
    // The background colour is black (0), the foreground is white (15).
    uint8 backColour = 0;
    uint8 foreColour = 15;

    // The attribute byte is made up of two nibbles - the lower being the
    // foreground colour, and the upper the background colour.
    uint8  attributeByte = (backColour << 4) | (foreColour & 0x0F);
    // The attribute byte is the top 8 bits of the word we have to send to the
    // VGA board.
    uint16 attribute = attributeByte << 8;

	  video_memory[cursor_y*80 + cursor_x] = c | attribute;
		cursor_x++;
	}

	// Check if we need to insert a new line because we have reached the end
	// of the screen.
	if (cursor_x >= 80)
	{
		cursor_x = 0;
		cursor_y ++;
	}

	// Scroll the screen if needed.
	Scroll();
	// Move the hardware cursor.
	SetCursor();
}

// Clears the screen.
void Screen::Clear()
{
	for (int i = 0; i < 80*25; i++)
	{
		video_memory[i] = 0x0F20;
	}

	// Move the hardware cursor back to the start.
	cursor_x = 0;
	cursor_y = 0;
	SetCursor();
} 

// Outputs a null-terminated ASCII string to the screen.
void Screen::WriteString(const char* string)
{
	while (*string) WriteChar(*string++);
} 

// Outputs a hexadecimal number to the screen.
void Screen::WriteHex(uint32 value, uint8 bits)
{
  uint8 max = bits/4;
	for( uint8 i=0; i<max; i++)
	{
		WriteChar("0123456789ABCDEF"[((value >> ((max-1-i)*4)) & 0xF)]);
	}
} 

// Outputs a decimal number to the screen.
void Screen::WriteInt(uint32 /*value*/)
{
} 

void Screen::DumpMemory(intptr start, uint32 size)
{
  uint8* buffer = (uint8*)start;
  for (uint32 i=0; i<size; i++)
  {
    if(i%16==0)
    {
      WriteHex((uint32)buffer,32);
      WriteString(" | ");
    }
    WriteHex(*buffer++,8);
    WriteChar(' ');
    if(i%16==15)
    {
      WriteChar('\n');
    }
  }
  WriteChar('\n');
}


// Scrolls the text on the screen up by one line.
void Screen::Scroll()
{
	// Row 25 is the end, this means we need to scroll up
	if(cursor_y < 25) return;

	// Move the current text chunk that makes up the screen
	// back in the buffer by a line
	memcpy((intptr)video_memory, (intptr)(video_memory + 80), 24*2*80);

	// The last line should now be blank. Do this by writing
	// 80 spaces to it.
	uint8  attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
	uint16 blank = 0x20 /* space */ | (attributeByte << 8);

	for (int i = 24*80; i < 25*80; i++)
	{
		 video_memory[i] = blank;
	}

	// The cursor should now be on the last line.
	cursor_y = 24;
} 

// Move the cursor
void Screen::SetCursor()
{
	// The screen is 80 characters wide...
	uint16 cursorLocation = cursor_y * 80 + cursor_x;

  // Set the high cursor byte.
	IOPort::Out8(IOPort::CGA_selector, 0x0E);                  
	IOPort::Out8(IOPort::CGA_data, (cursorLocation >> 8) & 0xFF);
  // Set the low cursor byte.
	IOPort::Out8(IOPort::CGA_selector, 0x0F);                  
	IOPort::Out8(IOPort::CGA_data, cursorLocation & 0xFF);
}


Screen& Screen::operator <<(uint8 n)
{
  WriteHex(n,8);
  return *this;
}

Screen& Screen::operator <<(uint16 n)
{
  WriteHex(n,16);
  return *this;
}

Screen& Screen::operator <<(uint32 n)
{
  WriteHex(n,32);
  return *this;
}

Screen& Screen::operator <<(intptr n)
{
  WriteHex((uint32)n,32);
  return *this;
}

Screen& Screen::operator <<(char c)
{
  WriteChar(c);
  return *this;
}

Screen& Screen::operator <<(const char* s)
{
  WriteString(s);
  return *this;
}

