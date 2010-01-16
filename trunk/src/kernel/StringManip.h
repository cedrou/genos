//------------------------------------------------------------------------------
// StringManip.h
//	String Manipulation methods
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

#include <Kernel.h>

namespace GenOS
{

  class StringManip
  {
  public:
    static bool IsHexDigit (char c)
    {
      return ( (c >= '0') && (c <= '9') ) || ( (c >= 'A') && (c <= 'F') ) || ( (c >= 'a') && (c <= 'f') );
    }

    static uint8 FromHexDigit (char c)
    {
      if ( (c >= '0') && (c <= '9') )
        return c - '0';
      if ( (c >= 'A') && (c <= 'F') )
        return 10 + c - 'A';
      if ( (c >= 'a') && (c <= 'f') )
        return 10 + c - 'a';

      PANIC("Invalid format for an hexadecimal digit");
    }

    static char ToHexDigit (uint8 value)
    {
      const char* hexDigit = "0123456789ABCDEF";
      if ( value > 15 )
        PANIC("Invalid value for an hexadecimal digit");
      return hexDigit[value];
    }

    static char* ToHexString (uint8 value, char* string)
    {
			*string++ = ToHexDigit ((value >> 4) & 0x0F);
			*string++ = ToHexDigit (value & 0x0F);
			return string;
    }
    
    static char* ToHexString (uint16 value, char* string)
    {
      string = ToHexString ((uint8)(value & 0xFF), string);
      string = ToHexString ((uint8)((value >> 8) & 0xFF), string);
      return string;
    }

    static char* ToHexString (uint32 value, char* string)
    {
      string = ToHexString ((uint16)(value & 0xFFFF), string);
      string = ToHexString ((uint16)((value >> 16) & 0xFFFF), string);
      return string;
    }

    static char* ToHexString (uint64 value, char* string)
    {
      string = ToHexString ((uint32)(value & 0xFFFFFFFF), string);
      string = ToHexString ((uint32)((value >> 32) & 0xFFFFFFFF), string);
      return string;
    }

    static char* ToHexString (const intptr buffer, size_t count, char* string)
    {
      const uint8* bytes = (const uint8*)buffer;
      for (size_t i = 0; i < count; i++)
      {
        string = ToHexString (*bytes++, string);
      }
      return string;
    }
  };

}