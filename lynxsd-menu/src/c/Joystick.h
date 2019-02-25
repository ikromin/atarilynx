#ifndef __LYNX_SD_JOYSTICK__
#define __LYNX_SD_JOYSTICK__

#include "Types.h"

// macros for joystick states
#define BJOY_UP Joy_IsOn(JOY_UP_MASK)
#define BJOY_DOWN Joy_IsOn(JOY_DOWN_MASK)
#define BJOY_LEFT Joy_IsOn(JOY_LEFT_MASK)
#define BJOY_RIGHT Joy_IsOn(JOY_RIGHT_MASK)
#define BJOY_A Joy_IsOn(JOY_BTN_1_MASK)
#define BJOY_B Joy_IsOn(JOY_BTN_2_MASK)

#define ACTION_DELAY_FRAMES_FAST 10
#define ACTION_DELAY_FRAMES_SLOW 20

extern u8 joystickActionDelay;

u8 __fastcall__ Joy_IsOn(u8 mask);
void Joy_Buffer();

#endif // __LYNX_SD_JOYSTICK__