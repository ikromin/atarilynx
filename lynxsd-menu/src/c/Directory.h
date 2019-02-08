#ifndef __LYNX_SD_DIRECTORY__
#define __LYNX_SD_DIRECTORY__

#include <string.h>
#include "LynxSD.h"

extern u8 gnNumDirEntries;
extern u8 ganDirOrder[256];
extern SDirEntry gsDirEntry[256];
extern char gszCurrentDir[256];
extern char gszCurrentFile[13];

void __fastcall__ ReadDirectory(const char *pDir);

#endif // __LYNX_SD_DIRECTORY__
