#pragma once

#include "../common.h"
#include "../fifo.h"
#include "../intmgr.h"
#include "../registers.h"

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
