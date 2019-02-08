//Initial version by SainT - Retro HQ
//22-05-2016	v1.2	GadgetUK		Changed interface colours.  Added Retro HQ logo. Added support for left and right directions, flip, and B button to go back.
//26-05-2016  v1.3	GadgetUK		Added support for .LSD files (full screen icon + palette, could be extended to contain publisher info etc).
//29-05-2016  v1.4	GadgetUK		Added support for .LYX and .O files - automatic block sizing based on file size.
//29-05-2016  v1.5  GadgetUK		Removed the facility to show 20 chars of text from the end of the .LSD file - It's too slow.

#ifndef __LYNX_SD__
#define __LYNX_SD__

typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef struct
{
	char	szFilename[13];
	u8		bDirectory;
} SDirEntry;

typedef enum {
	FR_OK = 0,			/* 0 */
	FR_DISK_ERR,		/* 1 */
	FR_NOT_READY,		/* 2 */
	FR_NO_FILE,			/* 3 */
	FR_NOT_OPENED,		/* 4 */
	FR_NOT_ENABLED,		/* 5 */
	FR_NO_FILESYSTEM	/* 6 */
} FRESULT;

typedef struct {
	u32		fsize;		/* File size */
	u16		fdate;		/* Last modified date */
	u16		ftime;		/* Last modified time */
	u8		fattrib;	/* Attribute */
	char	fname[13];	/* File name */
} SFileInfo;

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_MASK	0x3F	/* Mask of defined bits */

void LynxSD_Init();
void LynxSD_LowPowerMode();
FRESULT __fastcall__ LynxSD_OpenDir(const char *pDir);
FRESULT __fastcall__ LynxSD_ReadDir(SFileInfo *pInfo);
FRESULT __fastcall__ LynxSD_OpenFile(const char *pFile);
FRESULT __fastcall__ LynxSD_ReadFile(void *pBuffer, u16 nSize);
FRESULT __fastcall__ LynxSD_WriteFile(void *pBuffer, u16 nSize);
FRESULT LynxSD_CloseFile();
FRESULT __fastcall__ LynxSD_SeekFile(u32 nSeekPos);
u32 LynxSD_GetFileSize();
FRESULT __fastcall__ LynxSD_ProgramROMFromFile(u16 nStartBlock, u8 nBlockSize, u16 nBlockCount, u8 b512BlockCard);
FRESULT __fastcall__ LynxSD_ClearROMBlocks(u16 nStartBlock, u16 nBlocks, u8 b512BlockCard);

#endif // __LYNX_SD__