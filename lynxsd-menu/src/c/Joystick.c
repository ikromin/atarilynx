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
			inputDelay = (preferences[PREF_FAST_SCROLL] ? DIRECTION_DELAY_FRAMES_FAST : DIRECTION_DELAY_FRAMES_SLOW);
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
}

void Joy_Clear() {
	joyState = 0;
	previousJoyState = 0;
}