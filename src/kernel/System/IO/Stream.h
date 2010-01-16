//------------------------------------------------------------------------------
// Stream.h
//	Abstract stream class
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

#include <array.h>


namespace GenOS {
  namespace System {
    namespace IO {


class Stream
{
public: 
  enum Origin {
    Begin,
    Current,
    End
  };

public:
  virtual int Read (GenOS::Array<uint8>& buffer, size_t offset, size_t count) = 0;
  virtual int ReadByte () = 0;

  virtual void Write (const Array<uint8>& buffer, size_t offset, size_t count) = 0;
  virtual void WriteByte (uint8 value) = 0;

  virtual size_t Seek (size_t offset, Origin origin) = 0;

  virtual void Close() = 0;

  virtual void Flush() = 0;

  virtual bool CanRead() = 0;
  virtual bool CanWrite() = 0;
  virtual bool CanSeek() = 0;

  virtual size_t GetLength() = 0;
};

class InputStream : public Stream
{
public:
  virtual void Write (const Array<uint8>& /*buffer*/, size_t /*offset*/, size_t /*count*/) { ERROR("Cannot write an input stream"); }
  virtual void WriteByte (uint8 /*value*/) { ERROR("Cannot write an input stream"); }
  virtual void Flush()    { ERROR("Cannot flush an input stream"); }
  virtual bool CanRead()  { return true; }
  virtual bool CanWrite() { return false; }
};

class OutputStream : public Stream
{
public:
  virtual int Read (Array<uint8>& /*buffer*/, size_t /*offset*/, size_t /*count*/) { ERROR("Cannot read an output stream"); }
  virtual int ReadByte () { ERROR("Cannot read an output stream"); }
  virtual bool CanRead()  { return false; }
  virtual bool CanWrite() { return true; }
};

    }
  }
}