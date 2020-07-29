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

#include <joystick.h>
#include "Joystick.h"

/*
 ******************************************************************************
 Functions for joystick state processing
 ******************************************************************************
 */

unsigned char __fastcall__ _Joy_IsOn(u8 mask) {
  // button off
  if ((_engJoyState & mask) == 0) {
    if ((_engPreviousJoyState & mask) == 0) {
      return 0;
    }
    _engPreviousJoyState = _engJoyState;
    return 1;
  }

  // button on
  _engPreviousJoyState = _engJoyState; // cache button down state
  _engJoyState ^= mask; // reset buffered button state
  
  return 0;
}

void Joy_Buffer() {
  _engJoyState |= joy_read(JOY_1);
}

void Joy_Clear() {
  _engJoyState = 0;
}