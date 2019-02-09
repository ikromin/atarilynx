#include "Main.h"


void initialize()
{
	tgi_install(&lynxtgi);
	tgi_init();
	CLI();	
	while (tgi_busy());

	UI_init();

	joy_install(&lynxjoy);
}


unsigned char runLastROM() {
	char romFileName[256];

	//-- If A is held during boot, load the last ROM up
	if (JOY_BTN_1(joy_read(JOY_1)))
	{
		if (LynxSD_OpenFile("menu/lastrom") == FR_OK)
		{
			LynxSD_ReadFile(romFileName, 256);
			LynxSD_CloseFile();

			UI_showLastRomScreen(romFileName);
						
			if (LynxSD_Program(romFileName) == FR_OK)
			{
				tgi_setpalette(blackpal);
				tgi_clear();
				LaunchROM();
				return 1;
			}
		}
	}

	return 0;
}


void main(void) 
{
	initialize();
	LynxSD_Init();

	if (!runLastROM()) {
		UI_showLoadingScreen();
		UI_run();
	}
	
}
