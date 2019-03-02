#include "Main.h"

/*
 ******************************************************************************
 Functions for initialising the menu system and processing inputs
 ******************************************************************************
 */


static u8 waitingForAction = 0;
static u8 dirListLoaded = 0;
static u8 resetPalette = 0;
static u8 action = 0;
static char romFileName[256] = "";

/**
 * Launches the last loaded ROM if the toggle is active. Toggle is
 * based on the preferences option 'AUTO RUN ROM'. The 'B' button is used
 * to toggle it i.e. if the preference is yes and B is held down, it's
 * switched to no, and vise-versa.
 */
static u8 runLastROM() {
	u8 joyTriggered = (joy_read(JOY_1) & JOY_BTN_2_MASK) != 0;

	if (preferences[PREF_AUTO_RUN_ROM] ^ joyTriggered)
	{
		if (LynxSD_OpenFile(FILE_LAST_ROM) == FR_OK) {
			LynxSD_ReadFile(romFileName, 256);
			LynxSD_CloseFile();

			UI_showLastRomScreen(romFileName);
						
			if (LynxSD_Program(romFileName) == FR_OK) {
				UI_clear();
				LaunchROM();
				return 1;
			}
			else {
				waitingForAction = 1;
				UI_showFailScreen(0);
				return 1;
			}
		}
	}

	return 0;
}


/**
 * Launches the selected ROM file and saves it as the last loaded ROM
 */
void launchSelectedROM() {
	SDirEntry *pDir = &gsDirEntry[ganDirOrder[gnSelectIndex]];

	if (DIR_IsValidFilePath(pDir->szFilename)) {
		DIR_FullFilePath(romFileName, pDir->szFilename);

		UI_showProgrammingScreen();

		// boot ROM if programming successful
		if (LynxSD_Program(romFileName) == FR_OK) {
			u16 nDelay = 65535L;

			//-- Write out the last rom played to the sd card
			if (LynxSD_OpenFile(FILE_LAST_ROM) == FR_OK) {
				LynxSD_WriteFile(romFileName, 256);
				LynxSD_CloseFile();
			}

			while (nDelay--);

			UI_clear();
			LaunchROM();
		}
		else {
			waitingForAction = 1;
			UI_showFailScreen(0);
		}
	}
	else {
		waitingForAction = 1;
		UI_showFailScreen(0);
	}
}


/**
 * Changes current directory to the given directory
 */
static void changeToDirectory(char dirName[]) {
	// go back up a directory
	if (dirName[0] == '.') {
		u8 dirLen;

		// nothing to go back to if current directory is null
		if (gszCurrentDir[0] == 0) {
			return;
		}

		// save preferences if they are showing before going back to directory list
		if (prefsShowing) {
			PREFS_save();
		}

		// scan backwards through the current directory name setting characters to 0
		// until a slash is found or we've reached the start of the directory name
		dirLen = strlen(gszCurrentDir);
		while (dirLen > 0) {
			char c = gszCurrentDir[dirLen - 1];
			gszCurrentDir[dirLen - 1] = 0;
			if (c == '/') break;
			dirLen--;
		}

		UI_backAction();
	}
	// or go forward a directory by name
	else {
		if (gszCurrentDir[0]) strcat(gszCurrentDir, "/");
		strcat(gszCurrentDir, dirName);

		UI_forwardAction();
	}

	gnSelectIndex = 0;
	dirListLoaded = 0;
}


/**
 *  Main process loop, check for button inputs and performs necessary actions.
 */
void processLoop() {
	SDirEntry *pDir;

	while (1) {
		action = 0;
		Joy_Buffer();
		
		// process buttons input
		if (BJOY_A) { action = 'A'; }
		if (BJOY_B) { action = 'B'; }
		if (KEY_HIT) { action = KEY_STATE; }

		// check if we need to wait for action or clear to process actions
		if (waitingForAction) {
			if (action) {
				waitingForAction = 0;
				Joy_Clear();
			}
			continue;
		}

		// reset palette if required
		if (resetPalette) UI_resetPalette();

		// action any inputs if necessary
		switch (action) {
			case 'A':
				pDir = &gsDirEntry[ganDirOrder[gnSelectIndex]];
				switch (pDir->bDirectory) {
					case 0:
						launchSelectedROM();
						break;
					case 1:
						changeToDirectory(pDir->szFilename);
						break;
					case 2:
						pDir->bDirectory = 3;
						break;
					case 3:
						pDir->bDirectory = 2;
						break;
				}
				break;

			case 'B':
				changeToDirectory(".");
				break;
			
			case 'F':
				tgi_flip();
				break;

			case 'P':
				waitingForAction = UI_showPreviewScreen();
				if (waitingForAction) resetPalette = 1;
				break;

			case '1':
				PREFS_show();
				break;

			case '2':
				waitingForAction = 1;
				UI_showHelpScreen();
				break;
		}
	
		// read in directory contents if required
		if (dirListLoaded == 0) {
			UI_showLoadingDirScreen();
			DIR_read(gszCurrentDir);
			dirListLoaded = 1;
		}

		// process joystick d-pad
		if (BJOY_UP) UI_selectPrevious();
		else if (BJOY_LEFT) UI_selectPrevious2();
		else if (BJOY_DOWN) UI_selectNext();
		else if (BJOY_RIGHT) UI_selectNext2();
		
		if (!tgi_busy()) {
			UI_showDirectory();
		}
	}
}


void lynxInit() {
	// install TGI driver
	tgi_install(&lynxtgi);
	tgi_setframerate(60);
	tgi_init();
	CLI();
	
	tgi_setbgcolor(0);
	UI_showHelpScreen();
}


void main() {
	// initialise the Lynx, UI and SD cart
	lynxInit();
	LynxSD_Init();
	UI_init();

	PREFS_load();

	// install joystick driver
	joy_install(&lynxjoy);

	// try to run the last loaded ROM, if it fails load directory contents
	// and start the input processing loop
	if (!runLastROM()) {
		// show help screen on startup unless disabled in preferences
		if (preferences[PREF_BOOT_HELP]) {
			waitingForAction = 1;
		}

		processLoop();
	}
}
