#include "Keyboard.h"

#include "../intmgr.h"
#include "../registers.h"
#include "../HAL/Keyboard.h"
#include "../Screen.h"

using namespace GenOS::Driver;


const Keys::values frenchLayout[3][128] = {
{
//NORMAL        0               1                2                3            4                 5               6             7                 8             9               A               B               C            D                E           F
/*0*/  Keys::None,      Keys::Esc, Keys::Ampersand,    Keys::eacute, Keys::Quote, Keys::Apostrophe,  Keys::ParentL,  Keys::Minus, Keys::egrave, Keys::Underscore,   Keys::ccedil,   Keys::agrave,  Keys::ParentR, Keys::Equal, Keys::Backspace,  Keys::Tab,
/*1*/     Keys::a,        Keys::z,         Keys::e,         Keys::r,     Keys::t,          Keys::y,        Keys::u,      Keys::i,      Keys::o,          Keys::p,    Keys::Caret,   Keys::Dollar,    Keys::Enter,  Keys::Ctrl,         Keys::q,    Keys::s,
/*2*/     Keys::d,        Keys::f,         Keys::g,         Keys::h,     Keys::j,          Keys::k,        Keys::l,      Keys::m, Keys::ugrave,       Keys::Sup2,   Keys::ShiftL, Keys::Asterisk,        Keys::w,     Keys::x,         Keys::c,    Keys::v,
/*3*/     Keys::b,        Keys::n,     Keys::Comma, Keys::Semicolon, Keys::Colon,       Keys::Excl,   Keys::ShiftR, Keys::PntScr,    Keys::Alt,      Keys::Space, Keys::CapsLock,       Keys::F1,       Keys::F2,    Keys::F3,        Keys::F4,   Keys::F5,
/*4*/    Keys::F6,       Keys::F7,        Keys::F8,        Keys::F9,   Keys::F10,       Keys::None,     Keys::None,   Keys::None,     Keys::Up,     Keys::PageUp,     Keys::None,     Keys::Left,     Keys::None, Keys::Right,      Keys::None, Keys::None,
/*5*/  Keys::Down, Keys::PageDown,       Keys::Ins,    Keys::Delete,  Keys::None,       Keys::None, Keys::LessThan,    Keys::F11,    Keys::F12,       Keys::None,     Keys::None,     Keys::WinL,     Keys::None,  Keys::WinR,      Keys::None, Keys::None,
/*6*/  Keys::None,     Keys::None,      Keys::None,      Keys::None,  Keys::None,       Keys::None,     Keys::None,   Keys::None,   Keys::None,       Keys::None,     Keys::None,     Keys::None,     Keys::None,  Keys::None,      Keys::None, Keys::None,
/*7*/  Keys::None,     Keys::None,      Keys::None,      Keys::None,  Keys::None,       Keys::None,     Keys::None,   Keys::None,   Keys::None,       Keys::None,     Keys::None,     Keys::None,     Keys::None,  Keys::None,      Keys::None, Keys::None,
},                                                                                                                                                  
{
//SHIFT         0               1               2                3            4                5                  6             7               8             9               A               B             C            D                E           F
/*0*/  Keys::None,      Keys::Esc,       Keys::D1,        Keys::D2,    Keys::D3,        Keys::D4,          Keys::D5,     Keys::D6,       Keys::D7,     Keys::D8,       Keys::D9,       Keys::D0, Keys::Degree,  Keys::Plus, Keys::Backspace,  Keys::Tab,
/*1*/     Keys::A,        Keys::Z,        Keys::E,         Keys::R,     Keys::T,         Keys::Y,           Keys::U,      Keys::I,        Keys::O,      Keys::P,    Keys::Trema,    Keys::Pound,  Keys::Enter,  Keys::Ctrl,         Keys::Q,    Keys::S,
/*2*/     Keys::D,        Keys::F,        Keys::G,         Keys::H,     Keys::J,         Keys::K,           Keys::L,      Keys::M,  Keys::Percent,   Keys::None,   Keys::ShiftL,       Keys::Mu,      Keys::W,     Keys::X,         Keys::C,    Keys::V,
/*3*/     Keys::B,        Keys::N, Keys::Question,       Keys::Dot, Keys::Slash, Keys::Paragraph,      Keys::ShiftR, Keys::PntScr,      Keys::Alt,  Keys::Space, Keys::CapsLock,       Keys::F1,     Keys::F2,    Keys::F3,        Keys::F4,   Keys::F5,
/*4*/    Keys::F6,       Keys::F7,       Keys::F8,        Keys::F9,   Keys::F10,      Keys::None,        Keys::None,   Keys::None,       Keys::Up, Keys::PageUp,     Keys::None,     Keys::Left,   Keys::None, Keys::Right,      Keys::None, Keys::None,
/*5*/  Keys::Down, Keys::PageDown,      Keys::Ins,    Keys::Delete,  Keys::None,      Keys::None, Keys::GreaterThan,    Keys::F11,      Keys::F12,   Keys::None,     Keys::None,     Keys::WinL,   Keys::None,  Keys::WinR,      Keys::None, Keys::None,
/*6*/  Keys::None,     Keys::None,     Keys::None,      Keys::None,  Keys::None,      Keys::None,        Keys::None,   Keys::None,     Keys::None,   Keys::None,     Keys::None,     Keys::None,   Keys::None,  Keys::None,      Keys::None, Keys::None,
/*7*/  Keys::None,     Keys::None,     Keys::None,      Keys::None,  Keys::None,      Keys::None,        Keys::None,   Keys::None,     Keys::None,   Keys::None,     Keys::None,     Keys::None,   Keys::None,  Keys::None,      Keys::None, Keys::None,
},                                                                                                                                                  
{
//ALTGR         0               1                2                3            4                 5               6             7             8                 9               A               B               C            D                E           F
/*0*/  Keys::None,      Keys::Esc,      Keys::None,     Keys::Tilde, Keys::Sharp,     Keys::BraceL, Keys::BracketL,   Keys::Pipe, Keys::Backquote,  Keys::BackSlash,    Keys::Caret,       Keys::At, Keys::BracketR, Keys::BraceR,     Keys::None, Keys::None,
/*1*/  Keys::None,     Keys::None,      Keys::Euro,      Keys::None,  Keys::None,       Keys::None,     Keys::None,   Keys::None,      Keys::None,       Keys::None,     Keys::None, Keys::Currency,     Keys::None,  Keys::Ctrl,      Keys::None, Keys::None,
/*2*/  Keys::None,     Keys::None,      Keys::None,      Keys::None,  Keys::None,       Keys::None,     Keys::None,   Keys::None,      Keys::None,       Keys::None,   Keys::ShiftL,     Keys::None,     Keys::None,  Keys::None,      Keys::None, Keys::None,
/*3*/  Keys::None,     Keys::None,      Keys::None,      Keys::None,  Keys::None,       Keys::None,   Keys::ShiftR,   Keys::None,       Keys::Alt,       Keys::None, Keys::CapsLock,       Keys::F1,       Keys::F2,    Keys::F3,        Keys::F4,   Keys::F5,
/*4*/    Keys::F6,       Keys::F7,        Keys::F8,        Keys::F9,   Keys::F10,       Keys::None,     Keys::None,   Keys::None,        Keys::Up,     Keys::PageUp,     Keys::None,     Keys::Left,     Keys::None, Keys::Right,      Keys::None, Keys::None,
/*5*/  Keys::Down, Keys::PageDown,       Keys::Ins,    Keys::Delete,  Keys::None,       Keys::None, Keys::LessThan,    Keys::F11,       Keys::F12,       Keys::None,     Keys::None,     Keys::None,     Keys::None,  Keys::WinR,      Keys::None, Keys::None,
/*6*/  Keys::None,     Keys::None,      Keys::None,      Keys::None,  Keys::None,       Keys::None,     Keys::None,   Keys::None,      Keys::None,       Keys::None,     Keys::None,     Keys::None,     Keys::None,  Keys::None,      Keys::None, Keys::None,
/*7*/  Keys::None,     Keys::None,      Keys::None,      Keys::None,  Keys::None,       Keys::None,     Keys::None,   Keys::None,      Keys::None,       Keys::None,     Keys::None,     Keys::None,     Keys::None,  Keys::None,      Keys::None, Keys::None,
}
};

