#include <stdlib.h>
#include <6502.h>
#include <lynx.h>
#include <tgi.h>
#include <peekpoke.h>
#include <string.h>
#include <joystick.h>
#include <conio.h>
#include "LynxSD.h"

FRESULT __fastcall__ LynxSD_Program(const char *pFilename);
FRESULT __fastcall__ LynxSD_ProgramLYX(const char *pFilename);
FRESULT __fastcall__ LynxSD_ProgramHomebrew(const char *pFilename);
