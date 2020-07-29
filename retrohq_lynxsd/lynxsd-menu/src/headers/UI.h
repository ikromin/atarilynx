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

#ifndef __LYNX_SD_UI__
#define __LYNX_SD_UI__

#include "LynxSD.h"

void UI_init();
void __fastcall__ UI_showLastRomScreen(char romFileName[]);
void __fastcall__ UI_showFailScreen(char* fileName);
void UI_showLoadingDirScreen();
u8 UI_showPreviewScreen();
void UI_showProgrammingScreen();
void UI_showHelpScreen();
void UI_showInitScreen();
void UI_showCreditsScreen();

void UI_clear();
void UI_resetPalette();

void UI_selectPrevious();
void UI_selectNext();
void UI_selectPrevious2();
void UI_selectNext2();

void UI_forwardAction();
void UI_backAction();

void UI_showDirectory();

#endif // __LYNX_SD_UI__
