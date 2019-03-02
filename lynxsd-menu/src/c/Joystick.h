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