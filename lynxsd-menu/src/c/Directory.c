#include <string.h>
#include "Directory.h"
#include "Preferences.h"

/*
 ******************************************************************************
 Functions for reading directory contents from the SD card
 ******************************************************************************
 */

#define ROM_LIST_FILE "romlist.txt"

char gszCurrentDir[256] = "";
u8 gnSelectIndex = 0;
u8 gnNumDirEntries = 0;
u8 ganDirOrder[256];
SDirEntry gsDirEntry[256];

u32 gnReadCurOffset = 0;
u32 gnReadMaxOffset = 0;


static void __fastcall__ AddDirEntry(const char *pIn, const char *pLfnIn, u8 bIsDir)
{
	u8 nEntry = 0;
	s8 cmp;
	SDirEntry *pDir;

	if (gnNumDirEntries == 255) return;

  //-- Find place to insert entry
	while (nEntry < gnNumDirEntries) {
		pDir = &gsDirEntry[ganDirOrder[nEntry]];

		// directories first
		if (bIsDir && !pDir->bDirectory) break;

		cmp = stricmp(pIn, pDir->szFilename) >> 8; // shift 8 bits because we're using s8 instead of int

		if (cmp == 0) return; // same file name so ignore
		if ((bIsDir == pDir->bDirectory) && cmp < 0) break;

		nEntry++;
	}

  //-- Shuffle order list as needed
	if (nEntry < gnNumDirEntries) {
		u8 nEnd = gnNumDirEntries;
		while (nEnd > nEntry) {
			ganDirOrder[nEnd] = ganDirOrder[nEnd-1];
			nEnd--;
		}
	}

  //-- Create the physical new entry
	pDir = &gsDirEntry[gnNumDirEntries];
	strcpy(pDir->szFilename, pIn);
	strcpy(pDir->szLongName, pLfnIn);
	pDir->bDirectory = bIsDir;

  //-- Insert into sort list
	ganDirOrder[nEntry] = gnNumDirEntries;
	gnNumDirEntries++;
}


static FRESULT openFileForStreaming(const char *pFile) {
	FRESULT res = LynxSD_OpenFile(pFile);
	if (res == FR_OK) {
		gnReadCurOffset = 0;
		gnReadMaxOffset = LynxSD_GetFileSize();
	}
	return res;
}


/**
 * Reads a directory and stores results in the gsDirEntry array.
 */
void __fastcall__ DIR_read(const char *pDir) {
	SFileInfo sInfo;
	const char *pExt;
	u8 bIsLnxFile;
 
	gnNumDirEntries = 0;

	// using ROM list file
	if (strcmp(TXT_PREFS_DIR, sInfo.fname) != 0 && preferences[PREF_LONG_NAMES]) {
		if (DIR_IsValidFilePath(ROM_LIST_FILE)) {
			char listFile[256];
			DIR_FullFilePath(listFile, ROM_LIST_FILE);

			// list file format is:
			// [romname.ext]Long Name up to 50 chars

			if (openFileForStreaming(listFile) == FR_OK) {
				char buf, fileLine[64];
				u8 idx = 0, lfnIdx = 0, start83 = 0;

				while (gnReadCurOffset < gnReadMaxOffset) {
					if (LynxSD_ReadFile(&buf, 1) != FR_OK) { break; }
					
					if (buf == '[') {
						start83 = 1;
					}
					else if (buf == '\n') {
						if (lfnIdx > 1 && (idx - lfnIdx) <= 50) {
							fileLine[idx] = 0;
							AddDirEntry(fileLine, &fileLine[lfnIdx], 0);
						}
						idx = 0;
						lfnIdx = 0;
						start83 = 0;
					}
					else if (start83 && idx < 63) {
						if (buf == ']') {
							fileLine[idx] = 0;
							lfnIdx = idx + 1;
						}
						else fileLine[idx] = buf;

						idx++;
					}
					
					gnReadCurOffset++;
				}

				// get any entries that don't have a new line after them
				if (lfnIdx > 1 && (idx - lfnIdx) <= 50) {
					fileLine[idx] = 0;
					AddDirEntry(fileLine, &fileLine[lfnIdx], 0);
				}

				LynxSD_CloseFile();
			}
		}
	}

	//-- Open and read the dir
	if (LynxSD_OpenDir(pDir) == FR_OK) {
		while (LynxSD_ReadDir(&sInfo) == FR_OK) {
			bIsLnxFile = 0;
			pExt = sInfo.fname;
			
			while (*pExt && *pExt != '.') {
				pExt++;
			}

			if (*pExt == '.') {
				pExt++;
				if (((pExt[0] == 'L' && ((pExt[1] == 'N') || (pExt[1] == 'Y')) && pExt[2] == 'X')) ||
					((pExt[0] == 'O') && (pExt[1] == 0)) ||
					((pExt[0] == 'C') && (pExt[1] == 'O') && (pExt[2] == 'M')))
				{
					bIsLnxFile = 1;
				}
			}

			if ((bIsLnxFile || (sInfo.fattrib & AM_DIR)) && ((sInfo.fattrib & (AM_SYS|AM_HID)) == 0)) {
				// special case ignore the "menu" folder
				if (!((sInfo.fattrib & AM_DIR) && (strcmp("MENU", sInfo.fname) == 0))) {
					AddDirEntry(sInfo.fname, sInfo.fname, (sInfo.fattrib & AM_DIR) != 0 );
				}
			}
		}
	}

}


/**
 * Checks whether a given file path is valid
 */
u8 __fastcall__ DIR_IsValidFilePath(const char romFile[]) {
	if ((strlen(gszCurrentDir) + strlen(romFile) + 2) <= 255) {
		return 1;
	}
	return 0;
}


/**
 * Creates the full path to a file based on the current directory
 * and the given file name.
 */
void __fastcall__ DIR_FullFilePath(char buf[], const char file[]) {
	strcpy(buf, gszCurrentDir);
	if (buf[0]) strcat(buf, "/");
	strcat(buf, file);
}
