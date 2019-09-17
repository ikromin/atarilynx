/******************************************************************************
 Atari Gamer's Atari Lynx 30th Birthday Competition ROM Launcher

 Copyright (C) 2019 Igor Kromin

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <https://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef __LYNX_JOYSTICK__
#define __LYNX_JOYSTICK__

#include <joystick.h>
#include "Types.h"

// macros for joystick direction states
#define BJOY_UP _Joy_IsOn(JOY_UP_MASK)
#define BJOY_DOWN _Joy_IsOn(JOY_DOWN_MASK)
#define BJOY_LEFT _Joy_IsOn(JOY_LEFT_MASK)
#define BJOY_RIGHT _Joy_IsOn(JOY_RIGHT_MASK)
#define BJOY_A _Joy_IsOn(JOY_BTN_1_MASK)
#define BJOY_B _Joy_IsOn(JOY_BTN_2_MASK)

u8 __fastcall__ _Joy_IsOn(u8 mask);
void Joy_Buffer();
void Joy_Clear();

#endif // __LYNX_JOYSTICK__