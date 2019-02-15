#ifndef __LYNX_SD_UI__
#define __LYNX_SD_UI__

#include <stdlib.h>
#include <6502.h>
#include <tgi.h>
#include <string.h>
#include "LynxSD.h"
#include "Directory.h"
#include "Joystick.h"
#include "Program.h"

#define MAX_UI_LINES 7

void UI_init();
void UI_showLastRomScreen(char romFileName[]);
void UI_showLoadingScreen();
void UI_showProgrammingScreen();
void UI_showFailScreen();
void UI_showHelpScreen();
void UI_clear();

void UI_selectPrevious();
void UI_selectNext();
void UI_selectPrevious2();
void UI_selectNext2();

void UI_render();

#endif // __LYNX_SD_UI__
