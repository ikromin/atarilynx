#include "Main.h"

/*
 ******************************************************************************
 Functions for initialising the menu system and processing inputs
 ******************************************************************************
 */


static u8 waitingForInput = 0;
static u8 dirListLoaded = 0;

extern u8 joystickActionDelay;


/**
 * Launches the last loaded ROM if the toggle is active. Toggle is
 * based on the preferences option 'AUTO RUN ROM'. The 'B' button is used
 * to toggle it i.e. if the preference is yes and B is held down, it's
 * switched to no, and vise-versa.
 */
static unsigned char runLastROM() {
	char romFileName[256];
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
				waitingForInput = 1;
				UI_showFailScreen();
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
	char romFile[256];

	if (DIR_IsValidRomPath(pDir->szFilename)) {
		DIR_FullRomPath(romFile, pDir->szFilename);

		UI_showProgrammingScreen();

		// boot ROM if programming successful
		if (LynxSD_Program(romFile) == FR_OK) {
			u16 nDelay = 65535;

			//-- Write out the last rom played to the sd card
			if (LynxSD_OpenFile(FILE_LAST_ROM) == FR_OK) {
				LynxSD_WriteFile(romFile, 256);
				LynxSD_CloseFile();
			}

			while (nDelay--);

			UI_clear();
			LaunchROM();
		}
		else {
			waitingForInput = 1;
			UI_showFailScreen();
		}
	}
	else {
		waitingForInput = 1;
		UI_showFailScreen();
	}
}


/**
 * Changes current directory to the given directory
 */
static void changeToDirectory(char dirName[]) {
	// go back up a directory
	if (dirName[0] == '.') {
		char* dirPtr;

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
		dirPtr = gszCurrentDir + strlen(gszCurrentDir);
		while (--dirPtr >= gszCurrentDir) {
			if ((*dirPtr == '/') || (dirPtr == gszCurrentDir)) {
				*dirPtr = 0;
			}
		}

		UI_backAction();
	}
	// or go forward a directory by name
	else {
		UI_showLoadingScreen();

		if (gszCurrentDir[0]) strcat(gszCurrentDir, "/");
		strcat(gszCurrentDir, dirName);

		UI_forwardAction();
	}

	gnSelectIndex = 0;
	dirListLoaded = 0;
}


#define WAIT_FOR_INPUT_CHECK if (waitingForInput) { waitingForInput = 0; continue; }


/**
 *  Main process loop, check for button inputs and performs necessary actions.
 */
void processLoop() {
	SDirEntry *pDir;

	while (1) {
		Joy_Buffer();
		
		// process joystick buttons
		if (BJOY_A) {
			WAIT_FOR_INPUT_CHECK

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
		}
		else if (BJOY_B) {
			WAIT_FOR_INPUT_CHECK
			changeToDirectory(".");
		}

		// nothing else to do if waiting for inputs
		if (waitingForInput == 1) continue;

		// process joystick d-pad
		if (BJOY_UP) UI_selectPrevious();
		else if (BJOY_LEFT) UI_selectPrevious2();
		else if (BJOY_DOWN) UI_selectNext();
		else if (BJOY_RIGHT) UI_selectNext2();

		// process other buttons
		if (kbhit()) {
			char key = cgetc();
			switch (key) {
				case 'F':
					tgi_flip();
					break;
				case '1':
					PREFS_show();
					break;
				case '2':
					waitingForInput = 1;
					UI_showHelpScreen();
					break;
			}
		}

		// read in directory contents if required
		if (dirListLoaded == 0) {
			UI_showLoadingScreen();
			ReadDirectory(gszCurrentDir);
			dirListLoaded = 1;
		}
		
		// render the file/directory listing unless waiting for input
		if (!tgi_busy()) {
			UI_render();
			if (joystickActionDelay > 0) joystickActionDelay--;
		}
	};
}


void lynxInit() {
	// clear hardware pallete to start with a blank screen
	unsigned char* ptr = (unsigned char*) 0xfda0;
	unsigned char count = 0x20;
	while (count--) {
		*ptr++ = 0;
	}

	// install TGI driver
	tgi_install(&lynxtgi);
	tgi_setframerate(60);
	tgi_init();
	CLI();	
	while (tgi_busy());

	// install joystick driver
	joy_install(&lynxjoy);
}


void main(void) 
{
	// initialise the Lynx, UI and SD cart
	lynxInit();
	UI_init();
	LynxSD_Init();

	PREFS_load();

	// try to run the last loaded ROM, if it fails load directory contents
	// and start the input processing loop
	if (!runLastROM()) {
		// show help screen on startup unless disabled in preferences
		if (preferences[PREF_BOOT_HELP]) {
			waitingForInput = 1;
			UI_showHelpScreen();
		}

		processLoop();
	}
}
