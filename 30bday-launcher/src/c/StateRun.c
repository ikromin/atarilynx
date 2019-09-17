/******************************************************************************
 Atari Gamer's Atari Lynx 30th Birthday Competition ROM Launcher

 Copyright (C) 2019 Igor Kromin

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <https://www.gnu.org/licenses/>.
******************************************************************************/

#include "TgiExtra.h"
#include "Joystick.h"
#include "State.h"
#include "LynxSD.h"

/*
 ******************************************************************************
 Run screen code
 ******************************************************************************
 */

// Lynx registers
#define MSTERE0 ((volatile u8 *) 0xFD50)
#define MAPCTL ((volatile u8 *) 0xFFF9)
#define USE_SD_EEPROM_MASK 0x40
static u8 bLaunchLowPower = 1;

static u8 doRun = 0;

void LaunchROM()
{
		u8 *ptr;
		u8 count;

		if (bLaunchLowPower) LynxSD_LowPowerMode();

		asm("sei");
		*MSTERE0 = 0; // enable all audio channels
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

		ptr = (u8*) 0xfda0; // palette
		count = 0x20;
		while (count--)
		{
			*ptr++ = 0;
		}

		asm("brk");	
}

FRESULT __fastcall__ LynxSD_Program(const char *pFilename)
{
	FRESULT res = FR_DISK_ERR;
	bLaunchLowPower = 1; // default ROMs are launched and SD Cart MCU is switched off

	if (LynxSD_OpenFile(pFilename) == FR_OK) {
		char szHeader[64];
		LynxSD_ReadFile(szHeader, 64);

	  //-- Otherwise check for actual LNX ROM file
		if ((szHeader[0] == 'L' && szHeader[1] == 'Y' && szHeader[2] == 'N' && szHeader[3] == 'X')) {
			u8 nBlockSize;
			u16 nBlockSizeBytes;
			u32 nSize;
			u16 nBlockCount;

			// determine if SD Cart MCU should keep running
			if (szHeader[60] & USE_SD_EEPROM_MASK) {
				bLaunchLowPower = 0;
			}
			
			nBlockSize = szHeader[5];

			if ((nBlockSize == 1 || nBlockSize == 2 || nBlockSize == 4 || nBlockSize == 8)) {
				nSize = LynxSD_GetFileSize();
				nSize -= 64;

				LynxSD_SeekFile(64);

				nBlockSizeBytes = nBlockSize << 8;
				nBlockCount = (nSize + nBlockSizeBytes - 1) / nBlockSizeBytes;

				res = LynxSD_ProgramROMFromFile(0, nBlockSize, nBlockCount, 0);
				LynxSD_CloseFile();
			}
		}
	}

	return res;
}

void processInput_Run() {
	Joy_Clear();
}

void update_Run() {
	if (doRun > 0) {
		if (LynxSD_Program(romNames[selectedGame]) == FR_OK) {
			tgi_clear();
			while (tgi_busy()) ;
			tgi_updatedisplay();

			tgi_unload();
			joy_unload();
			
			LaunchROM();
		}
		else {
			changeState(GS_ERROR);
		}
	}

	doRun++;
}

void render_Run() {
	tgi_clear();

	(&logoGame)->data = gameImages[selectedGame];

  tgi_sprite(&logoGame);

  tgi_setcolor(6);
	tgi_outtextcentery(90, "-=[ LOADING ROM ]=-");
}

