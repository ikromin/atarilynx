#ifndef __LYNX_SD_PREFS__
#define __LYNX_SD_PREFS__

#include "LynxSD.h"

#define PREF_BOOT_HELP 0
#define PREF_AUTO_RUN_ROM 1
#define PREF_FAST_SCROLL 2

extern u8 preferences[256];
extern u8 prefsShowing;

void PREFS_save();
void PREFS_show();
void PREFS_load();

#endif // __LYNX_SD_PREFS__
