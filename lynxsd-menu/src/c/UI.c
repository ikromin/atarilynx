#include <tgi.h>
#include <string.h>
#include "UI.h"
#include "Directory.h"
#include "Sprites.h"

/*
 ******************************************************************************
 Functions for rendering the user interface
 ******************************************************************************
 */


#define TXT_LASTROM "Loading last ROM..."
#define TXT_STANDBY "Please stand by"
#define TXT_LOADING_DIR "Loading ROM list..."
#define TXT_LOADING "Loading..."
#define TXT_LOADING_PREVIEW "Loading preview..."
#define TXT_PROGRAMMING "Loading ROM..."
#define TXT_FAIL_LOAD "Error loading!"
#define TXT_ROOT_DIR "/"

#define WAIT_TGI while (tgi_busy());
#define TGI_CENTER_ECHO(y, t) tgi_outtextxy((160 - (strlen(t) * 8)) / 2, y, t)

extern u8 joystickActionDelay;
extern char gszCurrentDir[256];

static char* fileTypesMap[] = {
  &img_rom[0], &img_dir[0], &img_no[0], &img_yes[0]
};

// pallete colours in 0x0GBR format, 4 rows of 4 colours each
u8 masterPal[] = {
  // green values
  0x0000 >> 8,    0x030b >> 8,    0x0333 >> 8,    0x0444 >> 8,
  0x0999 >> 8,    0x0115 >> 8,    0x0aaa >> 8,    0x0aaa >> 8,
  0x0aaa >> 8,    0x0aaa >> 8,    0x0aaa >> 8,    0x0aaa >> 8,
  0x0aaa >> 8,    0x0aaa >> 8,    0x0aaa >> 8,    0x0aaa >> 8,
  // blue and red values
  0x0000 & 0xff,  0x030b & 0xff,  0x0333 & 0xff,  0x0444 & 0xff,	
  0x0999 & 0xff,  0x0115 & 0xff,  0x0aaa & 0xff,  0x0aaa & 0xff,
  0x0aaa & 0xff,  0x0aaa & 0xff,  0x0aaa & 0xff,  0x0aaa & 0xff,	
  0x0aaa & 0xff,  0x0aaa & 0xff,  0x0aaa & 0xff,  0x0aaa & 0xff
};