// Pause: E1 1D 45 E1 9D C5
// AltGr: 1D 38 / 9D B8 (Ctrl + Alt)


Keyboard::Keyboard()
{
  _shift = false;
  _ctrl = false;
  _alt = false;
  _escaped = false;

  InterruptManager::RegisterInterrupt(InterruptManager::Keyboard, &Keyboard::InterruptHandler, this);
}

Keyboard::~Keyboard()
{
  InterruptManager::UnregisterInterrupt(InterruptManager::Keyboard);
}

void Keyboard::InterruptHandler(const GenOS::Registers& /*regs*/, void* data)
{
  Keyboard* that = ((Keyboard*)data);

  const uint32 scancode = HAL::Keyboard::GetScanCode();

  //Screen::cout << "[KBD] queue scancode " << scancode << Screen::endl;
  that->_cache.Queue (scancode);

  //if (that->_handler.Address)
  //  that->_handler.Address(regs, that->_handler.Data);
}

Keys::values Keyboard::ReadKey()
{
  for (;;)
  {
    while (_cache.IsEmpty())
      _asm hlt;

    uint32 scancode = 0;
    _cache.Dequeue(scancode);

    const bool keyPressed = ((scancode & 0x80) == 0);
    const uint8 keyCode = scancode & 0x7F;

    // handle modifiers
    if ( ((scancode & 0x007F) == 0x2A) || ((scancode & 0x007F) == 0x36) )
    {
      _shift = keyPressed;
      continue;
    }
    if ( ((scancode & 0x007F) == 0x1D) )
    {
      _ctrl = keyPressed;
      continue;
    }
    if ( ((scancode & 0x007F) == 0x38) )
    {
      _alt = keyPressed;
      continue;
    }
    if (scancode & 0xFF00)
    {
      _escaped = true;
      continue;
    }

    // Key released
    if (!keyPressed)
      continue;

    unsigned int table = 0;
    if (_shift) 
    {
      table = 1;
    }
    else if (_ctrl && _alt)
    {
      table = 2;
    }

   return (Keys::values)(frenchLayout[table][keyCode] 
                      | (_shift ? Keys::ShiftModifier : 0)
                      | (_ctrl ? Keys::CtrlModifier : 0)
                      | (_alt ? Keys::AltModifier : 0));
  }
}