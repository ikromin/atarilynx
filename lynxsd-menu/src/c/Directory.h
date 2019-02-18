#ifndef __LYNX_SD_DIRECTORY__
#define __LYNX_SD_DIRECTORY__

#include "LynxSD.h"

extern char gszCurrentDir[256];
extern u8 gnSelectIndex;

extern u8 gnNumDirEntries;
extern u8 ganDirOrder[256];
extern SDirEntry gsDirEntry[256];

void __fastcall__ DIR_read(const char *pDir);
u8 __fastcall__ DIR_IsValidRomPath(const char romFile[]);
void __fastcall__ DIR_FullRomPath(char buf[], const char romFile[]);

#endif // __LYNX_SD_DIRECTORY__
