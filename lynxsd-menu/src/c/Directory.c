#include <string.h>
#include "Directory.h"
#include "Preferences.h"

/*
 ******************************************************************************
 Functions for reading directory contents from the SD card
 ******************************************************************************
 */

#define ROM_LIST_FILE "romlist.txt"
#define MAX_DIR_ENTRIES 255

#define DIR_ENTRY_LFN_LEN 45
#define DIR_ENTRY_NAMES_LEN (8 + 4 + 1 + DIR_ENTRY_LFN_LEN + 1)

char gszCurrentDir[256] = "";
u8 gnSelectIndex = 0;
u8 gnNumDirEntries = 0;
u8 ganDirOrder[256];
SDirEntry gsDirEntry[256];

// buffer big enough to hold the 8.3 file name and the 45 character long name
#define FILENAME_BUFFER_SIZE 10240
char gsFilenameBuffer[FILENAME_BUFFER_SIZE];
char *gpFilenamePtr;

static u32 nReadCurOffset = 0;
static u32 nReadMaxOffset = 0;


static void __fastcall__ AddDirEntry(const char *pIn, const char *pLfnIn, u8 bIsDir, u8 isOverride)
{
	u8 nEntry = 0;
	s8 cmp = -1;
	u16 nTotalFileLength = strlen(pIn)+1 + (pLfnIn ? strlen(pLfnIn)+1 : 0);
	SDirEntry *pDir;

	if (gnNumDirEntries == MAX_DIR_ENTRIES) return;

  //-- Find place to insert entry
	while (nEntry < gnNumDirEntries) {
		pDir = &gsDirEntry[ganDirOrder[nEntry]];

		// directories first
		if (bIsDir && !pDir->bDirectory) break;

		cmp = stricmp(pIn, pDir->szFilename) >> 8; // shift 8 bits because we're using s8 instead of int

		// long ROM name replacement
		if (cmp == 0 && pLfnIn) 
		{
			if (((gsFilenameBuffer + FILENAME_BUFFER_SIZE) - gpFilenamePtr) > strlen(pLfnIn))
			{
				pDir->szLongName = gpFilenamePtr;
				strcpy(gpFilenamePtr, pLfnIn);
				while (*gpFilenamePtr++);
			}
			return;
		}

		if ((bIsDir == pDir->bDirectory) && cmp < 0) break;

		nEntry++;
	}

	if (isOverride) return;

  //-- Shuffle order list as needed
	if (nEntry < gnNumDirEntries) {
		u8 nEnd = gnNumDirEntries;
		while (nEnd > nEntry) {
			ganDirOrder[nEnd] = ganDirOrder[nEnd-1];
			nEnd--;
		}
	}

//-- Only add if there is enough space in the buffer

	if (((gsFilenameBuffer + FILENAME_BUFFER_SIZE) - gpFilenamePtr) >= nTotalFileLength)
	{

	  //-- Create the physical new entry
		pDir = &gsDirEntry[gnNumDirEntries];
		pDir->bDirectory = bIsDir;

	  //-- Store filenames in global buffer to save memory, if no long name provided, just duplicate the pointer
	
		pDir->szFilename = gpFilenamePtr;
		pDir->szLongName = gpFilenamePtr;
		strcpy(gpFilenamePtr, pIn);
		while (*gpFilenamePtr++);

		if (pLfnIn)
		{
			pDir->szLongName = gpFilenamePtr;
			strcpy(gpFilenamePtr, pLfnIn);
			while (*gpFilenamePtr++);
		}

	  //-- Insert into sort list
		ganDirOrder[nEntry] = gnNumDirEntries;
		gnNumDirEntries++;
	}
}


static FRESULT openFileForStreaming(const char *pFile) {
	FRESULT res = LynxSD_OpenFile(pFile);
	if (res == FR_OK) {
		nReadCurOffset = 0;
		nReadMaxOffset = LynxSD_GetFileSize();
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
	gpFilenamePtr = gsFilenameBuffer;

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
				u8 ignore = strcmp("MENU", sInfo.fname) == 0 || strcmp("_PREVIEW", sInfo.fname) == 0;
				// special case ignore the "menu" folder
				if (!((sInfo.fattrib & AM_DIR) && (ignore))) {
					AddDirEntry(sInfo.fname, 0, (sInfo.fattrib & AM_DIR) != 0, 0);
				}
			}
		}
	}

	// using ROM list file
	if (strcmp(TXT_PREFS_DIR, sInfo.fname) != 0 && preferences[PREF_LONG_NAMES]) {
		if (DIR_IsValidFilePath(ROM_LIST_FILE)) {
			char listFile[256];
			DIR_FullFilePath(listFile, ROM_LIST_FILE);

			// list file format is:
			// [romname.ext]Long ROM Name

			if (openFileForStreaming(listFile) == FR_OK) {
				char buf, fileLine[DIR_ENTRY_NAMES_LEN];
				u8 idx = 0, lfnIdx = 0, start83 = 0;

				while (nReadCurOffset < nReadMaxOffset) {
					if (LynxSD_ReadFile(&buf, 1) != FR_OK) { break; }
					
					if (buf == '[') {
						start83 = 1;
					}
					else if (buf == '\n' || buf == '\r') {
						if (lfnIdx > 1 && (idx - lfnIdx) <= DIR_ENTRY_LFN_LEN) {
							fileLine[idx] = 0;
							AddDirEntry(fileLine, &fileLine[lfnIdx], 0, 1);
						}
						idx = 0;
						lfnIdx = 0;
						start83 = 0;
					}
					else if (start83 && idx < DIR_ENTRY_NAMES_LEN - 1) {
						if (buf == ']') {
							fileLine[idx] = 0;
							lfnIdx = idx + 1;
						}
						else fileLine[idx] = buf;

						idx++;
					}
					
					nReadCurOffset++;
				}

				// get any entries that don't have a new line after them
				if (lfnIdx > 1 && (idx - lfnIdx) <= DIR_ENTRY_LFN_LEN) {
					fileLine[idx] = 0;
					AddDirEntry(fileLine, &fileLine[lfnIdx], 0, 1);
				}

				LynxSD_CloseFile();
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
