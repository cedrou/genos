#include "Keyboard.h"

#include "../intmgr.h"
#include "../registers.h"
#include "../HAL/Keyboard.h"
#include "../Screen.h"

using namespace GenOS::Driver;


Keyboard::Keyboard()
{
  InterruptManager::RegisterInterrupt(InterruptManager::Keyboard, &Keyboard::InterruptHandler, this);
}

Keyboard::~Keyboard()
{
  InterruptManager::UnregisterInterrupt(InterruptManager::Keyboard);
}

void Keyboard::InterruptHandler(const GenOS::Registers& regs, void* data)
{
  Keyboard* that = ((Keyboard*)data);

  const uint32 scancode = HAL::Keyboard::GetScanCode();

  Screen::cout << "[KBD] queue scancode " << scancode << Screen::endl;
  that->_cache.Queue (scancode);

  //if (that->_handler.Address)
  //  that->_handler.Address(regs, that->_handler.Data);
}
