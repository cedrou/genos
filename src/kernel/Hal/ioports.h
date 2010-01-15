//------------------------------------------------------------------------------
// ioports.h
//	HAL - IO hardware ports
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

#pragma once

#include "../common.h"

namespace GenOS {
  namespace HAL {

  class IOPort
  {
  public:
    enum Ports {

      // Programmable Interrupt (PIC) Controller
			PIC_Master_Command  = 0x0020,
			PIC_Master_Data     = 0x0021,
			PIC_Slave_Command   = 0x00A0,
			PIC_Slave_Data      = 0x00A1,

      // Primary Programmable Interrupt Timer (PIT) Controller
      PIT_0_Data          = 0x0040,   // r/w, Channel 0: counter divisor
      PIT_1_Data          = 0x0041,   // r/w, Channel 1: RAM refresh counter (obsolete, should not be used)
      PIT_2_Data          = 0x0042,   // r/w, Channel 2: speaker
      PIT_Command         = 0x0043,   // r/w
      PIT_3               = 0x0044,   // r/w
      PIT_3_Control       = 0x0047,   // r/w

      // Keyboard Controller
      KBD_Data            = 0x0060,   // r/w
      KBD_Status          = 0x0064,   // r
      KBD_Command         = 0x0064,   // w

      // CGA (Color Graphics Adapter)
      // Register list:
      //	0x00 horizontal total
			//  0x01 horizontal displayed
			//  0x02 horizontal sync position
			//  0x03 horizontal sync pulse width
			//  0x04 vertical total
			//  0x05 vertical displayed
			//  0x06 vertical sync position
			//  0x07 vertical sunc pulse width
			//  0x08 interlace mode
			//  0x09 maximum scan lines
			//  0x0A cursor start
			//  0x0B cursor end
			//  0x0C start address high (readable)
			//  0x0D start address low (readable)
			//  0x0E cursor location high (readable)
			//  0x0F cursor location low (readable)
			//  0x10 light pen high
			//  0x11 light pen low
			CGA_selector        = 0x03D4,   // w, selects which register (0-11h) is to be accessed through CGA_data
			CGA_data            = 0x03D5,   // (r/)w, write data in register selected by CGA_selector

      // Serial port
      COM1                = 0x03F8,
      COM2                = 0x02F8,
      COM3                = 0x03E8,
      COM4                = 0x02E8,
                  COM_RBR = 0,        // r, receiver buffer register
                  COM_THB = 0,        // w, transmitter holding buffer
                  COM_DLL = 0,        // w, divisor latch LSB
                  COM_IER = 1,        // r/w, Interrupt enable register
                  COM_DLM = 1,        // w, divisor latch MSB
                  COM_IIR = 2,        // r, Interrupt identification register
                  COM_FCR = 2,        // w, FIFO control register
                  COM_LCR = 3,        // r/w, line control register
                  COM_MCR = 4,        // r/w, modem control register
                  COM_LSR = 5,        // r, line status register
                  COM_MSR = 6,        // r, modem status register
                  COM_SCR = 7,        // r/w, scratch register

    } ;

  public:
    // Write a byte/word/dword out to the specified port.
    static void Out8(uint16 port, uint8 value);
    static void Out16(uint16 port, uint16 value);
    static void Out32(uint16 port, uint32 value);

    // Read a byte/word/dword in the specified port.
    static uint8  In8(uint16 port);
    static uint16 In16(uint16 port);
    static uint32 In32(uint16 port);
  };

}
}