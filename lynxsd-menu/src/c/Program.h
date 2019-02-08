#ifndef __LYNX_SD_PROGRAM__
#define __LYNX_SD_PROGRAM__

#include "LynxSD.h"

FRESULT __fastcall__ LynxSD_Program(const char *pFilename);
FRESULT __fastcall__ LynxSD_ProgramLYX(const char *pFilename);
FRESULT __fastcall__ LynxSD_ProgramHomebrew(const char *pFilename);
void LaunchROM();

#endif // __LYNX_SD_PROGRAM__
