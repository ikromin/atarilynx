#include "LynxSD.h"

////////////////////////////////////////////////////////////////////////////////
//
// Microcontroller communication constants
//
////////////////////////////////////////////////////////////////////////////////

typedef enum
{
	ECommandByte_OpenDir = 0,
	ECommandByte_ReadDir,
	ECommandByte_OpenFile,
	ECommandByte_GetSize,
	ECommandByte_Seek,
	ECommandByte_Read,
	ECommandByte_Write,
	ECommandByte_Close,
	ECommandByte_ProgramFile,
	ECommandByte_ClearBlocks,
	ECommandByte_LowPowerMode
} ECommandByte;

////////////////////////////////////////////////////////////////////////////////
//
// Lynx registers
//
////////////////////////////////////////////////////////////////////////////////

const u8 AUXMASK = 16;
#define IODAT ((volatile u8 *) 0xFD8B)
#define IODIR ((volatile u8 *) 0xFD8A)
#define CART1 ((volatile u8 *) 0xFCB3)
#define CART0 ((volatile u8 *) 0xFCB2)

////////////////////////////////////////////////////////////////////////////////
//
// Support functions to read and write bytes to the cartridge microcontroller
//
////////////////////////////////////////////////////////////////////////////////

static void __fastcall__ WriteByte(u8 byte)
{
	while (*IODAT & AUXMASK); // make sure the cart isnt blocking the write
	*CART1 = byte;
}

static void __fastcall__ WriteBytes(u8 *pBuf, u16 nSize)
{
	while (nSize--)
	{
		WriteByte(*pBuf++);
	}
}

static void __fastcall__ WriteWord(u16 word)
{
	WriteBytes((u8*) &word, 2);
}

static void __fastcall__ WriteDword(u32 dword)
{
	WriteBytes((u8*) &dword, 4);
}

static u8 ReadByte()
{
	while (!(*IODAT & AUXMASK)); // wait for data in read FIFO
	return *CART0; // read byte
}

static void __fastcall__ ReadBytes(u8 *pOut, u16 size)
{
	while (size--)
	{
		*pOut++ = ReadByte();
	}
}

static u32 ReadDword()
{
	u32 nDword;
	ReadBytes((u8*) &nDword, 4);
	return nDword;
}

static void __fastcall__ WriteString(const char *pStr)
{
	char c;
	do
	{
		c = *pStr++;
		WriteByte(c);
	} while (c);
}

////////////////////////////////////////////////////////////////////////////////
//
// Microcontroller commands
//
////////////////////////////////////////////////////////////////////////////////

void LynxSD_Init()
{
	*IODIR = 0; // all input
	*CART1 = 0xaa; // initialise cart comms
}

FRESULT __fastcall__ LynxSD_OpenFile(const char *pFile)
{
	WriteByte(ECommandByte_OpenFile); // open file command
	WriteString(pFile);
	return (FRESULT) ReadByte(); // read the response
}

FRESULT __fastcall__ LynxSD_ReadFile(void *pBuffer, u16 nSize)
{
	WriteByte(ECommandByte_Read);
	WriteWord(nSize);
	ReadBytes((u8 *) pBuffer, nSize);
	return (FRESULT) ReadByte(); // final byte is status for read
}

FRESULT LynxSD_CloseFile()
{
	WriteByte(ECommandByte_Close);
	return (FRESULT) ReadByte(); // status for close
}

FRESULT __fastcall__ LynxSD_SeekFile(u32 nSeekPos)
{
	WriteByte(ECommandByte_Seek);
	WriteDword(nSeekPos);
	return (FRESULT) ReadByte(); // seek result
}

u32 LynxSD_GetFileSize()
{
	WriteByte(ECommandByte_GetSize);
	return ReadDword(); // size of open file
}

FRESULT __fastcall__ LynxSD_ProgramROMFromFile(u16 nStartBlock, u8 nBlockSize, u16 nBlockCount, u8 b512BlockCard)
{
	if (b512BlockCard)
	{
		nBlockSize |= 0x10;
	}

	WriteByte(ECommandByte_ProgramFile);
	WriteWord(nStartBlock); // start block from 0-511 (>255 with 512 block card bit set)
	WriteByte(nBlockSize); // or with 0x10 for 512 block card (a19 controlled by aux pin)
	WriteWord(nBlockCount); // blocks to write
	return (FRESULT) ReadByte(); // waits for programming to end and gets result
}
