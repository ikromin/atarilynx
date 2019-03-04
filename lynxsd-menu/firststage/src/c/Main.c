#include "LynxSD.h"


/*
 ******************************************************************************
 This is the first stage loader that blanks the INSERT GAME screen and loads
 the actual menu loader code. It uses the bare minumum Lynx SD API so do not
 reuse any of the include files here.
 ******************************************************************************
 */

#define MSTERE0 ((volatile u8 *) 0xFD50)
#define MAPCTL ((volatile u8 *) 0xFFF9)


void main() {
	u8 *ptr;
	u8 count;

	ptr = (u8*) 0xfda0; // palette
	count = 0x20;
	while (count--)
	{
		*ptr++ = 0;
	}


	LynxSD_Init();

	if (LynxSD_OpenFile("menu2stg.bin") == FR_OK) {
		char szHeader[64];
		u8 nBlockSize;
		u16 nBlockSizeBytes;
		u32 nSize;
		u16 nBlockCount;

		LynxSD_ReadFile(szHeader, 64);
		
		nBlockSize = szHeader[5];
		nSize = LynxSD_GetFileSize();
		nSize -= 64;

		LynxSD_SeekFile(64);

		nBlockSizeBytes = nBlockSize << 8;
		nBlockCount = (nSize + nBlockSizeBytes - 1) / nBlockSizeBytes;

		LynxSD_ProgramROMFromFile(0, nBlockSize, nBlockCount, 0);
		LynxSD_CloseFile();
	}

	*MSTERE0 = 0; // enable all audio channels
	asm("sei");
	*MAPCTL = 0; // memory mapping for boot state

	ptr = (u8*) 0xfd00; // timers and audio fd00
	count = 0x40;//40
	while (count--)
	{
		*ptr++ = 0;
	}

	*((u8*) 0xFD80) = 0;
	*((u8*) 0xFD81) = 0;
	*((u8*) 0xFD92) = 0;
	*((u8*) 0xFD9C) = 0;
	*((u8*) 0xFD9D) = 0;
	*((u8*) 0xFD9E) = 0;
	*((u8*) 0xFD9D) = 0;

	asm("brk");
}
