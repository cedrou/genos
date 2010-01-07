#pragma once

#include "Kernel.h"

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