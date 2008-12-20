//------------------------------------------------------------------------------
// Create a floppy disk that contains the files which path are passed in 
// parameters.
//
// Usage: createfloppy.exe <InputFilePath> [<InputFilePath>*] <OutputFilePath>
//
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

//#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
//#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
//#endif
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <tchar.h>
#include <string.h>


int _tmain(int argc, _TCHAR* argv[])
{
	unsigned char * floppy = new unsigned char [1474560];
	memset(floppy, 0, 1474560);
	unsigned char* current_floppy = floppy;

	for (int i=1; i<argc-1; i++)
	{
		FILE* f = _tfopen(argv[i], _T("rb"));
		fseek(f,0,SEEK_END);
		long filesize = ftell(f);
		fseek(f,0,SEEK_SET);
		fread(current_floppy, 1, filesize, f);
		current_floppy += filesize;
    fclose(f);
	}

	FILE* f = _tfopen(argv[argc-1], _T("wb"));
	fwrite(floppy, 1, 1474560, f);
	fclose(f);

	delete floppy;

	return 0;
}

