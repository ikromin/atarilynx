#include <joystick.h>
#include "Joystick.h"

/*
 ******************************************************************************
 Functions for joystick state processing
 ******************************************************************************
 */

// a count down used to delay reading of joystick events
// should be decremented in the main game loop
u8 joystickActionDelay = 0;

static unsigned char joyState = 0;


unsigned char __fastcall__ Joy_IsOn(unsigned char mask) {
	if ((joyState & mask) == 0) { // if button is not pressed, do nothing
		return 0;
	}
	joyState ^= mask; // reset buffered button state

	// delay processing if the previous action countdown hasn't expired
	// this works more or less like a key debouncer
	if (joystickActionDelay == 0) {
		joystickActionDelay = ACTION_DELAY_FRAMES;
		return 1;
	}
	return 0;
}


void Joy_Buffer() {
  joyState |= joy_read(JOY_1);
}