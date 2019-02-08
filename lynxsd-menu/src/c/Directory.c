#include "Directory.h"

/*
 ******************************************************************************
 Functions for reading directory contents from the SD card
 ******************************************************************************
 */


u8 gnNumDirEntries = 0;
u8 ganDirOrder[256];
SDirEntry gsDirEntry[256];
char gszCurrentDir[256] = "";
char gszCurrentFile[13] = "";


static void __fastcall__ AddDirEntry(const char *pIn, u8 bIsDir)
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
