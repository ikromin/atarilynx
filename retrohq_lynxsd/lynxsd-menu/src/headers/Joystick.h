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

#ifndef __LYNX_SD_JOYSTICK__
#define __LYNX_SD_JOYSTICK__

#include <conio.h>
#include "Types.h"

// macros for joystick direction states
#define BJOY_UP _Joy_IsOn(JOY_UP_MASK, 0)
#define BJOY_DOWN _Joy_IsOn(JOY_DOWN_MASK, 0)
#define BJOY_LEFT _Joy_IsOn(JOY_LEFT_MASK, 0)
#define BJOY_RIGHT _Joy_IsOn(JOY_RIGHT_MASK, 0)

// macros for joystick button and 'keyboard' button states
#define BJOY_A _Joy_IsOn(JOY_BTN_1_MASK, 1)
#define BJOY_B _Joy_IsOn(JOY_BTN_2_MASK, 1)
#define KEY_HIT kbhit()
#define KEY_STATE _Key_GetHit()

#define DIRECTION_DELAY_FRAMES_FAST 6
#define DIRECTION_DELAY_FRAMES_SLOW 14

u8 __fastcall__ _Joy_IsOn(u8 mask, u8 isButton);
char _Key_GetHit();
void Joy_Buffer();
void Joy_Clear();

#endif // __LYNX_SD_JOYSTICK__