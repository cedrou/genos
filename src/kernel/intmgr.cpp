//------------------------------------------------------------------------------
// intmgr.cpp
//	CPU Interruptions Manager
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

#include "intmgr.h"
#include "screen.h"
#include "kernel.h"
#include "hal/pic.h"

using namespace GenOS;
using namespace GenOS::HAL;


#define ISR_NOERRCODE(i) \
  void __declspec(naked) Int##i () { \
  __asm { \
    __asm cli            \
    __asm push 0         \
    __asm push i         \
    __asm jmp InterruptManager::IsrCommon  \
  }                      \
};
#define ISR_ERRCODE(i)   \
  void __declspec(naked) Int##i () { \
  __asm { \
    __asm cli            \
    __asm push i         \
    __asm jmp InterruptManager::IsrCommon  \
  }                      \
};

ISR_NOERRCODE(0 );
ISR_NOERRCODE(1 );
ISR_NOERRCODE(2 );
ISR_NOERRCODE(3 );
ISR_NOERRCODE(4 );
ISR_NOERRCODE(5 );
ISR_NOERRCODE(6 );
ISR_NOERRCODE(7 );
ISR_ERRCODE  (8 );
ISR_NOERRCODE(9 );
ISR_ERRCODE  (10);
ISR_ERRCODE  (11);
ISR_ERRCODE  (12);
ISR_ERRCODE  (13);
ISR_ERRCODE  (14);
ISR_NOERRCODE(15);
ISR_NOERRCODE(16);
ISR_NOERRCODE(17);
ISR_NOERRCODE(18);
ISR_NOERRCODE(19);
ISR_NOERRCODE(20);
ISR_NOERRCODE(21);
ISR_NOERRCODE(22);
ISR_NOERRCODE(23);
ISR_NOERRCODE(24);
ISR_NOERRCODE(25);
ISR_NOERRCODE(26);
ISR_NOERRCODE(27);
ISR_NOERRCODE(28);
ISR_NOERRCODE(29);
ISR_NOERRCODE(30);
ISR_NOERRCODE(31); 
ISR_NOERRCODE(32); // IRQ0
ISR_NOERRCODE(33);
ISR_NOERRCODE(34);
ISR_NOERRCODE(35);
ISR_NOERRCODE(36);
ISR_NOERRCODE(37);
ISR_NOERRCODE(38);
ISR_NOERRCODE(39);
ISR_NOERRCODE(40);
ISR_NOERRCODE(41);
ISR_NOERRCODE(42);
ISR_NOERRCODE(43);
ISR_NOERRCODE(44);
ISR_NOERRCODE(45);
ISR_NOERRCODE(46);
ISR_NOERRCODE(47);



#pragma pack(push,1)

struct InterruptDescriptor
{
  uint16 base_lo;  // The lower 16 bits of the address to jump to when this interrupt fires.
  uint16 sel;      // Kernel segment selector.
  uint8  always0;  // This must always be zero.
  uint8  flags;    // More flags. See documentation.
  uint16 base_hi;  // The upper 16 bits of the address to jump to.
};

struct InterruptDescriptorTable
{
  uint16 limit;
  uint32 base;     // The address of the first element in our idt_entry_t array.
};

#pragma pack(pop)


static InterruptDescriptor idt[256];

InterruptManager::HandlerInfo InterruptManager::_handlers[256] = {0};

void InterruptManager::RegisterInterrupt(uint8 interrupt, Handler handler, void* data)
{
  if ( (_handlers[interrupt].Address != NULL) && (_handlers[interrupt].Address != (void*)-1) )
  {
    Screen::cout << "WARNING - overwrite interrupt #" << interrupt << " handler !" << Screen::endl;
    Screen::cout << "    Previous Address/Data = " << _handlers[interrupt].Address << " / " << _handlers[interrupt].Data << Screen::endl;
    Screen::cout << "         New Address/Data = " << handler << " / " << data << Screen::endl;
  }
  _handlers[interrupt].Address = handler;
  _handlers[interrupt].Data = data;
}

void InterruptManager::UnregisterInterrupt(uint8 interrupt)
{
  _handlers[interrupt].Address = NULL;
  _handlers[interrupt].Data = NULL;
}


