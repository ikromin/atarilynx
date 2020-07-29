/******************************************************************************
 Turkey Puncher 3 Turbo: Hyper Punching. An April Fools' parody game.

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

static u8 joyState = 0;
static u8 previousJoyState = 0;

unsigned char __fastcall__ _Joy_IsOn(u8 mask) {
  // button off
  if ((joyState & mask) == 0) {
    if ((previousJoyState & mask) == 0) {
      return 0;
    }
    previousJoyState = joyState;
    return 1;
  }

  // button on
  previousJoyState = joyState; // cache button down state
  joyState ^= mask; // reset buffered button state
  
  return 0;
}

void Joy_Buffer() {
  joyState |= joy_read(JOY_1);
}