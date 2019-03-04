#ifndef __LYNX_SD__
#define __LYNX_SD__

#include "Types.h"

typedef enum {
	FR_OK = 0,			/* 0 */
	FR_DISK_ERR,		/* 1 */
	FR_NOT_READY,		/* 2 */
	FR_NO_FILE,			/* 3 */
	FR_NOT_OPENED,		/* 4 */
	FR_NOT_ENABLED,		/* 5 */
	FR_NO_FILESYSTEM	/* 6 */
} FRESULT;

void LynxSD_Init();
FRESULT __fastcall__ LynxSD_OpenFile(const char *pFile);
FRESULT __fastcall__ LynxSD_ReadFile(void *pBuffer, u16 nSize);
FRESULT LynxSD_CloseFile();
FRESULT __fastcall__ LynxSD_SeekFile(u32 nSeekPos);
u32 LynxSD_GetFileSize();
FRESULT __fastcall__ LynxSD_ProgramROMFromFile(u16 nStartBlock, u8 nBlockSize, u16 nBlockCount, u8 b512BlockCard);

#endif // __LYNX_SD__