void InterruptManager::Initialize()
{
  // Remap the irq table.
  HAL::PIC::Remap (0x20, 0x28);

  // Fill in the IDT
  memset(&idt, (uint8)0, sizeof(InterruptDescriptor)*256);

  EncodeIdtEntry( 0, (uint32)Int0 , 0x08, 0x8E);
  EncodeIdtEntry( 1, (uint32)Int1 , 0x08, 0x8E);
  EncodeIdtEntry( 2, (uint32)Int2 , 0x08, 0x8E);
  EncodeIdtEntry( 3, (uint32)Int3 , 0x08, 0x8E);
  EncodeIdtEntry( 4, (uint32)Int4 , 0x08, 0x8E);
  EncodeIdtEntry( 5, (uint32)Int5 , 0x08, 0x8E);
  EncodeIdtEntry( 6, (uint32)Int6 , 0x08, 0x8E);
  EncodeIdtEntry( 7, (uint32)Int7 , 0x08, 0x8E);
  EncodeIdtEntry( 8, (uint32)Int8 , 0x08, 0x8E);
  EncodeIdtEntry( 9, (uint32)Int9 , 0x08, 0x8E);
  EncodeIdtEntry(10, (uint32)Int10, 0x08, 0x8E);
  EncodeIdtEntry(11, (uint32)Int11, 0x08, 0x8E);
  EncodeIdtEntry(12, (uint32)Int12, 0x08, 0x8E);
  EncodeIdtEntry(13, (uint32)Int13, 0x08, 0x8E);
  EncodeIdtEntry(14, (uint32)Int14, 0x08, 0x8E);
  EncodeIdtEntry(15, (uint32)Int15, 0x08, 0x8E);
  EncodeIdtEntry(16, (uint32)Int16, 0x08, 0x8E);
  EncodeIdtEntry(17, (uint32)Int17, 0x08, 0x8E);
  EncodeIdtEntry(18, (uint32)Int18, 0x08, 0x8E);
  EncodeIdtEntry(19, (uint32)Int19, 0x08, 0x8E);
  EncodeIdtEntry(20, (uint32)Int20, 0x08, 0x8E);
  EncodeIdtEntry(21, (uint32)Int21, 0x08, 0x8E);
  EncodeIdtEntry(22, (uint32)Int22, 0x08, 0x8E);
  EncodeIdtEntry(23, (uint32)Int23, 0x08, 0x8E);
  EncodeIdtEntry(24, (uint32)Int24, 0x08, 0x8E);
  EncodeIdtEntry(25, (uint32)Int25, 0x08, 0x8E);
  EncodeIdtEntry(26, (uint32)Int26, 0x08, 0x8E);
  EncodeIdtEntry(27, (uint32)Int27, 0x08, 0x8E);
  EncodeIdtEntry(28, (uint32)Int28, 0x08, 0x8E);
  EncodeIdtEntry(29, (uint32)Int29, 0x08, 0x8E);
  EncodeIdtEntry(30, (uint32)Int30, 0x08, 0x8E);
  EncodeIdtEntry(31, (uint32)Int31, 0x08, 0x8E); 
  EncodeIdtEntry(32, (uint32)Int32, 0x08, 0x8E);
  EncodeIdtEntry(33, (uint32)Int33, 0x08, 0x8E);
  EncodeIdtEntry(34, (uint32)Int34, 0x08, 0x8E);
  EncodeIdtEntry(35, (uint32)Int35, 0x08, 0x8E);
  EncodeIdtEntry(36, (uint32)Int36, 0x08, 0x8E);
  EncodeIdtEntry(37, (uint32)Int37, 0x08, 0x8E);
  EncodeIdtEntry(38, (uint32)Int38, 0x08, 0x8E);
  EncodeIdtEntry(39, (uint32)Int39, 0x08, 0x8E);
  EncodeIdtEntry(40, (uint32)Int40, 0x08, 0x8E);
  EncodeIdtEntry(41, (uint32)Int41, 0x08, 0x8E);
  EncodeIdtEntry(42, (uint32)Int42, 0x08, 0x8E);
  EncodeIdtEntry(43, (uint32)Int43, 0x08, 0x8E);
  EncodeIdtEntry(44, (uint32)Int44, 0x08, 0x8E);
  EncodeIdtEntry(45, (uint32)Int45, 0x08, 0x8E);
  EncodeIdtEntry(46, (uint32)Int46, 0x08, 0x8E);
  EncodeIdtEntry(47, (uint32)Int47, 0x08, 0x8E);

  // Give the IDT to the CPU
  InterruptDescriptorTable idtptr; 
  idtptr.limit = sizeof(InterruptDescriptor) * 256 -1;
  idtptr.base  = (uint32)&idt;
  __asm lidt idtptr        ; Load the IDT pointer.
}

