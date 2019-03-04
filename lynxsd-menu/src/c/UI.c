#include <tgi.h>
#include <string.h>
#include <stdlib.h>
#include "UI.h"
#include "Directory.h"
#include "Preferences.h"
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
#define TXT_NO_ROMS "** NO ROMS **"

#define TXT_ROOT_DIR "/"
#define TXT_OLD_PREVIEW_DIR "_PREVIEW/"

#define PALETTE_FILE "menu/default.pal"

#define SKIP_ANIM_FRAMES 5

extern char gszCurrentDir[256];
static char curentDirPart[19];

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
static u8 curDirDispOffset = 0;
static u8 curRomDispOffset = 0;
static s8 curDirDispScrollDir = -1;
static s8 curRomDispScrollDir = -1;
static u8 skipAnimFrames = 0;

static SDirEntry* dirEntry;

static SCB_REHV_PAL* spritePtr;


static void waitTgi() {
  while (tgi_busy());
  tgi_clear();
}


static void tgi_outtextcentery(u8 y, char* t) {
  tgi_outtextxy((160 - (strlen(t) * 8)) / 2, y, t);
}


/**
 * Draws the top and bottom 'thick' borders.
 */
static void drawBorders() {
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
  drawBorders();

  spritePtr = &genericSprite;
  spritePtr->data = &img_loading[0];
  spritePtr->vpos = 26;
  spritePtr->hpos = 26;
  tgi_sprite(spritePtr);

  tgi_setcolor(4);
  tgi_outtextcentery(1, topLine);
  tgi_outtextcentery(93, bottomLine);
}


/**
 * Screen to display when a previous ROM is being loaded on startup. 
 **/
void __fastcall__ UI_showLastRomScreen(char romFileName[]) {
  waitTgi();

  drawLoadingScreen(TXT_LASTROM, romFileName);
	tgi_updatedisplay();
}


/**
 * Screen to display when the ROM list is loading.
 */
void UI_showLoadingDirScreen() {
  waitTgi();

  drawLoadingScreen(TXT_LOADING_DIR, TXT_STANDBY);
	tgi_updatedisplay();
}


/**
 * Screen to display when a ROM preview is selected. Will first
 * show a loading screen and then if the LSD preview file exists it
 * will be shown, otherwise an error screen will be shown.
 */
u8 UI_showPreviewScreen() {
  char previewFile[256];
  char* i;
  u8 fail = 1;

  dirEntry = &gsDirEntry[ganDirOrder[gnSelectIndex]];

  // previews can only be shown for file type entries
  if (dirEntry->bDirectory != 0) {
    return 0;
  }
  
  waitTgi();

  drawLoadingScreen(TXT_LOADING_PREVIEW, dirEntry->szFilename);
	tgi_updatedisplay();

  // get path to ROM file based on preferences
  if (preferences[PREF_OLD_PREVIEW]) {
    strcpy(previewFile, TXT_OLD_PREVIEW_DIR);
    strcat(previewFile, dirEntry->szFilename);
  }
  else {
      DIR_FullFilePath(previewFile, dirEntry->szFilename);
  }

  // LSD file name is the ROM file name with its extension set to LSD
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
    if (strlen(previewFile) > 19) previewFile[18] = 0; // truncate to fit on screen
    UI_showFailScreen(previewFile);
  }
  else {
    waitTgi();
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

  waitTgi();

  drawLoadingScreen(TXT_PROGRAMMING, dirEntry->szFilename);
  tgi_updatedisplay();
}


/**
 * Screen to display during an error condition.
 */
void __fastcall__ UI_showFailScreen(char* fileName) {
  dirEntry = &gsDirEntry[ganDirOrder[gnSelectIndex]];

  waitTgi();

  drawBorders();

  spritePtr = &genericSprite;
  spritePtr->data = &img_fail[0];
  spritePtr->vpos = 26;
  spritePtr->hpos = 26;
  tgi_sprite(spritePtr);

  tgi_setcolor(4);
  tgi_outtextcentery(1, TXT_FAIL_LOAD);
  tgi_outtextcentery(93, fileName == 0 ? dirEntry->szFilename : fileName);

  tgi_updatedisplay();
}


/**
 * Screen to show when the Opt2 button is pressed for help.
 */
void UI_showHelpScreen() {
  waitTgi();
  
  spritePtr = &genericSprite;
  spritePtr->data = &img_help[0];
  spritePtr->vpos = 0;
  spritePtr->hpos = 0;
  tgi_sprite(&genericSprite);
  
  tgi_updatedisplay();
}


void UI_showInitScreen() {
  waitTgi();
  tgi_setpalette(masterPal);

  tgi_setcolor(1);
  tgi_outtextcentery(41, "Lynx SD");
  tgi_outtextcentery(51, "Menu Loader 2");

  tgi_setcolor(2);
  tgi_outtextcentery(26, "retrohq.co.uk");
  tgi_outtextcentery(66, "atarigamer.com");

  tgi_updatedisplay();
}


void UI_showCreditsScreen() {
  waitTgi();

  tgi_setcolor(1);
  tgi_outtextcentery(5, "Lynx SD");
  tgi_outtextcentery(15, "Menu Loader 2");

  tgi_setcolor(4);
  tgi_outtextcentery(30, "Original code by");
  tgi_outtextcentery(40, "SainT");

  tgi_outtextcentery(55, "v1 Improvements");
  tgi_outtextcentery(65, "GadgetUK");

  tgi_outtextcentery(80, "v2 Redesign");
  tgi_outtextcentery(90, "Necrocia");

  tgi_updatedisplay();
}


