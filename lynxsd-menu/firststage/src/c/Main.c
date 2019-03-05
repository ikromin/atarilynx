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
#define SCREEN_BUFFER1 0xC038

u8 gaPleaseWait[] =
{
	0xfc, 0xc0, 0xfe, 0x38, 0x78, 0xfe, 0x00, 0xc6, 0x38, 0x7e, 0x7e,
	0xc6, 0xc0, 0xc0, 0x6c, 0xcc, 0xc0, 0x00, 0xc6, 0x6c, 0x18, 0x18,
	0xc6, 0xc0, 0xc0, 0xc6, 0xc0, 0xc0, 0x00, 0xd6, 0xc6, 0x18, 0x18,
	0xc6, 0xc0, 0xfc, 0xc6, 0x7c, 0xfc, 0x00, 0xfe, 0xc6, 0x18, 0x18,
	0xfc, 0xc0, 0xc0, 0xfe, 0x06, 0xc0, 0x00, 0xfe, 0xfe, 0x18, 0x18,
	0xc0, 0xc0, 0xc0, 0xc6, 0xc6, 0xc0, 0x00, 0x6c, 0xc6, 0x18, 0x18,
	0xc0, 0xfe, 0xfe, 0xc6, 0x7c, 0xfe, 0x00, 0x44, 0xc6, 0x7e, 0x18
};

void main() {
	u8 *ptr;
	u16 count;
	u8 *screenMemory = (u8*) SCREEN_BUFFER1, *wait = gaPleaseWait;

	// set palette colour 0 to black
	*((volatile u8*) 0xfda0) = 0x00; // green 1
	*((volatile u8*) 0xfdb0) = 0x00; // blue / red 1

	// and set palette colour 1 to white
	*((volatile u8*) 0xfda1) = 0xff; // green 1
	*((volatile u8*) 0xfdb1) = 0xff; // blue / red 1

	// setup screen address pointer for LCD hardware
	*((volatile u8 *)0xFD94) = ((u16)screenMemory) & 0xff; // low byte of screen address
	*((volatile u8 *)0xFD95) = ((u16)screenMemory) >> 8; // high byte of screen address
	
	// clear the screen
	count = (160*102)/2;
	while (count--)
	{
		*screenMemory++ = 0;
	}

	// write PLEASE WAIT in center of screen
	for (count = 0; count < 7; count++)
	{
		u8 x = 11;
		screenMemory = (u8*) (SCREEN_BUFFER1 + 47*80) + (count * 80) + 18;
		while (x--)
		{
			u8 c = *wait++;
			u8 bit = 0x80;
			while (bit)
			{
				u8 out = 0;
				if (c & bit)
				{
					out |= 0x10;
				}
				bit >>= 1;
				if (c & bit)
				{
					out |= 0x01;
				}
				bit >>= 1;
				*screenMemory++ = out;
			}
		}
	}

	// enable LCD controller
	*((volatile u8 *)0xFD93) = 41; // refresh rate
	*((volatile u8 *)0xFD92) = 0xd;

	// now load up the proper menu

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
