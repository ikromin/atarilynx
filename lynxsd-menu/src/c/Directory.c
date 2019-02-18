#include <string.h>
#include "LynxSD.h"
#include "Directory.h"

/*
 ******************************************************************************
 Functions for reading directory contents from the SD card
 ******************************************************************************
 */

char gszCurrentDir[256] = "";
u8 gnSelectIndex = 0;
u8 gnNumDirEntries = 0;
u8 ganDirOrder[256];
SDirEntry gsDirEntry[256];


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


/**
 * Reads a directory and stores results in the gsDirEntry array.
 */
void __fastcall__ DIR_read(const char *pDir)
{
	SFileInfo sInfo;
	const char *pExt;
	u8 bIsLnxFile;
 
	gnNumDirEntries = 0;

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
					AddDirEntry(sInfo.fname, (sInfo.fattrib & AM_DIR) != 0 );
				}
			}
		}
	}
}


/**
 * Checks whether a given ROM file path is valid
 */
u8 __fastcall__ DIR_IsValidRomPath(const char romFile[]) {
	if ((strlen(gszCurrentDir) + strlen(romFile) + 2) <= 255) {
		return 1;
	}
	return 0;
}


/**
 * Creates the full path to a ROM file based on the current directory
 * and the given ROM file name.
 */
void __fastcall__ DIR_FullRomPath(char buf[], const char romFile[]) {
	strcpy(buf, gszCurrentDir);
	if (buf[0]) strcat(buf, "/");
	strcat(buf, romFile);
}
