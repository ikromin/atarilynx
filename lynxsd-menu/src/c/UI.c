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
#define TXT_LOADING "Loading ROM list..."
#define TXT_PROGRAMMING "Loading ROM..."
#define TXT_FAIL_LOAD "Failed to load ROM"
#define TXT_ROOT_DIR "/"

#define WAIT_TGI while (tgi_busy());
#define TGI_CENTER_ECHO(y, t) tgi_outtextxy((160 - (strlen(t) * 8)) / 2, y, t)

extern u8 joystickActionDelay;
extern char gszCurrentDir[256];

static char* fileTypesMap[] = {
  &img_rom[0], &img_dir[0], &img_no[0], &img_yes[0]
};

// pallete colours in 0x0GBR format, 4 rows of 4 colours each
unsigned char masterpal[] = {
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

static unsigned char blackpal[16];
static unsigned char currentpal[16];

static unsigned char currentUiLine = 0;


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
void UI_showLastRomScreen(char romFileName[]) {
  WAIT_TGI
  tgi_clear();

  drawLoadingScreen(TXT_LASTROM, romFileName);
	tgi_updatedisplay();
}

/**
 * Screen to display when the ROM list is loading.
 */
void UI_showLoadingScreen() {
  WAIT_TGI
  tgi_clear();

  drawLoadingScreen(TXT_LOADING, TXT_STANDBY);
	tgi_updatedisplay();
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
void UI_showFailScreen() {
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
  TGI_CENTER_ECHO(93, dirEntry->szFilename);

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
 * Sets up initial palletes by creating an all black colour pallete and copying
 * the master pallete into the current pallete.
 */
void UI_init() {
  unsigned char i = 0;
  for (i = 0; i < 16; i++) {
    blackpal[i] = 0x00;
    currentpal[i] = masterpal[i];
  }

  tgi_setpalette(masterpal);
	tgi_setbgcolor(0);

  tgi_clear();
  tgi_updatedisplay();
}


void UI_clear() {
	tgi_setpalette(blackpal);
	tgi_clear();
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
  u32 scrollPos = (7500 * (u32) gnSelectIndex) / (100 * (u32) gnNumDirEntries);
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
