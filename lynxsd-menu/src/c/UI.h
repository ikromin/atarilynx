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

void runUI();
void showLastRomScreen(char romFileName[]);
void showLoadingScreen();
void UI_init();

#endif // __LYNX_SD_UI__
