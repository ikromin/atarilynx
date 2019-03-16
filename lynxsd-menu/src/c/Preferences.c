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

#include <string.h>
#include "Preferences.h"
#include "Directory.h"
#include "UI.h"

/*
 ******************************************************************************
 Functions for reading and writing preference options
 ******************************************************************************
 */

#define FILE_PREFS "menu/prefs"
#define NUM_PREFS 5

u8 preferences[NUM_PREFS] = { 1, 0, 1, 1, 0 }; // default preferences
u8 prefsShowing = 0;

static char* prefNames[NUM_PREFS] = {
	"BOOT HELP ON",
	"AUTO RUN ROM",
	"FAST SCROLL",
  "LONG NAMES",
	"USE _PREVIEW"
};


/**
 * Saves the current preferences state to memory and to file.
 */
void PREFS_save() {
	u16 nDelay = 65535L;
	u8 idx;

	for (idx = 0; idx < NUM_PREFS; idx++) {
		// 2 is the offset of the 'yes' preferences in the fileTypesMap
		preferences[idx] = (&gsDirEntry[idx])->bDirectory - 2;
	}

	if (LynxSD_OpenFile(FILE_PREFS) == FR_OK) {
		LynxSD_WriteFile(preferences, NUM_PREFS);
		LynxSD_CloseFile();
	}

	prefsShowing = 0;

	while (nDelay--);
}


/**
 * Reads preferences from a preferences file if it exists.
 */
void PREFS_load() {
	if (LynxSD_OpenFile(FILE_PREFS) == FR_OK) {
		LynxSD_ReadFile(preferences, NUM_PREFS);
		LynxSD_CloseFile();
	}
}


/**
 * Displays the list of preferences based on the preference names array.
 * Preferences are simulated using directory/file entries with bDirectory
 * set to the index into the fileTypesMap from UI.c.
 */
void PREFS_show() {
	SDirEntry *prefsPtr;
	u8 idx;

	prefsShowing = 1;
	UI_forwardAction();

	gnSelectIndex = 0;
	gnNumDirEntries = NUM_PREFS;
	gpFilenamePtr = gsFilenameBuffer;

	strcpy(gszCurrentDir, TXT_PREFS_DIR);
	
	for (idx = 0; idx < NUM_PREFS; idx++) {
		prefsPtr = &gsDirEntry[idx];

		prefsPtr->szFilename = gpFilenamePtr;
		prefsPtr->szLongName = gpFilenamePtr;

		strcpy(gpFilenamePtr, prefNames[idx]);
		while (*gpFilenamePtr++);
		
		prefsPtr->bDirectory = preferences[idx] + 2;
		ganDirOrder[idx] = idx;
	}
}

