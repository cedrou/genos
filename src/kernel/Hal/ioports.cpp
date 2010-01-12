//------------------------------------------------------------------------------
// ioports.cpp
//	HAL - IO hardware ports
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

#include "ioports.h"

using namespace GenOS::HAL;

void IOPort::Out8(uint16 port, uint8 value)
{
	__asm
	{
		mov al, value
		mov dx, port
		out dx, al
	}
}
void IOPort::Out16(uint16 port, uint16 value)
{
	__asm
	{
		mov ax, value
		mov dx, port
		out dx, ax
	}
}
void IOPort::Out32(uint16 port, uint32 value)
{
	__asm
	{
		mov eax, value
		mov dx, port
		out dx, eax
	}
}

uint8 IOPort::In8(uint16 port)
{
	__asm
	{
		mov dx, port
		in al, dx
	}
}

uint16 IOPort::In16(uint16 port)
{
	__asm
	{
		mov dx, port
		in ax, dx
	}
}

uint32 IOPort::In32(uint16 port)
{
	__asm
	{
		mov dx, port
		in eax, dx
	}
}

