//------------------------------------------------------------------------------
// keyboard.h
//	Driver - Keyboard
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

#include <common.h>
#include <fifo.h>
#include <intmgr.h>
#include <registers.h>

namespace GenOS {
  namespace Driver {


class Keys { public: enum values
{
       None = 0x00,
  Backspace = 0x08,     Tab, 
  // Displayed ASCII
      Space = 0x20,    Excl,    Quote,      Sharp,     Dollar,  Percent,    Ampersand, Apostrophe, 
    ParentL = 0x28, ParentR, Asterisk,       Plus,      Comma,    Minus,          Dot,      Slash,
         D0 = 0x30,      D1,       D2,         D3,         D4,       D5,           D6,         D7, 
         D8 = 0x38,      D9,    Colon,  Semicolon,   LessThan,    Equal,  GreaterThan,   Question,
         At = 0x40,       A,        B,          C,          D,        E,            F,          G, 
          H = 0x48,       I,        J,          K,          L,        M,            N,          O,
          P = 0x50,       Q,        R,          S,          T,        U,            V,          W,
          X = 0x58,       Y,        Z,   BracketL,  BackSlash, BracketR,        Caret, Underscore,
  Backquote = 0x60,       a,        b,          c,          d,        e,            f,          g,
          h = 0x68,       i,        j,          k,          l,        m,            n,          o, 
          p = 0x70,       q,        r,          s,          t,        u,            v,          w,
          x = 0x78,       y,        z,     BraceL,       Pipe,   BraceR,        Tilde,     Delete,
  

  F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

  Esc, PntScr, Break, Ins,

  Degree, 
  Enter,
  ugrave, eacute, egrave, agrave, ccedil, 
  Trema, 
  Sup2, Pound, Mu, Paragraph, Euro, Currency,

  Up, Down, Left, Right,
  PageUp, PageDown,

  ShiftL, ShiftR,
  CapsLock,
  Ctrl,
  Alt, 
  WinL, WinR,
  Fn,

  // Modifiers
  ShiftModifier = 0x10000,
  CtrlModifier  = 0x20000,
  AltModifier   = 0x40000,
};};

class Keyboard
{
private:
  Fifo<uint32, 256>             _cache;
  InterruptManager::HandlerInfo _keyDownHandler;
  InterruptManager::HandlerInfo _keyUpHandler;
  bool                          _shift;
  bool                          _ctrl;
  bool                          _alt;
  bool                          _escaped;

public:
  Keyboard();
  ~Keyboard();

  //void RegisterKeyDownEvent(InterruptManager::Handler handler, void* data = NULL);
  //void UnregisterKeyDownEvent();

  //void RegisterKeyUpEvent(InterruptManager::Handler handler, void* data = NULL);
  //void UnregisterKeyUpEvent();

  Keys::values Keyboard::ReadKey();


private:
  static void __stdcall InterruptHandler(const Registers& regs, void* data);
};

  }
}
