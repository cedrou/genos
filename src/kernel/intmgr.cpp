//------------------------------------------------------------------------------
// intmgr.cpp
//	CPU Interruptions Manager
//------------------------------------------------------------------------------
// Copyright (c) 2008, Cedric Rousseau
// All rights reserved.
// 
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
#include "ioports.h"

using namespace GenOS;


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

InterruptManager::InterruptHandler InterruptManager::_handlers[256];

InterruptManager::InterruptHandler InterruptManager::RegisterInterrupt(uint8 interrupt, InterruptHandler handler)
{
  InterruptHandler oldHandler = _handlers[interrupt];
  _handlers[interrupt] = handler;
  return oldHandler;
}

void InterruptManager::Initialize()
{
  // Remap the irq table.
  IOPort::Out8(IOPort::PIC_Master_Command, 0x11);
  IOPort::Out8(IOPort::PIC_Slave_Command, 0x11);

  IOPort::Out8(IOPort::PIC_Master_Data, 0x20);
  IOPort::Out8(IOPort::PIC_Slave_Data, 0x28);
  IOPort::Out8(IOPort::PIC_Master_Data, 0x04);
  IOPort::Out8(IOPort::PIC_Slave_Data, 0x02);
  IOPort::Out8(IOPort::PIC_Master_Data, 0x01);
  IOPort::Out8(IOPort::PIC_Slave_Data, 0x01);
  IOPort::Out8(IOPort::PIC_Master_Data, 0x0);
  IOPort::Out8(IOPort::PIC_Slave_Data, 0x0);

  // Fill in the IDT
  memset(&idt, (uint8)0, sizeof(InterruptDescriptor)*256);

  EncryptIdtEntry( 0, (uint32)Int0 , 0x08, 0x8E);
  EncryptIdtEntry( 1, (uint32)Int1 , 0x08, 0x8E);
  EncryptIdtEntry( 2, (uint32)Int2 , 0x08, 0x8E);
  EncryptIdtEntry( 3, (uint32)Int3 , 0x08, 0x8E);
  EncryptIdtEntry( 4, (uint32)Int4 , 0x08, 0x8E);
  EncryptIdtEntry( 5, (uint32)Int5 , 0x08, 0x8E);
  EncryptIdtEntry( 6, (uint32)Int6 , 0x08, 0x8E);
  EncryptIdtEntry( 7, (uint32)Int7 , 0x08, 0x8E);
  EncryptIdtEntry( 8, (uint32)Int8 , 0x08, 0x8E);
  EncryptIdtEntry( 9, (uint32)Int9 , 0x08, 0x8E);
  EncryptIdtEntry(10, (uint32)Int10, 0x08, 0x8E);
  EncryptIdtEntry(11, (uint32)Int11, 0x08, 0x8E);
  EncryptIdtEntry(12, (uint32)Int12, 0x08, 0x8E);
  EncryptIdtEntry(13, (uint32)Int13, 0x08, 0x8E);
  EncryptIdtEntry(14, (uint32)Int14, 0x08, 0x8E);
  EncryptIdtEntry(15, (uint32)Int15, 0x08, 0x8E);
  EncryptIdtEntry(16, (uint32)Int16, 0x08, 0x8E);
  EncryptIdtEntry(17, (uint32)Int17, 0x08, 0x8E);
  EncryptIdtEntry(18, (uint32)Int18, 0x08, 0x8E);
  EncryptIdtEntry(19, (uint32)Int19, 0x08, 0x8E);
  EncryptIdtEntry(20, (uint32)Int20, 0x08, 0x8E);
  EncryptIdtEntry(21, (uint32)Int21, 0x08, 0x8E);
  EncryptIdtEntry(22, (uint32)Int22, 0x08, 0x8E);
  EncryptIdtEntry(23, (uint32)Int23, 0x08, 0x8E);
  EncryptIdtEntry(24, (uint32)Int24, 0x08, 0x8E);
  EncryptIdtEntry(25, (uint32)Int25, 0x08, 0x8E);
  EncryptIdtEntry(26, (uint32)Int26, 0x08, 0x8E);
  EncryptIdtEntry(27, (uint32)Int27, 0x08, 0x8E);
  EncryptIdtEntry(28, (uint32)Int28, 0x08, 0x8E);
  EncryptIdtEntry(29, (uint32)Int29, 0x08, 0x8E);
  EncryptIdtEntry(30, (uint32)Int30, 0x08, 0x8E);
  EncryptIdtEntry(31, (uint32)Int31, 0x08, 0x8E); 
  EncryptIdtEntry(32, (uint32)Int32, 0x08, 0x8E);
  EncryptIdtEntry(33, (uint32)Int33, 0x08, 0x8E);
  EncryptIdtEntry(34, (uint32)Int34, 0x08, 0x8E);
  EncryptIdtEntry(35, (uint32)Int35, 0x08, 0x8E);
  EncryptIdtEntry(36, (uint32)Int36, 0x08, 0x8E);
  EncryptIdtEntry(37, (uint32)Int37, 0x08, 0x8E);
  EncryptIdtEntry(38, (uint32)Int38, 0x08, 0x8E);
  EncryptIdtEntry(39, (uint32)Int39, 0x08, 0x8E);
  EncryptIdtEntry(40, (uint32)Int40, 0x08, 0x8E);
  EncryptIdtEntry(41, (uint32)Int41, 0x08, 0x8E);
  EncryptIdtEntry(42, (uint32)Int42, 0x08, 0x8E);
  EncryptIdtEntry(43, (uint32)Int43, 0x08, 0x8E);
  EncryptIdtEntry(44, (uint32)Int44, 0x08, 0x8E);
  EncryptIdtEntry(45, (uint32)Int45, 0x08, 0x8E);
  EncryptIdtEntry(46, (uint32)Int46, 0x08, 0x8E);
  EncryptIdtEntry(47, (uint32)Int47, 0x08, 0x8E);

  // Give the IDT to the CPU
  InterruptDescriptorTable idtptr; 
  idtptr.limit = sizeof(InterruptDescriptor) * 256 -1;
  idtptr.base  = (uint32)&idt;
  __asm lidt idtptr        ; Load the IDT pointer.
}

void InterruptManager::EncryptIdtEntry(uint8 index, uint32 base, uint16 sel, uint8 flags)
{
  idt[index].base_lo = base & 0xFFFF;
  idt[index].base_hi = (base >> 16) & 0xFFFF;
  idt[index].sel     = sel;
  idt[index].always0 = 0;
  idt[index].flags   = flags;
}

void InterruptManager::Isr(Registers regs)
{
  if (regs.int_no >= 32)
  {
    // Send an EOI (end of interrupt) signal to the PICs.
    // If this interrupt involved the slave.
    if (regs.int_no >= 40)
    {
      // Send reset signal to slave.
      IOPort::Out8(IOPort::PIC_Slave_Command, 0x20);
    }
    // Send reset signal to master. (As well as slave, if necessary).
    IOPort::Out8(IOPort::PIC_Master_Command, 0x20);
  }

  if (_handlers[regs.int_no] != 0)
  {
    return _handlers[regs.int_no](regs);
  }

  if(regs.int_no < 19)
  {
    const char* CpuExceptions[] = {
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

    Screen::WriteString("**** Unhandled exception: ");
    Screen::WriteString(CpuExceptions[regs.int_no]); 
    Screen::WriteString(" ****\n");
  }
  else
  {
    Screen::WriteString("**** Unhandled interrupt: 0x");
    Screen::WriteHex(regs.int_no, 8);
    Screen::WriteString(" ****\n");
  }

  regs.Print();

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

    call Isr

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
