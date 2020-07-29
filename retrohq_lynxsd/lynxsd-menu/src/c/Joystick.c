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

#include <joystick.h>
#include "Joystick.h"
#include "Preferences.h"

/*
 ******************************************************************************
 Functions for joystick state processing
 ******************************************************************************
 */


static u8 joyState = 0;
static u8 previousJoyState = 0;
static u8 repeating = 0;

extern u8 inputDelay;


unsigned char __fastcall__ _Joy_IsOn(u8 mask, u8 isButton) {
	if (isButton) {
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

	// d-pad buttons trigger after timeout
	else {
		if ((joyState & mask) == 0) { // if button is not pressed, do nothing
			return 0;
		}
		joyState ^= mask; // reset buffered button state
		
		// delay processing if the countdown hasn't expired
		if (inputDelay == 0) {
			if (repeating)
			{
				inputDelay = (preferences[PREF_FAST_SCROLL] ? DIRECTION_DELAY_FRAMES_FAST : DIRECTION_DELAY_FRAMES_SLOW);
			}
			else
			{
				inputDelay = DIRECTION_DELAY_FRAMES_SLOW; // long initial delay to stop overshoot on first press
				repeating = 1;
			}
			return 1;
		}
		return 0;
	}

}


char _Key_GetHit() {
	char c = cgetc();

	// delay processing if the countdown hasn't expired
	if (inputDelay == 0) {
		inputDelay = DIRECTION_DELAY_FRAMES_FAST;
		return c;
	}
	return 0;
}


void Joy_Buffer() {
  joyState |= joy_read(JOY_1);
  if (joyState == 0) repeating = 0;
}

void Joy_Clear() {
	joyState = 0;
	previousJoyState = 0;
}