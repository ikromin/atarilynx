#ifndef __LYNX_SD_UI__
#define __LYNX_SD_UI__

#include <stdlib.h>
#include <6502.h>
#include <tgi.h>
#include <string.h>
#include <joystick.h>
#include <conio.h>
#include "LynxSD.h"
#include "Directory.h"
#include "Program.h"

extern unsigned char masterpal[];
extern unsigned char blackpal[];

void UI_run();
void UI_showLastRomScreen(char romFileName[]);
void UI_showLoadingScreen();
void UI_init();

#endif // __LYNX_SD_UI__