static u8 blackPal[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static u8 previewPal[16];

static u8 currentUiLine = 0;


/**
 * Draws the top and bottom 'thick' borders.
 */
static void drawBorders() {
  SCB_REHV_PAL* spritePtr;

  spritePtr = &borderSprite;
  spritePtr->sprctl0 = BPP_3 | TYPE_NORMAL;
  spritePtr->vpos = 0;
  tgi_sprite(spritePtr);

  spritePtr = &borderSprite;
  spritePtr->sprctl0 = BPP_3 | TYPE_NORMAL | VFLIP;
  spritePtr->vpos = 101;
  tgi_sprite(spritePtr);
}


/**
 * Draws the loading screen with a top and bottom centered text.
 */
static void drawLoadingScreen(char topLine[], char bottomLine[]) {
  SCB_REHV_PAL* spritePtr;

  drawBorders();

  spritePtr = &genericSprite;
  spritePtr->data = &img_loading[0];
  spritePtr->vpos = 26;
  spritePtr->hpos = 26;
  tgi_sprite(spritePtr);

  tgi_setcolor(4);
  TGI_CENTER_ECHO(1, topLine);
  TGI_CENTER_ECHO(93, bottomLine);
}


/**
 * Screen to display when a previous ROM is being loaded on startup. 
 **/
void __fastcall__ UI_showLastRomScreen(char romFileName[]) {
  WAIT_TGI
  tgi_clear();

  drawLoadingScreen(TXT_LASTROM, romFileName);
	tgi_updatedisplay();
}


/**
 * Screen to display when the ROM list is loading.
 */
void UI_showLoadingDirScreen() {
  WAIT_TGI
  tgi_clear();

  drawLoadingScreen(TXT_LOADING_DIR, TXT_STANDBY);
	tgi_updatedisplay();
}


/**
 * Screen to display when a ROM preview is selected. Will first
 * show a loading screen and then if the LSD preview file exists it
 * will be shown, otherwise an error screen will be shown.
 */
u8 UI_showPreviewScreen() {
  SDirEntry* dirEntry = &gsDirEntry[ganDirOrder[gnSelectIndex]];
  char previewFile[256];
  char* i;
  u8 fail = 1;

  // previews can only be shown for file type entries
  if (dirEntry->bDirectory != 0) {
    return 0;
  }
  
  WAIT_TGI
  tgi_clear();

  drawLoadingScreen(TXT_LOADING_PREVIEW, dirEntry->szFilename);
	tgi_updatedisplay();

  // LSD file name is the ROM file name with its extension set to LSD
  DIR_FullRomPath(previewFile, dirEntry->szFilename);
  i = strrchr(previewFile, '.');
  if (i) strcpy(i + 1, "LSD");

	if (LynxSD_OpenFile(previewFile) == FR_OK) {
		if (LynxSD_ReadFile(img_preview, 8365) == FR_OK) {
			if (LynxSD_ReadFile(previewPal, 32) == FR_OK) {
        fail = 0;
			}
		}
		LynxSD_CloseFile();
	}

  if (fail) {
    tgi_setpalette(masterPal);
    UI_showFailScreen(previewFile);
  }
  else {
    WAIT_TGI
    tgi_clear();
    tgi_setpalette(previewPal);
    tgi_sprite(&previewSprite);
    tgi_updatedisplay();
  }

  return 1;
}


/**
 * Screen to display when a ROM file is being programmed into the cart.
 */
void UI_showProgrammingScreen() {
  SDirEntry* dirEntry = &gsDirEntry[ganDirOrder[gnSelectIndex]];

  WAIT_TGI
  tgi_clear();

  drawLoadingScreen(TXT_PROGRAMMING, dirEntry->szFilename);
  tgi_updatedisplay();
}


/**
 * Screen to display during an error condition.
 */
void __fastcall__ UI_showFailScreen(char* fileName) {
  SCB_REHV_PAL* spritePtr;
  SDirEntry* dirEntry = &gsDirEntry[ganDirOrder[gnSelectIndex]];

  WAIT_TGI
  tgi_clear();

  drawBorders();

  spritePtr = &genericSprite;
  spritePtr->data = &img_fail[0];
  spritePtr->vpos = 26;
  spritePtr->hpos = 26;
  tgi_sprite(spritePtr);

  tgi_setcolor(4);
  TGI_CENTER_ECHO(1, TXT_FAIL_LOAD);
  TGI_CENTER_ECHO(93, fileName == 0 ? dirEntry->szFilename : fileName);

  tgi_updatedisplay();
}


/**
 * Screen to show when the Opt2 button is pressed for help.
 */
void UI_showHelpScreen() {
  SCB_REHV_PAL* spritePtr;

  WAIT_TGI
  tgi_clear();
  
  spritePtr = &genericSprite;
  spritePtr->data = &img_help[0];
  spritePtr->vpos = 0;
  spritePtr->hpos = 0;
  tgi_sprite(&genericSprite);
  
  tgi_updatedisplay();
}


/**
 * Sets up the palette and background colour and clears the display.
 */
void UI_init() {
  tgi_setpalette(masterPal);
	tgi_setbgcolor(0);

  tgi_clear();
  tgi_updatedisplay();
}


/**
 * Sets the palette to all black and clears the display.
 */
void UI_clear() {
	tgi_setpalette(blackPal);
	tgi_clear();
}


/**
 * Resets the palette to the default.
 */
void UI_resetPalette() {
  tgi_setpalette(masterPal);
}


void UI_selectPrevious() {
  if (gnSelectIndex > 0) gnSelectIndex--;
  if (currentUiLine > 0) currentUiLine--;
}

void UI_selectNext() {
  if (gnSelectIndex < gnNumDirEntries - 1) gnSelectIndex++;
	if (currentUiLine < MAX_UI_LINES && currentUiLine < gnNumDirEntries - 1) currentUiLine++;
}


void UI_selectPrevious2() {
  u8 validFirstLine = gnSelectIndex % MAX_UI_LINES;
  
  if (gnSelectIndex < MAX_UI_LINES) gnSelectIndex = 0;
	else gnSelectIndex -= MAX_UI_LINES;
  
  if (currentUiLine > gnSelectIndex) currentUiLine = validFirstLine;
}


void UI_selectNext2() {
	u8 validLastLine = gnSelectIndex % MAX_UI_LINES;

  if (gnSelectIndex + MAX_UI_LINES >= gnNumDirEntries) gnSelectIndex = gnNumDirEntries - 1;
	else gnSelectIndex += MAX_UI_LINES;

  if (currentUiLine > validLastLine) currentUiLine = validLastLine;
}


void UI_forwardAction() {
	currentUiLine = 0;
}


void UI_backAction() {
	currentUiLine = 0;
}


void UI_render() {
	SDirEntry* dirEntry;
  u32 scrollPos = (7400 * (u32) gnSelectIndex) / (100 * (u32) (gnNumDirEntries - 1));
  u8 highlightOffset = 10 * currentUiLine;
  u8 curLine, startIndex, lineIndex;

	tgi_clear();
  tgi_sprite(&menuSprite);

  // current selection highlight
  tgi_setcolor(1);
  tgi_bar(5, highlightOffset + 5, 133, highlightOffset + 13);

  tgi_setcolor(4);

  startIndex = gnSelectIndex - currentUiLine; // start drawing n lines before current line
  for (curLine = 0; curLine <= MAX_UI_LINES; curLine++) {
    // work out the current line directory index
    lineIndex = startIndex + curLine;
    if (lineIndex >= gnNumDirEntries) break;

    dirEntry = &gsDirEntry[ganDirOrder[lineIndex]];
  
    // sprite data picked from file type map
    (&fileSprite)->data = fileTypesMap[dirEntry->bDirectory];

    tgi_outtextxy(5, (curLine * 10) + 6, dirEntry->szFilename);
    (&fileSprite)->vpos = (curLine * 10) + 5;
    tgi_sprite(&fileSprite);
  }

  // display current directory
  tgi_outtextxy(5, 89, (gszCurrentDir[0] == 0 ? TXT_ROOT_DIR : gszCurrentDir));

  tgi_setcolor(1);
  tgi_bar(153, 5 + scrollPos, 154, 9 + scrollPos); // scrollbar indicator

	tgi_updatedisplay();
}
