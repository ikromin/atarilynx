#include "GameMenu.h"
#include "LynxSD.h"
#include "Program.h"

char preview[8365];

u8 nPreviewDelay = 0;
u8 nRevertPalette = 0;
u8 nDisplayIcon = 0;
u8 gnIconLine = 255;

unsigned char masterpal[]={0x01,0x01,0x01,0x05,0x08,0x0B,0x0D,0x03,0x05,0x0F,0x03,0x07,0x0A,0x0C,0x0F,0x09,
														0x11,0x11,0x03,0x07,0x6B,0x9E,0xBF,0x49,0x55,0xFF,0x33,0x77,0xAA,0x66,0x3F,0x15};

unsigned char currentpal[]={0x01,0x01,0x01,0x05,0x08,0x0B,0x0D,0x03,0x05,0x0F,0x03,0x07,0x0A,0x0C,0x0F,0x09,
														0x11,0x11,0x03,0x07,0x6B,0x9E,0xBF,0x49,0x55,0xFF,0x33,0x77,0xAA,0x66,0x3F,0x15};

unsigned char blackpal[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
														0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

SCB_REHV_PAL iconsprite =  {
	BPP_4 | TYPE_NONCOLL,
	LITERAL | REHV,
	0x00,
  0x0000,
  &icon[0],
  106,
	0,
	0xFF,
	0xFF,
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};

SCB_REHV_PAL previewsprite =  {
	BPP_4 | TYPE_NONCOLL,
	LITERAL | REHV,
	0x00,
  0x0000,
  &preview[0],
  0,
	0,
	0x100,
	0x100,
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};


////////////////////////////////////////////////////////////////////////////////
//
// Lynx registers
//
////////////////////////////////////////////////////////////////////////////////

#define MSTERE0 ((volatile u8 *) 0xFD50)
#define MAPCTL ((volatile u8 *) 0xFFF9)

////////////////////////////////////////////////////////////////////////////////
//
// Globals
//
////////////////////////////////////////////////////////////////////////////////

static SDirEntry gsDirEntry[256];
static u8 ganDirOrder[256];
static u8 gnNumDirEntries = 0;
static u8 gnTopLine = 0, gnSelectedLine = 0;

static char gszCurrentDir[256] = "";
static char gszCurrentFile[13] = "";

////////////////////////////////////////////////////////////////////////////////
//
// Menu specific ROM laoding moved to menu source file
//
////////////////////////////////////////////////////////////////////////////////

u8 GetLightColor()
{
	u8 pen = 0;
	u8 green = 0;
	u8 iX = 0;
	for (iX=0;iX<16;iX++)
	{
		if (currentpal[iX] > green)
		{
			green = currentpal[iX];
			pen = iX;
		}
	}
	return pen;
}

u8 GetDarkColor()
{
	u8 pen = 0;
	u8 green = 255;
	u8 iX = 0;
	for (iX=0;iX<16;iX++)
	{
		if ((currentpal[iX] < green) && (iX != 0))
		{
			green = currentpal[iX];
			pen = iX;
		}
	}
	return pen;
}

FRESULT __fastcall__ LynxSD_ReadIcon(const char *pFilename, char *image, char *palette)
{
	FRESULT res = FR_DISK_ERR;
	if (LynxSD_OpenFile(pFilename) == FR_OK)
	{	
		if (LynxSD_ReadFile(image, 8365) == FR_OK)
		{
			if (LynxSD_ReadFile(palette, 32) == FR_OK)
			{
				res = FR_OK;
			}
		}
		LynxSD_CloseFile();
	}

	return res;
}

////////////////////////////////////////////////////////////////////////////////
//
// Game menu code
//
////////////////////////////////////////////////////////////////////////////////

void __fastcall__ AddDirEntry(const char *pIn, u8 bIsDir)
{
	u8 nEntry = 0;
	SDirEntry *pDir;

	if (gnNumDirEntries == 255) return;

//-- Find place to insert entry

	while (nEntry < gnNumDirEntries)
	{
		pDir = &gsDirEntry[ganDirOrder[nEntry]];

		if (bIsDir && !pDir->bDirectory) // directories first
		{
			break;
		}

		if ((bIsDir == pDir->bDirectory) && (stricmp(pIn, pDir->szFilename) < 0))
		{
			break;
		}

		nEntry++;
	}

//-- Shuffle order list as needed

	if (nEntry < gnNumDirEntries)
	{
		u8 nEnd = gnNumDirEntries;
		while (nEnd > nEntry)
		{
			ganDirOrder[nEnd] = ganDirOrder[nEnd-1];
			nEnd--;
		}
	}

//-- Create the physical new entry

	pDir = &gsDirEntry[gnNumDirEntries];
	strcpy(pDir->szFilename, pIn);
	pDir->bDirectory = bIsDir;

//-- Insert into sort list

	ganDirOrder[nEntry] = gnNumDirEntries;
	gnNumDirEntries++;
}

void __fastcall__ ReadDirectory(const char *pDir)
{
	SFileInfo sInfo;
	const char *pExt;
	u8 bIsLnxFile;

	tgi_clear();
	tgi_setcolor(13);
	tgi_outtextxy(40, 36, "Menu v1.8");
	tgi_outtextxy(40, 46, "Reading...");
	tgi_setcolor(9);
	tgi_updatedisplay();
 
//-- If this is a subdir, add ".." as first entry

	gnNumDirEntries = 0;
	if (pDir[0] != 0)
	{
		AddDirEntry("..", 1);
	}

//-- Open and read the dir

	if (LynxSD_OpenDir(pDir) == FR_OK)
	{
		while (LynxSD_ReadDir(&sInfo) == FR_OK)
		{
			bIsLnxFile = 0;
			pExt = sInfo.fname;
			while (*pExt && *pExt != '.')
			{
				pExt++;
			}
			if (*pExt == '.')
			{
				pExt++;
				if (((pExt[0] == 'L' && ((pExt[1] == 'N') || (pExt[1] == 'Y')) && pExt[2] == 'X')) ||
					((pExt[0] == 'O') && (pExt[1] == 0)) ||
					((pExt[0] == 'C') && (pExt[1] == 'O') && (pExt[2] == 'M')))
				{
					bIsLnxFile = 1;
				}
			}

			if ((bIsLnxFile || (sInfo.fattrib & AM_DIR)) && ((sInfo.fattrib & (AM_SYS|AM_HID)) == 0))
			{
				if (!((sInfo.fattrib & AM_DIR) && (strcmp("MENU", sInfo.fname) == 0))) // special case ignore the "menu" folder
				{
					AddDirEntry(sInfo.fname, sInfo.fattrib & AM_DIR);
				}
			}
		}
	}
}

void DisplayDirectory()
{
	SDirEntry *pDir;
	char pIcon[256];

	u8 i = 0;
	u8 line = 0;
	u8 nMax = gnNumDirEntries - gnTopLine;

	if (nMax > 12)
	{
		nMax = 12;
	}
	nMax += gnTopLine;
	line = gnTopLine;
 
	tgi_clear();

	while (line < nMax)
	{
		u8 nScreenLine = line - gnTopLine;

		pDir = &gsDirEntry[ganDirOrder[line]];
		if (pDir->bDirectory)
		{
			if (nDisplayIcon == 0)
			{
				tgi_setcolor(12);
				tgi_outtextxy(8, 3+(nScreenLine*8), "[");
				tgi_outtextxy(16, 3+(nScreenLine*8), pDir->szFilename);
				tgi_outtextxy(16+(strlen(pDir->szFilename)*8), 3+(nScreenLine*8), "]");
				tgi_setcolor(9);
			}
		}
		else
		{
			if (nDisplayIcon == 0)
			{
				tgi_outtextxy(8, 3+(nScreenLine*8), pDir->szFilename);
			}
		}

		if (nPreviewDelay < 255)
		{
			nPreviewDelay++;
		}

		if (gnSelectedLine == line)
		{
			if (nDisplayIcon == 0)
			{
				tgi_setcolor(14);
				tgi_outtextxy(0, 3+(nScreenLine*8), ">");
				tgi_setcolor(9);
			}

			if (nPreviewDelay > 192)
			{
				if (gnSelectedLine != gnIconLine)
				{

					strcat(pIcon,"_PREVIEW/");

					i = 0;
					while ((pDir->szFilename[i] != 46) && (i < 13))
					{
						pIcon[9+i] = pDir->szFilename[i];
						i++;
					}

					pIcon[i+9] = 46; //.
					pIcon[i+10] = 76; //L
					pIcon[i+11] = 83; //S
					pIcon[i+12] = 68; //D
					pIcon[i+13] = 00; //null

					if (LynxSD_ReadIcon(pIcon,preview,currentpal) == FR_OK)
					{
						tgi_setpalette(currentpal);
						nDisplayIcon = 1;
					}
					else
					{
						nDisplayIcon = 0;
					}

					gnIconLine = gnSelectedLine;	
				}
			}// end of delay check
		}

		line++;
	}
	
	if (nDisplayIcon == 0)
	{
		tgi_sprite(&iconsprite);
	}
	else	
	{
		tgi_sprite(&previewsprite);
	}

	if (nRevertPalette == 1)
	{
		tgi_setpalette(masterpal);
		nRevertPalette = 0;
	}

	tgi_updatedisplay();
}

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

void CancelPreview()
{
	nPreviewDelay = 0;
	nDisplayIcon = 0;
	nRevertPalette = 1;
	gnIconLine = 255;
}

void LaunchROM()
{
		u8 *ptr;
		u8 count;

		tgi_setpalette(blackpal);

		LynxSD_LowPowerMode();
		*MSTERE0 = 0; // enable all audio channels
		asm("sei");
		*MAPCTL = 0; // memory mapping for boot state

		ptr = (u8*) 0xfd00; // timers and audio fd00
		count = 0x40;//40
		while (count--)
		{
			*ptr++ = 0;
		}

		tgi_clear();

		*((u8*) 0xFD80) = 0;
		*((u8*) 0xFD81) = 0;
		*((u8*) 0xFD92) = 0;
		*((u8*) 0xFD9C) = 0;
		*((u8*) 0xFD9D) = 0;
		*((u8*) 0xFD9E) = 0;
		*((u8*) 0xFD9D) = 0;

		ptr = (u8*) 0xfda0; // palette
		count = 0x20;
		while (count--)
		{
			*ptr++ = 0;
		}

		asm("brk");	
}

void main(void) 
{	
	u8 nJoyDown = 0, nJoyUp = 0, nLastJoy = 0, nJoyDownDelay = 0, nJoyUpDelay = 0;
	u8 nJoyRight = 0, nJoyLeft = 0, nJoyRightDelay = 0, nJoyLeftDelay = 0;

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

	ReadDirectory(gszCurrentDir);

	if (gszCurrentFile[0])
	{
		u8 n;
		for (n = 0; n < gnNumDirEntries; n++)
		{
			SDirEntry *pDir = &gsDirEntry[ganDirOrder[n]];
			if (stricmp(pDir->szFilename, gszCurrentFile) == 0)
			{
				gnSelectedLine = n;
				break;
			}
		}
	}

	while (1)
	{
		if (!tgi_busy())
		{
			u8 nThisJoy = joy_read(JOY_1);
			u8 nPressed = nThisJoy & ~nLastJoy;
			u8 nReleased = ~nThisJoy & nLastJoy;
			nLastJoy = nThisJoy;

		//-- Reset the repeat counters

			if (JOY_UP(nPressed|nReleased))
			{
				nJoyUp = 0;
				nJoyUpDelay = 14;
			}
			if (JOY_DOWN(nPressed|nReleased))
			{
				nJoyDown = 0;
				nJoyDownDelay = 14;
			}

			if (JOY_RIGHT(nPressed|nReleased))
			{
				nJoyRight = 0;
				nJoyRightDelay = 14;
			}

			if (JOY_LEFT(nPressed|nReleased))
			{
				nJoyLeft = 0;
				nJoyLeftDelay = 14;
			}

		//-- Update the repeat counters

			if (JOY_UP(nThisJoy))
			{
				nJoyUp++;
				if (nJoyUp > nJoyUpDelay)
				{
					nJoyUp = 1;
					nJoyUpDelay = 2;
				}
			}
			if (JOY_DOWN(nThisJoy))
			{
				nJoyDown++;
				if (nJoyDown > nJoyDownDelay)
				{
					nJoyDown = 1;
					nJoyDownDelay = 2;
				}
			}

			if (JOY_RIGHT(nThisJoy))
			{
				nJoyRight++;
				if (nJoyRight > nJoyRightDelay)
				{
					nJoyRight = 1;
					nJoyRightDelay = 2;
				}
			}

			if (JOY_LEFT(nThisJoy))
			{
				nJoyLeft++;
				if (nJoyLeft > nJoyLeftDelay)
				{
					nJoyLeft = 1;
					nJoyLeftDelay = 2;
				}
			}

		//-- Do up / down

			if (nJoyUp == 1)
			{
				if (gnSelectedLine > 0)
				{
					gnSelectedLine--;
					CancelPreview();
				}
			}

			if (nJoyDown == 1)
			{
				if (gnSelectedLine < (gnNumDirEntries-1))
				{
					gnSelectedLine++;
					CancelPreview();
				}
			}

			if (nJoyRight == 1)
			{
				gnSelectedLine+=11;
				CancelPreview();

				if (gnSelectedLine > (gnNumDirEntries-1))
				{
					gnSelectedLine = gnNumDirEntries-1;
				}
			}

			if (nJoyLeft == 1)
			{
				gnSelectedLine-=11;
				CancelPreview();

				if ((gnSelectedLine <= 0) || (gnSelectedLine > (gnNumDirEntries-1)))
				{
					gnSelectedLine = 0;
				}
			}

			//if ((nJoyUp == 1) || (nJoyDown == 1))
			{
				if (gnSelectedLine < gnTopLine)
				{
					gnTopLine = gnSelectedLine;
					//gnSelectedLine = gnTopLine+12;
				}
				else if (gnSelectedLine >= (gnTopLine + 12))
				{
					gnTopLine = gnSelectedLine-11;
					//gnSelectedLine = gnTopLine;
				}
			}

		//-- Selection

			if (kbhit())
			{
				switch (cgetc()) 
					{ 
						case 'F': 
							{
										tgi_flip(); 
										break; 
							}
					}

			}

			if (JOY_BTN_1(nReleased))
			{
				SDirEntry *pDir = &gsDirEntry[ganDirOrder[gnSelectedLine]];

			//-- Check for directory actions

				if (pDir->bDirectory)
				{
					u8 bDoRead = 0;

				//-- Do back up directory

					if (pDir->szFilename[0] == '.' && pDir->szFilename[1] == '.' && pDir->szFilename[2] == 0)
					{
						char *pEnd = gszCurrentDir + strlen(gszCurrentDir);
						while (--pEnd >= gszCurrentDir)
						{
							if ((*pEnd == '/') || (pEnd == gszCurrentDir))
							{
								*pEnd = 0;
								bDoRead = 1;
								break;
							}
						}

					}

				//-- Do forward directory
					
					else
					{
						if ((strlen(gszCurrentDir) + strlen(pDir->szFilename) + 2) <= 255)
						{
							if (gszCurrentDir[0])
							{
								strcat(gszCurrentDir, "/");
							}
							strcat(gszCurrentDir, pDir->szFilename);
							bDoRead = 1;
						}
					}

					if (bDoRead)
					{
						ReadDirectory(gszCurrentDir);
						gnTopLine = 0;
						gnSelectedLine = 0;
						continue;
					}
				}

			//-- Otherwise its a file

				else
				{
					FRESULT res = FR_DISK_ERR;
					u8 iX = 0;
					char gszOldDir[256];

					//store old directory in case the file is bad
					strcpy(gszOldDir, gszCurrentDir);

					if ((strlen(gszCurrentDir) + strlen(pDir->szFilename) + 2) <= 255)
					{
						char *pCurrentEnd = gszCurrentDir + strlen(gszCurrentDir);
						if (gszCurrentDir[0])
						{
							strcat(gszCurrentDir, "/");
						}
						strcat(gszCurrentDir, pDir->szFilename);

						tgi_clear();
						if (nDisplayIcon == 1)
						{
							tgi_sprite(&previewsprite);
							tgi_setbgcolor(GetDarkColor());
							tgi_setcolor(GetLightColor());
						}
						tgi_outtextxy(24, 46, "Programming...");
						tgi_setbgcolor(0);
						//tgi_outtextxy(34, 0, gszCurrentDir);
						tgi_setbgcolor(0);
						tgi_setcolor(9);
						tgi_updatedisplay();

						
			
						res = LynxSD_Program(gszCurrentDir);

						//boot ROM
						if (res == FR_OK)
						{
							u16 nDelay = 65535;

						//-- Write out the last rom played to the sd card
						
							if (LynxSD_OpenFile("menu/lastrom") == FR_OK)
							{
								LynxSD_WriteFile(gszCurrentDir, 256);
								LynxSD_CloseFile();
							}

							while (nDelay--);

							LaunchROM();
							*pCurrentEnd = 0;
						}
						else
						{
							//rom bad, just go back to the previous directory entry selected
							strcpy(gszCurrentDir,gszOldDir);
						}
					}
				}
			}
			else
			{
				if (JOY_BTN_2(nReleased))
				{
					SDirEntry *pDir = &gsDirEntry[ganDirOrder[0]];
					gnSelectedLine = 0;

					CancelPreview();

					if (pDir->bDirectory)
					{
						u8 bDoRead = 0;
						if (pDir->szFilename[0] == '.' && pDir->szFilename[1] == '.' && pDir->szFilename[2] == 0)
						{
							char *pEnd = gszCurrentDir + strlen(gszCurrentDir);
							while (--pEnd >= gszCurrentDir)
							{
								if ((*pEnd == '/') || (pEnd == gszCurrentDir))
								{
									*pEnd = 0;
									bDoRead = 1;
									break;
								}
							}

						}
						if (bDoRead)
						{
							ReadDirectory(gszCurrentDir);
							gnTopLine = 0;
							gnSelectedLine = 0;
							continue;
						}
					}
				}
			}

			DisplayDirectory();
		}
	};
}
