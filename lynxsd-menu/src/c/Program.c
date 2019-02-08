#include "Program.h"

/*
 ******************************************************************************
 Functions for programming a ROM into cart SRAM from LNX, LYX and O files
 ******************************************************************************
 */


// Lynx registers
#define MSTERE0 ((volatile u8 *) 0xFD50)
#define MAPCTL ((volatile u8 *) 0xFFF9)


// Object loader from David Huseby and Karri Kaksonen
static u8 gObjectLoader[] =
{
	0xff, 0x81, 0xca, 0x33, 0xbe, 0x80, 0xa2, 0xc4, 0x6d, 0x98, 0xfe, 0x8d, 0xbc, 0x66, 0xc0, 0x7a,
	0x09, 0x50, 0x23, 0x28, 0x18, 0xc8, 0x06, 0x70, 0x58, 0x4f, 0x1b, 0xe1, 0xc7, 0x90, 0x08, 0xcd,
	0x1a, 0x6e, 0x5a, 0x45, 0x32, 0xd7, 0x6d, 0xc6, 0x8a, 0xe5, 0xd8, 0x5c, 0xa0, 0xe8, 0x4f, 0x7a,
	0x5f, 0x73, 0x8d, 0x22,
	0xa2, 0x00, 0xa0, 0x08, 0xad, 0xb2, 0xfc, 0x95, 0x26, 0xe8, 0x88, 0xd0,
	0xf7, 0xa5, 0x26, 0x85, 0x2e, 0x20, 0xca, 0xfb, 0xa5, 0x28, 0x49, 0xff, 0xa8, 0xa5, 0x27, 0x49,
	0xff, 0xaa, 0x20, 0xa1, 0xfb, 0xa5, 0x2a, 0xa6, 0x2b, 0x85, 0x31, 0x86, 0x32, 0xa5, 0x2d, 0x49,
	0xff, 0xa8, 0xa5, 0x2c, 0x49, 0xff, 0xaa, 0x20, 0xac, 0xfb, 0x6c, 0x2a, 0x00, 0xe8, 0xd0, 0x03,
	0xc8, 0xf0, 0x57, 0x20, 0xbf, 0xfb, 0x80, 0xf5, 0xe8, 0xd0, 0x03, 0xc8, 0xf0, 0x4c, 0x20, 0xbf,
	0xfb, 0x92, 0x31, 0xe6, 0x31, 0xd0, 0xf1, 0xe6, 0x32, 0x80, 0xed, 0xad, 0xb2, 0xfc, 0xe6, 0x2f,
	0xd0, 0x38, 0xe6, 0x30, 0xd0, 0x34, 0x48, 0xda, 0x5a, 0xa5, 0x1a, 0x29, 0xfc, 0xa8, 0x09, 0x02,
	0xaa, 0xa5, 0x2e, 0xe6, 0x2e, 0x38, 0x80, 0x0b, 0x90, 0x04, 0x8e, 0x8b, 0xfd, 0x18, 0xe8, 0x8e,
	0x87, 0xfd, 0xca, 0x8e, 0x87, 0xfd, 0x2a, 0x8c, 0x8b, 0xfd, 0xd0, 0xec, 0xa5, 0x1a, 0x8d, 0x8b,
	0xfd, 0x64, 0x2f, 0xa9, 0xf8, 0x85, 0x30, 0x7a, 0xfa, 0x68, 0x60,
	0x01, // block
	0x00, 0x00, // offset
	0x88, // executable
	0x00, 0x00, // load address
	0x00, 0x00 // size
};