/**
 * Sets up the palette and background colour and clears the display.
 */
void UI_init() {
  // try to read in the palette file if it exists
  if (LynxSD_OpenFile(PALETTE_FILE) == FR_OK) {
		LynxSD_ReadFile(masterPal, 32);
    LynxSD_CloseFile();
  }

  tgi_setpalette(masterPal);
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

  curRomDispOffset = 0;
  curRomDispScrollDir = -1;
}

void UI_selectNext() {
  if (gnSelectIndex < gnNumDirEntries - 1) gnSelectIndex++;
	if (currentUiLine < MAX_UI_LINES && currentUiLine < gnNumDirEntries - 1) currentUiLine++;

  curRomDispOffset = 0;
  curRomDispScrollDir = -1;
}


void UI_selectPrevious2() {
  if (gnSelectIndex < MAX_UI_LINES) gnSelectIndex = 0;
	else gnSelectIndex -= MAX_UI_LINES;
  
  if (currentUiLine > gnSelectIndex) currentUiLine = gnSelectIndex;

  curRomDispOffset = 0;
  curRomDispScrollDir = -1;
}


void UI_selectNext2() {
	u8 validLastLine = gnSelectIndex % MAX_UI_LINES;

  if (gnSelectIndex + MAX_UI_LINES >= gnNumDirEntries) gnSelectIndex = gnNumDirEntries - 1;
	else gnSelectIndex += MAX_UI_LINES;

  if (currentUiLine > validLastLine) currentUiLine = validLastLine;

  curRomDispOffset = 0;
  curRomDispScrollDir = -1;
}


void UI_forwardAction() {
	currentUiLine = 0;
  curDirDispOffset = 0;
  curDirDispScrollDir = -1;
}


void UI_backAction() {
	currentUiLine = 0;
  curDirDispOffset = 0;
  curDirDispScrollDir = -1;
}


void UI_showDirectory() {
  u8 curLine, startIndex, lineIndex;
  u8 romLen = 0;
  u32 scrollPos = (7400 * (u32) gnSelectIndex) / (100 * (u32) (gnNumDirEntries - 1));
  u8 dirLen = strlen(gszCurrentDir);

	tgi_clear();
  tgi_sprite(&menuSprite);

  // draw directory entries if we have any to show
  if (gnNumDirEntries) {
    // current selection highlight
    tgi_setcolor(1);
    tgi_bar(5, (10 * currentUiLine) + 5, 133, (10 * currentUiLine) + 13);

    tgi_setcolor(4);

    startIndex = gnSelectIndex - currentUiLine; // start drawing n lines before current line
    for (curLine = 0; curLine <= MAX_UI_LINES; curLine++) {
      // work out the current line directory index
      lineIndex = startIndex + curLine;
      if (lineIndex >= gnNumDirEntries) {
        // draw blank lines so when there are few directory entries, it takes
        // about the same amount of time to draw each frame (required for scroll animations)
        tgi_outtextxy(5, (curLine * 10) + 6, "                ");
        continue;
      }

      dirEntry = &gsDirEntry[ganDirOrder[lineIndex]];
    
      // sprite data picked from file type map
      (&fileSprite)->data = fileTypesMap[dirEntry->bDirectory];

      // display a directory entry, scroll if currently selected
      romLen = strlen(dirEntry->szLongName);
      if (romLen > 16) {
        if (currentUiLine == curLine) {
          strncpy(curentDirPart, &(dirEntry->szLongName[curRomDispOffset]), 16);

          if (skipAnimFrames == 0) {
            if (curRomDispOffset + 16 > romLen || curRomDispOffset == 0) curRomDispScrollDir *= -1;
            curRomDispOffset += curRomDispScrollDir;
          }
        }
        else strncpy(curentDirPart, dirEntry->szLongName, 16);

        tgi_outtextxy(5, (curLine * 10) + 6, curentDirPart);
      }
      else tgi_outtextxy(5, (curLine * 10) + 6, dirEntry->szLongName);

      (&fileSprite)->vpos = (curLine * 10) + 5;
      tgi_sprite(&fileSprite);
    }
  }
  else {
    tgi_outtextxy(5, 6, TXT_NO_ROMS);
  }

  // display current directory
  tgi_setcolor(4);
  if (dirLen > 18) {
    strncpy(curentDirPart, &gszCurrentDir[curDirDispOffset], 18);
    tgi_outtextxy(5, 89, curentDirPart);
  }
  else tgi_outtextxy(5, 89, (gszCurrentDir[0] == 0 ? TXT_ROOT_DIR : gszCurrentDir));

  tgi_setcolor(1);
  tgi_bar(153, 5 + scrollPos, 154, 9 + scrollPos); // scrollbar indicator

	tgi_updatedisplay();

  if (skipAnimFrames++ > SKIP_ANIM_FRAMES) {
    skipAnimFrames = 0;

    if (curDirDispOffset + 18 > dirLen || curDirDispOffset == 0) curDirDispScrollDir *= -1;
    curDirDispOffset += curDirDispScrollDir;
  }
}
