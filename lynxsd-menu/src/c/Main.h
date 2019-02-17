#ifndef __LYNX_SD_MENU__
#define __LYNX_SD_MENU__

#include <6502.h>
#include <tgi.h>
#include <joystick.h>
#include <conio.h>
#include <string.h>

#include "LynxSD.h"
#include "Joystick.h"
#include "UI.h"
#include "Program.h"
#include "Directory.h"
#include "Preferences.h"

// TGI and Joystick library references
extern char lynxtgi[];
extern char lynxjoy[];

#define FILE_LAST_ROM "menu/lastrom"

#endif // __LYNX_SD_MENU__
