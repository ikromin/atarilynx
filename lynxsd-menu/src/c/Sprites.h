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
