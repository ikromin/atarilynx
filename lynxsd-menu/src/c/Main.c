#include "Main.h"


void initialize()
{
	tgi_install(&lynxtgi);
	tgi_setpalette(blackpal);
	tgi_init();
	CLI();
	
	while (tgi_busy()) 
	{ 
	};

	tgi_setpalette(masterpal);

	tgi_setcolor(9);
	tgi_setbgcolor(0); 
	tgi_clear();

	joy_install(&lynxjoy);
}


void main(void) 
{
	LynxSD_Init();

	initialize();

	tgi_setcolor(9);
	tgi_setbgcolor(0);

	tgi_clear();
	tgi_updatedisplay();


	if (LynxSD_OpenFile("menu/lastrom") == FR_OK)
	{
		char *pFile;

		LynxSD_ReadFile(gszCurrentDir, 256);
		LynxSD_CloseFile();

		//-- If A is held during boot, load the last ROM up
		if (JOY_BTN_1(joy_read(JOY_1)))
		{
			tgi_clear();
			tgi_setbgcolor(0);
			tgi_setcolor(9);
			tgi_outtextxy(24, 46, "Programming...");
			tgi_setbgcolor(0);
			tgi_updatedisplay();
						
			if (LynxSD_Program(gszCurrentDir) == FR_OK)
			{
				tgi_setpalette(blackpal);
				tgi_clear();
				LaunchROM();
			}
		}

		pFile = gszCurrentDir + strlen(gszCurrentDir);
		while (pFile > gszCurrentDir && *pFile != '/') pFile--;
		if (*pFile == '/')
		{
			*pFile++ = 0;
			strcpy(gszCurrentFile, pFile);
		}
		else
		{
			strcpy(gszCurrentFile, gszCurrentDir);
			gszCurrentDir[0] = 0;
		}
	}

	runUI();
}