FRESULT __fastcall__ LynxSD_Program(const char *pFilename)
{
	FRESULT res = FR_DISK_ERR;

	if (LynxSD_OpenFile(pFilename) == FR_OK)
	{
		char szHeader[10];
		LynxSD_ReadFile(szHeader, 10);

	  //-- Really an object file incorrectly named
		if ((szHeader[6] == 'B' && szHeader[7] == 'S' && szHeader[8] == '9' && szHeader[9] == '3'))
		{
			LynxSD_CloseFile();
			return LynxSD_ProgramHomebrew(pFilename);
		}

	  //-- Otherwise check for actual LNX ROM file
		else if ((szHeader[0] == 'L' && szHeader[1] == 'Y' && szHeader[2] == 'N' && szHeader[3] == 'X'))
		{
			u8 nBlockSize;
			u16 nBlockSizeBytes;
			u32 nSize;
			u16 nBlockCount;
			
			nBlockSize = szHeader[5];

			if ((nBlockSize == 1 || nBlockSize == 2 || nBlockSize == 4 || nBlockSize == 8))
			{
				nSize = LynxSD_GetFileSize();
				nSize -= 64;

				LynxSD_SeekFile(64);

				nBlockSizeBytes = nBlockSize << 8;
				nBlockCount = (nSize + nBlockSizeBytes - 1) / nBlockSizeBytes;

				res = LynxSD_ProgramROMFromFile(0, nBlockSize, nBlockCount, 0);
				LynxSD_CloseFile();
			}
		}

	  //-- Finally just try as a raw LYX file
		else
		{
			LynxSD_CloseFile();
			return LynxSD_ProgramLYX(pFilename);
		}
	}
	return res;
}


FRESULT __fastcall__ LynxSD_ProgramLYX(const char *pFilename)
{
	FRESULT res = FR_DISK_ERR;
	if (LynxSD_OpenFile(pFilename) == FR_OK)
	{
		u8 nBlockSize;
		u16 nBlockSizeBytes;
		u32 nSize;
    u16 nBlockCount;
	
		nSize = LynxSD_GetFileSize();
		switch (nSize)
		{
			case 65536:
				nBlockSize = 1; //1 = 64?
				break;
			case 131072:
				nBlockSize = 2; //2 = 128
				break;
			case 262144:
				nBlockSize = 4; //4 = 256
				break;
			case 524288:
				nBlockSize = 8; //8 = 512?
				break;
			default:
				nBlockSize = 4;
				break;
		}
		
		nBlockSizeBytes = nBlockSize << 8;
		nBlockCount = (nSize + nBlockSizeBytes - 1) / nBlockSizeBytes;

		res = LynxSD_ProgramROMFromFile(0, nBlockSize, nBlockCount, 0);

		LynxSD_CloseFile();
	}
	return res;
}


FRESULT __fastcall__ LynxSD_ProgramHomebrew(const char *pFilename)
{
  FRESULT res = FR_DISK_ERR;
  u16 nDelay = 65535;

  //-- Try and open homebrew file
  if (LynxSD_OpenFile(pFilename) == FR_OK)
  {
      char szHeader[10];

      //-- Read in the expected 10 bytes header
      LynxSD_ReadFile(szHeader, 10);
      LynxSD_CloseFile();

      //-- If this is a homebrew patch the loader
      if ((szHeader[6] == 'B' && szHeader[7] == 'S' && szHeader[8] == '9' && szHeader[9] == '3'))
      {
        //-- Open up the loader
        if (LynxSD_OpenFile("menu/homebrew") == FR_OK)
        {
          u16 nSize = LynxSD_GetFileSize();
          if (nSize >= sizeof(gObjectLoader))
          {
            nSize = ((szHeader[4] << 8) | szHeader[5]) - 10; // object size

            //-- Sort out byte ordering expected by the loader
            gObjectLoader[0xcf] = szHeader[3]; // load address, LSB first
            gObjectLoader[0xd0] = szHeader[2];
            gObjectLoader[0xd1] = (u8) nSize; // size, LSB first
            gObjectLoader[0xd2] = (u8) (nSize >> 8);

            //-- Write out loader
            LynxSD_WriteFile(gObjectLoader, sizeof(gObjectLoader));
            LynxSD_CloseFile(); // flush written data
  
            while (nDelay--);
            LynxSD_OpenFile("menu/homebrew");

            //-- Program the homebrew object boot loader
            if (LynxSD_ProgramROMFromFile(0, 8, 1, 0) == FR_OK)
            {
              u16 nBlocks = (nSize + 2047) >> 11; // object blocks

              LynxSD_CloseFile();

              //-- Program the actual homebrew object
              LynxSD_OpenFile(pFilename); // already opened, shouldnt fail
              LynxSD_SeekFile(10); // skip header
              if (LynxSD_ProgramROMFromFile(1, 8, nBlocks, 0) == FR_OK)
              {
                res = FR_OK; // all done, we should be able to boot as a ROM now
              }
            }
          }
          LynxSD_CloseFile();
        }
      }
    }
  return res;
}


void LaunchROM()
{
		u8 *ptr;
		u8 count;

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
