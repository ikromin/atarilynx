/******************************************************************************
 Lynx SD Menu Loader 2. Firmware for the Lynx SD game cartridge.

 Copyright 2019 Igor Kromin (Necrocia) - Atari Gamer

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef __LYNX_SD_DIRECTORY__
#define __LYNX_SD_DIRECTORY__

#include "LynxSD.h"

typedef struct {
	char*	szFilename;
	u8		bDirectory;
	char*  szLongName;
} SDirEntry;


extern char gszCurrentDir[256];
extern u8 gnSelectIndex;

extern u8 gnNumDirEntries;
extern u8 ganDirOrder[256];
extern SDirEntry gsDirEntry[256];

extern char gsFilenameBuffer[];
extern char *gpFilenamePtr;

void __fastcall__ DIR_read(const char *pDir);
u8 __fastcall__ DIR_IsValidFilePath(const char romFile[]);
void __fastcall__ DIR_FullFilePath(char buf[], const char file[]);

#endif // __LYNX_SD_DIRECTORY__