void InterruptManager::EncodeIdtEntry(uint8 index, uint32 base, uint16 sel, uint8 flags)
{
  idt[index].base_lo = base & 0xFFFF;
  idt[index].base_hi = (base >> 16) & 0xFFFF;
  idt[index].sel     = sel;
  idt[index].always0 = 0;
  idt[index].flags   = flags;
}

void InterruptManager::Isr(Registers regs)
{
  regs.Print();

  const HandlerInfo& ihi = _handlers[regs.int_no];
  if (ihi.Address != 0)
  {
    return ihi.Address(regs, ihi.Data);
  }

  if(regs.int_no < 19)
  {
    static const char* CpuExceptions[] = {
      "Division by zero exception",
      "Debug exception",
      "Non maskable interrupt",
      "Breakpoint exception",
      "Into detected overflow",
      "Out of bounds exception",
      "Invalid opcode exception",
      "No coprocessor exception",
      "Double fault",
      "Coprocessor segment overrun",
      "Bad TSS",
      "Segment not present",
      "Stack fault",
      "General protection fault",
      "Page fault",
      "Unknown interrupt exception",
      "Coprocessor fault",
      "Alignment check exception",
      "Machine check exception"
    };

    Screen::cout << "**** Unhandled exception: " << CpuExceptions[regs.int_no] << " ****" << Screen::endl;
  }
  else
  {
    Screen::cout << "**** Unhandled interrupt: 0x" << (uint8)regs.int_no << " ****" << Screen::endl;
  }

  Kernel::Hang();
}


void __declspec(naked) InterruptManager::IsrCommon () 
{
  __asm 
  {
    pushad                  ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    xor eax, eax
    mov ax, ds               ; Lower 16-bits of eax = ds.
    push eax                 ; save the data segment descriptor

    mov ax, 0x10  ; load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
  }

  //__asm call Isr;

  static Registers* regs = NULL;
  __asm mov regs, esp;

  if (_handlers[regs->int_no].Address == 0)
  {
    regs->Print();

    if(regs->int_no < 19)
    {
      static const char* CpuExceptions[] = {
        "Division by zero exception",
        "Debug exception",
        "Non maskable interrupt",
        "Breakpoint exception",
        "Into detected overflow",
        "Out of bounds exception",
        "Invalid opcode exception",
        "No coprocessor exception",
        "Double fault",
        "Coprocessor segment overrun",
        "Bad TSS",
        "Segment not present",
        "Stack fault",
        "General protection fault",
        "Page fault",
        "Unknown interrupt exception",
        "Coprocessor fault",
        "Alignment check exception",
        "Machine check exception"
      };

      Screen::cout << "**** Unhandled exception: " << CpuExceptions[regs->int_no] << " ****" << Screen::endl;
    }
    else
    {
      Screen::cout << "**** Unhandled interrupt: 0x" << (uint8)regs->int_no << " ****" << Screen::endl;
    }

    Kernel::Hang();
  }

  _handlers[regs->int_no].Address(*regs, _handlers[regs->int_no].Data);

  if (regs->int_no >= 32)
  {
    // Send an EOI (end of interrupt) signal to the PICs.
    HAL::PIC::EndOfInterrupt((uint8)(regs->int_no - 32));
  }

  __asm jmp IsrEnd;
}

void __declspec(naked) InterruptManager::IsrEnd () 
{
  __asm
  {
    pop eax        ; reload the original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popad                    ; Pops edi,esi,ebp...
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    sti
    iretd          ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
  }
}
