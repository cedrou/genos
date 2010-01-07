//------------------------------------------------------------------------------
// array.h
//	Definition of a templated array
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
#include "bitmanip.h"

namespace GenOS
{

  template <typename T>
  class Array
  {
  private:
    T*            _buffer;
    size_t        _size;
    size_t        _capacity;

  public:
    Array(size_t size = 0)
    {
      _size = size;
      _capacity = size==0 ? 4 : BitManip::RoundUpPow2(size);
      _buffer = new T[_capacity];
    }

    ~Array()
    {
      delete[] _buffer;
    }

    size_t Size() const
    {
      return _size;
    }

    T& At(uint32 index)
    {
      ASSERT(index<_size);
      return _buffer[index];
    }
    const T& At(uint32 index) const
    {
      ASSERT(index<_size);
      return _buffer[index];
    }
    T& operator[](uint32 index) { return At(index); }
    const T& operator[](uint32 index) const { return At(index); }

    uint32 Find(const T& item) const
    {
      uint32 index = 0;
      while((index < _size) && (item != _buffer[index])) index++;
      return index;
    }

    void Push(const T& item)
    {
      if(_size==_capacity)
      {
        // Double the capacity of the buffer
        if(_capacity==0) 
          _capacity = 4;
        else 
          _capacity *= 2;

        T* buffer = new T[_capacity];

        memcpy(buffer, _buffer, _size * sizeof(T));

        // switch buffers
        if(_buffer) delete[] _buffer;
        _buffer = buffer;
      }

      _buffer[_size++] = item;
    }

    void Insert(const T& item, uint32 index)
    {
      if(_size==_capacity)
      {
        // Double the capacity of the buffer
        if(_capacity==0) 
          _capacity = 4;
        else 
          _capacity *= 2;

        T* buffer = new T[_capacity];

        // identity until the index
        memcpy(buffer, _buffer, index * sizeof(T));

        // save room for on item then copy the following ones
        memcpy(&buffer[i+1], &_buffer[i], (_size-index) * sizeof(T));

        // switch buffers
        if(_buffer) delete[] _buffer;
        _buffer = buffer;
      }
      else
      {
        for(uint32 i=_size; i>index; i--)
        {
          _buffer[i] = _buffer[i-1];
        }
      }
      _buffer[index] = item;
      _size++;
    }

    void Remove(size_t index)
    {
      for(uint32 i=index; i<_size-1; i++)
      {
        _buffer[i] = _buffer[i+1];
      }
      _size--;
      //TODO: Resize the buffer
    }

    void Remove(const T& item)
    {
      Remove(Find(item));
    }


  };


}
