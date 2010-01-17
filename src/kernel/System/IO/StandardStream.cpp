//------------------------------------------------------------------------------
// StandardStream.h
//	Implementation of standard input and output streams
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

#include "StandardStream.h"

#include <Screen.h>

using namespace GenOS;
using namespace GenOS::System::IO;

StdInStream StdInStream::s_instance;

StdInStream* StdInStream::GetInstance()
{
  return &s_instance;
}

StdInStream::StdInStream()
{
}

StdInStream::~StdInStream()
{
}

//int StdInStream::Read (Array<uint8>& buffer, size_t offset, size_t count)
//{
//  ASSERT ( (offset + count) <= buffer.Size() );
//  for (size_t i = 0; i < count; i++)
//  {
//    buffer[offset + i] = (uint8)(ReadByte() & 0xFF);
//  }
//  return count;
//}

int StdInStream::ReadByte ()
{
  for(;;)
  {
    GenOS::Driver::Keys::values key = _kbd.ReadKey();
    if (key <= 0xFF)
      return key;
  }
}

void StdInStream::Close()
{
  NOTIMPLEMENTED;
}






StdOutStream StdOutStream::s_instance;

StdOutStream* StdOutStream::GetInstance()
{
  return &s_instance;
}

StdOutStream::StdOutStream()
{
}

StdOutStream::~StdOutStream()
{
}

//void StdOutStream::Write (const Array<uint8>& buffer, size_t offset, size_t count)
//{
//  ASSERT ( (offset + count) <= buffer.Size() );
//  for (size_t i = 0; i < count; i++)
//  {
//    Screen::cout << (char)buffer[offset + i];
//  }
//}

void StdOutStream::WriteString (const char* string)
{
  ASSERT (string != NULL);
  while (*string)
  {
    Screen::cout << (char)*string++;
  }
}

void StdOutStream::WriteByte (uint8 value)
{
  Screen::cout << (char)value;
}

void StdOutStream::Close()
{
  NOTIMPLEMENTED;
}

void StdOutStream::Flush()
{
  NOTIMPLEMENTED;
}
