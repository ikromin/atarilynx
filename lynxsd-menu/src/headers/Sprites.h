/******************************************************************************
 Lynx SD Menu Loader 2. Firmware for the Lynx SD game cartridge.

 Copyright 2019 Igor Kromin (Necrocia) - Atari Gamer

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef __LYNX_SD_SPRITES__
#define __LYNX_SD_SPRITES__

#include <tgi.h>

// sprite references
extern char img_menublr[];
extern char img_menubtb[];
extern char img_menusep[];
extern char img_menuscrl[];
extern char img_rom[];
extern char img_dir[];
extern char img_yes[];
extern char img_no[];
extern char img_loading[];
extern char img_help[];
extern char img_fail[];
extern char img_border[];

// holds preview data that is loaded from SD card
char img_preview[8365];

// thick top/bottom border
SCB_REHV_PAL borderSprite = {
	BPP_3 | TYPE_NORMAL, REHV, 0x0,
  0x0, &img_border[0],
  0, 0,
	0x0100, 0x0100,
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};

// used to draw most images like loading screens
SCB_REHV_PAL genericSprite = {
	BPP_3 | TYPE_NORMAL, REHV, 0x0,
  0x0, &img_loading[0],
  0, 0,
	0x0100, 0x0100,
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};

// used to draw file type images
SCB_REHV_PAL fileSprite = {
	BPP_3 | TYPE_BACKGROUND, REHV, 0x0,
  0x0, &img_rom[0],
  134, 0,
	0x0100, 0x0100,
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};

// used to draw the menu interface
SCB_REHV menuSprite6 = {
	BPP_3 | TYPE_BACKGROUND, REUSEPAL, 0x0,
  0x0, &img_menuscrl[0],
  153, 5,
	0x0100, 0x0100
};
SCB_REHV menuSprite5 = {
	BPP_3 | TYPE_BACKGROUND, REUSEPAL, 0x0,
  (char*) &menuSprite6, &img_menusep[0],
  5, 86,
	0x0100, 0x0100
};
SCB_REHV menuSprite4 = {
	BPP_3 | TYPE_BACKGROUND | VFLIP, REUSEPAL, 0x0,
  (char*) &menuSprite5, &img_menubtb[0],
  2, 101,
	0x0100, 0x0100
};
SCB_REHV menuSprite3 = {
	BPP_3 | TYPE_BACKGROUND, REUSEPAL, 0x0,
  (char*) &menuSprite4, &img_menubtb[0],
  2, 0,
	0x0100, 0x0100
};
SCB_REHV menuSprite2 = {
	BPP_3 | TYPE_BACKGROUND | HFLIP, REUSEPAL, 0x0,
  (char*) &menuSprite3, &img_menublr[0],
  159, 1,
	0x0100, 0x0100
};
SCB_REHV_PAL menuSprite = {
	BPP_3 | TYPE_BACKGROUND, REHV, 0x0,
  (char*) &menuSprite2, &img_menublr[0],
  0, 1,
	0x0100, 0x0100,
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};

// used to show ROM previews from LSD files
SCB_REHV_PAL previewSprite = {
	BPP_4 | TYPE_NONCOLL, LITERAL | REHV, 0x0,
  0x0, &img_preview[0],
  0, 0,
	0x100, 0x100,
	{0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF}
};

#endif // __LYNX_SD_SPRITES__
